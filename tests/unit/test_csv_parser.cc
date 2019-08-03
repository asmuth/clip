/**
 * This file is part of the "fviz" project
 *   Copyright (c) 2018 Paul Asmuth
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <sexpr_parser.h>
#include "fileutil.h"
#include "unittest.h"
#include "csv.h"

using namespace fviz;

void test_parse_unquoted() {
  std::string testfile;
  EXPECT_OK(read_file(test_data_path("bardata.csv"), &testfile));

  CSVData data;
  EXPECT_OK(csv_parse(testfile, &data));

  EXPECT_EQ(data.size(), 7);
  EXPECT_EQ(data[0].size(), 7);
  EXPECT_EQ(data[1].size(), 7);
  EXPECT_EQ(data[2].size(), 7);
  EXPECT_EQ(data[3].size(), 7);
  EXPECT_EQ(data[4].size(), 7);
  EXPECT_EQ(data[5].size(), 7);
  EXPECT_EQ(data[6].size(), 7);
  EXPECT_EQ(data[1][0], "1");
  EXPECT_EQ(data[1][1], "34");
  EXPECT_EQ(data[1][2], "43");
  EXPECT_EQ(data[1][3], "A");
  EXPECT_EQ(data[1][4], "5");
  EXPECT_EQ(data[1][5], "54");
  EXPECT_EQ(data[1][6], "A");
  EXPECT_EQ(data[2][0], "2");
  EXPECT_EQ(data[2][1], "38");
  EXPECT_EQ(data[2][2], "65");
  EXPECT_EQ(data[2][3], "B");
  EXPECT_EQ(data[2][4], "12");
  EXPECT_EQ(data[2][5], "55");
  EXPECT_EQ(data[2][6], "B");
  EXPECT_EQ(data[3][0], "3");
  EXPECT_EQ(data[3][1], "43");
  EXPECT_EQ(data[3][2], "46");
  EXPECT_EQ(data[3][3], "C");
  EXPECT_EQ(data[3][4], "-0");
  EXPECT_EQ(data[3][5], "63");
  EXPECT_EQ(data[3][6], "C");
  EXPECT_EQ(data[4][0], "4");
  EXPECT_EQ(data[4][1], "33");
  EXPECT_EQ(data[4][2], "38");
  EXPECT_EQ(data[4][3], "D");
  EXPECT_EQ(data[4][4], "17");
  EXPECT_EQ(data[4][5], "53");
  EXPECT_EQ(data[4][6], "D");
  EXPECT_EQ(data[5][0], "5");
  EXPECT_EQ(data[5][1], "21");
  EXPECT_EQ(data[5][2], "38");
  EXPECT_EQ(data[5][3], "E");
  EXPECT_EQ(data[5][4], "18");
  EXPECT_EQ(data[5][5], "41");
  EXPECT_EQ(data[5][6], "E");
  EXPECT_EQ(data[6][0], "6");
  EXPECT_EQ(data[6][1], "33");
  EXPECT_EQ(data[6][2], "55");
  EXPECT_EQ(data[6][3], "F");
  EXPECT_EQ(data[6][4], "0");
  EXPECT_EQ(data[6][5], "53");
  EXPECT_EQ(data[6][6], "F");
}

void test_parse_quoted() {
  std::string testfile;
  EXPECT_OK(read_file(test_data_path("bardata_quoted.csv"), &testfile));

  CSVData data;
  EXPECT_OK(csv_parse(testfile, &data));

  EXPECT_EQ(data.size(), 7);
  EXPECT_EQ(data[0].size(), 7);
  EXPECT_EQ(data[1].size(), 7);
  EXPECT_EQ(data[2].size(), 7);
  EXPECT_EQ(data[3].size(), 7);
  EXPECT_EQ(data[4].size(), 7);
  EXPECT_EQ(data[5].size(), 7);
  EXPECT_EQ(data[6].size(), 7);
  EXPECT_EQ(data[1][0], "1");
  EXPECT_EQ(data[1][1], "34");
  EXPECT_EQ(data[1][2], "43");
  EXPECT_EQ(data[1][3], "A");
  EXPECT_EQ(data[1][4], "5");
  EXPECT_EQ(data[1][5], "54");
  EXPECT_EQ(data[1][6], "A");
  EXPECT_EQ(data[2][0], "2");
  EXPECT_EQ(data[2][1], "38");
  EXPECT_EQ(data[2][2], "65");
  EXPECT_EQ(data[2][3], "B as in 'beans'");
  EXPECT_EQ(data[2][4], "12");
  EXPECT_EQ(data[2][5], "55");
  EXPECT_EQ(data[2][6], "B as in 'beans'");
  EXPECT_EQ(data[3][0], "3");
  EXPECT_EQ(data[3][1], "43");
  EXPECT_EQ(data[3][2], "46");
  EXPECT_EQ(data[3][3], "C as in \"coffee\"");
  EXPECT_EQ(data[3][4], "-0");
  EXPECT_EQ(data[3][5], "63");
  EXPECT_EQ(data[3][6], "C as in \"coffee\"");
  EXPECT_EQ(data[4][0], "4");
  EXPECT_EQ(data[4][1], "33");
  EXPECT_EQ(data[4][2], "38");
  EXPECT_EQ(data[4][3], "D");
  EXPECT_EQ(data[4][4], "17");
  EXPECT_EQ(data[4][5], "53");
  EXPECT_EQ(data[4][6], "D");
  EXPECT_EQ(data[5][0], "5");
  EXPECT_EQ(data[5][1], "21");
  EXPECT_EQ(data[5][2], "38");
  EXPECT_EQ(data[5][3], "E");
  EXPECT_EQ(data[5][4], "18");
  EXPECT_EQ(data[5][5], "41");
  EXPECT_EQ(data[5][6], "E");
  EXPECT_EQ(data[6][0], "6");
  EXPECT_EQ(data[6][1], "33");
  EXPECT_EQ(data[6][2], "55");
  EXPECT_EQ(data[6][3], "F");
  EXPECT_EQ(data[6][4], "0");
  EXPECT_EQ(data[6][5], "53");
  EXPECT_EQ(data[6][6], "F as in \"Fire!!!\"");
}

int main() {
  test_parse_unquoted();
  test_parse_quoted();
  return EXIT_SUCCESS;
}

