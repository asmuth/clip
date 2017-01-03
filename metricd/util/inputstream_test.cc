/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <stdlib.h>
#include <stdio.h>
#include "metricd/util/exception.h"
#include "metricd/util/io/inputstream.h"
#include "metricd/util/test/unittest.h"

UNIT_TEST(FileInputStreamTest);

//TEST_CASE(FileInputStreamTest, TestOpenFile, [] () {
//  auto file = FileInputStream::openFile(
//      "test/fixtures/gbp_per_country.csv");
//
//  EXPECT(file.get() != nullptr);
//});
//
//TEST_CASE(FileInputStreamTest, TestInvalidFileName, [] () {
//  auto errmsg = "error opening file 'test/fixtures/invalid.csv': "
//      "No such file or directory";
//
//  EXPECT_EXCEPTION(errmsg, [] () {
//    auto file = FileInputStream::openFile("test/fixtures/invalid.csv");
//  });
//});
//
//TEST_CASE(FileInputStreamTest, TestReadUTF8ByteOrderMark, [] () {
//  auto file = FileInputStream::openFile(
//      "test/fixtures/gbp_per_country.csv");
//
//  EXPECT(file.get() != nullptr);
//  EXPECT(file->readByteOrderMark() == FileInputStream::BOM_UTF8);
//});
