/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Paul Asmuth <paul@asmuth.com>
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>
#include <set>
#include <vector>
#include "tsdb.h"
#include "page_index.h"
#include "varint.h"

namespace tsdb {

struct FlushedPage {
  PageMap::PageIDType page_id;
  uint64_t version;
  uint64_t disk_addr;
  uint64_t disk_size;
};

struct FlushedIndex {
  uint64_t series_id;
  uint64_t version;
  uint64_t disk_addr;
  uint64_t disk_size;
};

struct IndexPage {
  uint64_t addr;
  uint64_t size;
};

bool TSDB::commit() {
  std::unique_lock<std::mutex> lk(commit_mutex_);
  std::vector<FlushedPage> flushed_pages;
  std::vector<IndexPage> index_pages;

  /* get a snapshot of all series */
  bool all_series_clean = true;
  std::set<uint64_t> series_ids;
  txn_map_.listSlots(&series_ids);

  /* write the transaction header */
  std::string txn_data;
  writeVarUInt(&txn_data, 0); // flags
  writeVarUInt(&txn_data, bsize_);

  /* write the pages and partition indexes for each series */
  for (auto siter = series_ids.begin(); siter != series_ids.end(); ) {
    auto series_id = *siter;

    /* get a snapshot of the series */
    Transaction txn;
    if (!txn_map_.startTransaction(series_id, true, &txn)) {
      /* if we can't start the transaction, the series was deleted while
         our commit is running, so we ignore it */
      siter = series_ids.erase(siter);
      continue;
    } else {
      ++siter;
    }

    /* write the series index data header */
    std::string index_data;
    auto page_idx = txn.getPageIndex();
    writeVarUInt(&index_data, (uint64_t) page_idx->getType());
    writeVarUInt(&index_data, (uint64_t) page_idx->getMetadata().size());
    index_data.append(page_idx->getMetadata());
    writeVarUInt(&index_data, page_idx->getSize());

    for (size_t i = 1; i < page_idx->getSize(); ++i) {
      auto point = page_idx->getSplitpoints()[i - 1].point;
      writeVarUInt(&index_data, point); // FIXME use delta encoding
    }

    /* write each page to disk */
    bool all_pages_clean = true;
    for (size_t i = 0; i < page_idx->getSize(); ++i) {
      auto page_id = page_idx->getEntries()[i].page_id;
      PageInfo page_info;

      if (!page_map_.getPageInfo(page_id, &page_info)) {
        return false;
      }

      if (page_info.is_dirty) {
        all_pages_clean = false;

        PageBuffer page_buf(page_idx->getType());
        page_map_.getPage(page_id, &page_buf);

        std::string page_data;
        page_buf.encode(&page_data);
        assert(!page_data.empty());

        if (!allocPage(
              page_data.size(),
              &page_info.disk_addr,
              &page_info.disk_size)) {
          return false;
        }

        auto rc = pwrite(
            fd_,
            page_data.data(),
            page_data.size(),
            page_info.disk_addr);

        if (rc <= 0) {
          return false;
        }

        FlushedPage flushed_page;
        flushed_page.page_id = page_id;
        flushed_page.version = page_info.version;
        flushed_page.disk_addr = page_info.disk_addr;
        flushed_page.disk_size = page_info.disk_size;
        flushed_pages.emplace_back(flushed_page);
      }

      /* append the pages position to the series index */
      assert(page_info.disk_addr % bsize_ == 0);
      assert(page_info.disk_size % bsize_ == 0);
      writeVarUInt(&index_data, page_info.disk_addr / bsize_);
      writeVarUInt(&index_data, page_info.disk_size / bsize_);
    }

    uint64_t index_disk_addr;
    uint64_t index_disk_size;
    if (page_idx->hasDiskSnapshot() && all_pages_clean) {
      /* reuse the previous series index */
      page_idx->getDiskSnapshot(&index_disk_addr, &index_disk_size);
    } else {
      all_series_clean = false;

      /* write the series index to disk*/
      if (!allocPage(index_data.size(), &index_disk_addr, &index_disk_size)) {
        return false;
      }

      auto rc = pwrite(
          fd_,
          index_data.data(),
          index_data.size(),
          index_disk_addr);

      if (rc <= 0) {
        return false;
      }

      IndexPage index_page;
      index_page.addr = index_disk_addr;
      index_page.size = index_disk_size;
      index_pages.emplace_back(index_page);

      page_idx->setDiskSnapshot(index_disk_addr, index_disk_size);
    }

    /* append the series index position to the transaction */
    writeVarUInt(&txn_data, series_id);
    assert(index_disk_addr % bsize_ == 0);
    assert(index_disk_size % bsize_ == 0);
    writeVarUInt(&txn_data, index_disk_addr / bsize_);
    writeVarUInt(&txn_data, index_disk_size / bsize_);
  }

  /* append the eof marker to the transaction */
  writeVarUInt(&txn_data, 0);

  /* if nothing has changed, bail out */
  if (all_series_clean) {
    return true;
  }

  /* write the new transaction to disk */
  uint64_t txn_disk_addr;
  uint64_t txn_disk_size;
  if (!allocPage(txn_data.size(), &txn_disk_addr, &txn_disk_size)) {
    return false;
  }

  auto rc = pwrite(fd_, txn_data.data(), txn_data.size(), txn_disk_addr);
  if (rc <= 0) {
    return false;
  }

  /* fsync all changes before comitting the new transaction */
#ifdef HAVE_FDATASYNC
  if (fdatasync(fd_) != 0) {
    return false;
  }
#else
  if (fsync(fd_) != 0) {
    return false;
  }
#endif

  /* commit the new transaction */
  std::string commit_data(kMagicBytes, sizeof(kMagicBytes));
  writeVarUInt(&commit_data, txn_disk_addr);
  writeVarUInt(&commit_data, txn_disk_size);
  writeVarUInt(&commit_data, fpos_);

  rc = pwrite(fd_, commit_data.data(), commit_data.size(), 0);
  if (rc <= 0) {
    return false;
  }

#ifdef HAVE_FDATASYNC
  if (fdatasync(fd_) != 0) {
    return false;
  }
#else
  if (fsync(fd_) != 0) {
    return false;
  }
#endif

  /* advise the kernel that we are not going to read back the index */
#ifdef HAVE_POSIX_FADVISE
  posix_fadvise(fd_, txn_disk_addr, txn_disk_size, POSIX_FADV_DONTNEED);
  for (const auto& p : index_pages) {
    posix_fadvise(fd_, p.addr, p.size, POSIX_FADV_DONTNEED);
  }
#endif

  /* drop the flushed pages from memory */
  for (const auto& p : flushed_pages) {
    page_map_.flushPage(p.page_id, p.version, p.disk_addr, p.disk_size);
  }

  return true;
}

} // namespace tsdb

