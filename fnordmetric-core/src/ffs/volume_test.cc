/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <metricd/ffs/cowblob.h>
#include <metricd/ffs/volume.h>
#include <metricd/util/unittest.h>

using namespace fnordmetric::ffs;

UNIT_TEST(VolumeTest);

TEST_CASE(VolumeTest, TestCowBlob, [] () {
  auto volume = Volume::openFile(
      "/tmp/__ffs_test_volume",
      Volume::FILE_TRUNCATE || Volume::FILE_AUTODELETE);

  auto blob = volume->insert<COWBlob>("myblob");
});

