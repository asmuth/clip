/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fnordmetric/io/file.h>
#include <fnordmetric/util/unittest.h>
#include <fnordmetric/sstable/livesstable.h>

using namespace fnord::sstable;
using namespace fnord::io;
UNIT_TEST(SSTableTest);

const int kTestFileOpenFlags =
    File::O_READ | File::O_WRITE | File::O_CREATEOROPEN | File::O_TRUNCATE |
        File::O_AUTODELETE;

TEST_CASE(SSTableTest, TestLiveSSTableAppend, [] () {
  auto file = File::openFile(
      "/tmp/__fnord__sstabletest1.sstable",
      kTestFileOpenFlags);

  DefaultIndexProvider index_provider;
  auto tbl = LiveSSTable::create(
      std::move(file),
      index_provider,
      NULL,
      0);

/*
  tbl.append("key1", "value1");
  tbl.append("key2", "value2");
  tbl.append("key3", "value3");
  tbl.finalize();
*/
});


