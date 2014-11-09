/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnordmetric/metricdb/backends/disk/binaryformat.h>
#include <fnordmetric/metricdb/backends/disk/samplewriter.h>
#include <fnordmetric/metricdb/backends/disk/tokenindex.h>
#include <fnordmetric/util/ieee754.h>
#include <fnordmetric/util/runtimeexception.h>
#include <stdlib.h>

namespace fnordmetric {
namespace metricdb {
namespace disk_backend {

SampleWriter::SampleWriter(
    TokenIndex* token_index) :
    token_index_(token_index) {}

template <> void SampleWriter::writeValue<uint64_t>(uint64_t value) {
  appendUInt64(value);
}

template <> void SampleWriter::writeValue<double>(double value) {
  appendUInt64(fnord::util::IEEE754::toBytes(value));
}

void SampleWriter::writeLabel(
    const std::string& key,
    const std::string& value) {
  writeToken(key, true);
  writeToken(value, false);
}

void SampleWriter::writeToken(const std::string& token, bool force_indexing) {
  uint32_t token_id = token_index_->findToken(token);

  if (token.size() >= TokenIndex::kMinTokenID) {
    RAISE(kIllegalArgumentError, "token too large");
  }

  if (token_id > 0) {
    // write token reference
    appendUInt32(token_id);
  } else if (force_indexing) {
    // write new definition
    token_id = token_index_->addToken(token);
    appendUInt32(0xffffffff);
    appendUInt32(token_id);
    appendUInt32(token.size());
    appendString(token);
  } else {
    // write anonymous token
    appendUInt32(token.size());
    appendString(token);
  }
}


}
}
}

