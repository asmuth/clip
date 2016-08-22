/**
 * Copyright (c) 2016 zScale Technology GmbH <legal@zscale.io>
 * Authors:
 *   - Paul Asmuth <paul@zscale.io>
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Affero General Public License ("the license") as
 * published by the Free Software Foundation, either version 3 of the License,
 * or any later version.
 *
 * In accordance with Section 7(e) of the license, the licensing of the Program
 * under the license does not imply a trademark license. Therefore any rights,
 * title and interest in our trademarks remain entirely with us.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the license for more details.
 *
 * You can be released from the requirements of the license by purchasing a
 * commercial license. Buying such a license is mandatory as soon as you develop
 * commercial activities involving this program without disclosing the source
 * code of your own applications
 */
#include <stdlib.h>
#include <stdio.h>
#include "fnordmetric/util/exception.h"
#include "fnordmetric/util/io/inputstream.h"
#include "fnordmetric/util/test/unittest.h"

#include "fnordmetric/eventql.h"

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
