/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnordmetric/io/fileutil.h>
#include <fnordmetric/metricdb/metricrepository.h>
#include <fnordmetric/util/unittest.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

using namespace fnordmetric::metricdb;
using namespace fnord::io;

UNIT_TEST(MetricRepositoryTest);

const char kTestRepoPath[] = "/tmp/__fnordmetric_test_metricrepo";

TEST_CASE(MetricRepositoryTest, TestCreateNewMetric, [] () {
  //io::FileUtil::rm_rf(kTestRepoPath);
  io::FileUtil::mkdir_p(kTestRepoPath);

  MetricRepository repo(std::shared_ptr<FileRepository>(
      new FileRepository(kTestRepoPath)));

  EXPECT(repo.findMetric("http_status_codes") == nullptr);
  auto metric = repo.findOrCreateMetric("http_status_codes");
});



