/**
 * This file is part of the "plotfx" project
 *   Copyright (c) 2018 Paul Asmuth
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 * 
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * 
 * * Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <plist/plist_parser.h>

using namespace plist;

#define EXPECT(X) \
    if (!(X)) { \
      std::cerr << "ERROR: expectation failed: " << #X << " on line " << __LINE__ <<  std::endl; \
      std::exit(1); \
    }

#define EXPECT_EQ(A, B) EXPECT((A) == (B))

#define EXPECT_STREQ(A, B) EXPECT(std::string(A) == std::string(B))

void test_parse_simple() {
  std::string confstr =
      R"(
        prop0: 1337;
        prop1: "hello";
        prop2: 'world';
      )";

  PropertyListParser parser(confstr.c_str(), confstr.size());
  PropertyList plist;
  if (!parser.parse(&plist)) {
    std::cerr << parser.get_error() << std::endl;
    std::exit(1);
  }

  EXPECT_EQ(plist[0].kind, PropertyKind::VALUE_LITERAL);
  EXPECT_STREQ(plist[0].name, "prop0");
  EXPECT_STREQ(plist[0], "1337");
  EXPECT_EQ(plist[1].kind, PropertyKind::VALUE);
  EXPECT_STREQ(plist[1].name, "prop1");
  EXPECT_STREQ(plist[1], "hello");
  EXPECT_EQ(plist[2].kind, PropertyKind::VALUE);
  EXPECT_STREQ(plist[2].name, "prop2");
  EXPECT_STREQ(plist[2], "world");
}

void test_parse_tuple() {
  std::string confstr =
      R"(
        prop0: 1337 1338 1339;
      )";

  PropertyListParser parser(confstr.c_str(), confstr.size());
  PropertyList plist;
  if (!parser.parse(&plist)) {
    std::cerr << parser.get_error() << std::endl;
    std::exit(1);
  }

  EXPECT_EQ(plist.size(), 1);
  EXPECT_STREQ(plist[0].name, "prop0");
  EXPECT_EQ(plist[0].kind, PropertyKind::TUPLE);
  EXPECT_EQ(plist[0].size(), 3);
  EXPECT_STREQ(plist[0][0], "1337");
  EXPECT_STREQ(plist[0][1], "1338");
  EXPECT_STREQ(plist[0][2], "1339");
}

void test_parse_list() {
  std::string confstr =
      R"(
        prop0: 1337, 1338, 1339;
      )";

  PropertyListParser parser(confstr.c_str(), confstr.size());
  PropertyList plist;
  if (!parser.parse(&plist)) {
    std::cerr << parser.get_error() << std::endl;
    std::exit(1);
  }

  EXPECT_EQ(plist.size(), 1);
  EXPECT_STREQ(plist[0].name, "prop0");
  EXPECT_EQ(plist[0].kind, PropertyKind::LIST);
  EXPECT_EQ(plist[0].size(), 3);
  EXPECT_STREQ(plist[0][0], "1337");
  EXPECT_STREQ(plist[0][1], "1338");
  EXPECT_STREQ(plist[0][2], "1339");
}

void test_parse_tuple_list() {
  std::string confstr =
      R"(
        prop0: a 1, b 2, c 3;
      )";

  PropertyListParser parser(confstr.c_str(), confstr.size());
  PropertyList plist;
  if (!parser.parse(&plist)) {
    std::cerr << parser.get_error() << std::endl;
    std::exit(1);
  }

  EXPECT_EQ(plist.size(), 1);
  EXPECT_STREQ(plist[0].name, "prop0");
  EXPECT_EQ(plist[0].kind, PropertyKind::LIST);
  EXPECT_EQ(plist[0].size(), 3);
  EXPECT_EQ(plist[0][0].kind, PropertyKind::TUPLE);
  EXPECT_EQ(plist[0][0].size(), 2);
  EXPECT_STREQ(plist[0][0][0], "a");
  EXPECT_STREQ(plist[0][0][1], "1");
  EXPECT_EQ(plist[0][1].kind, PropertyKind::TUPLE);
  EXPECT_EQ(plist[0][1].size(), 2);
  EXPECT_STREQ(plist[0][1][0], "b");
  EXPECT_STREQ(plist[0][1][1], "2");
  EXPECT_EQ(plist[0][2].kind, PropertyKind::TUPLE);
  EXPECT_EQ(plist[0][2].size(), 2);
  EXPECT_STREQ(plist[0][2][0], "c");
  EXPECT_STREQ(plist[0][2][1], "3");
}

void test_parse_element() {
  std::string confstr =
      R"(
        blah: xxx;

        elem {
          prop0: 1337;
          prop1: "hello";
          prop2: 'world';
        }
      )";

  PropertyListParser parser(confstr.c_str(), confstr.size());
  PropertyList plist;
  if (!parser.parse(&plist)) {
    std::cerr << parser.get_error() << std::endl;
    std::exit(1);
  }

  EXPECT_EQ(plist.size(), 2);
  EXPECT_STREQ(plist[0].name, "blah");
  EXPECT_STREQ(plist[0], "xxx");

  EXPECT_STREQ(plist[1].name, "elem");
  const auto& elem = plist[1];
  EXPECT_EQ(elem.kind, PropertyKind::MAP);
  EXPECT_EQ(elem.size(), 3);
  EXPECT_STREQ(elem[0].name, "prop0");
  EXPECT_STREQ(elem[0], "1337");
  EXPECT_STREQ(elem[1].name, "prop1");
  EXPECT_STREQ(elem[1], "hello");
  EXPECT_STREQ(elem[2].name, "prop2");
  EXPECT_STREQ(elem[2], "world");
}

void test_parse_nested() {
  std::string confstr =
      R"(
        outer  {
          inner {
            prop: 1337;
          }
        }
      )";

  PropertyListParser parser(confstr.c_str(), confstr.size());
  PropertyList plist;
  if (!parser.parse(&plist)) {
    std::cerr << parser.get_error() << std::endl;
    std::exit(1);
  }

  EXPECT_EQ(plist.size(), 1);
  const auto& outer = plist[0];

  EXPECT_STREQ(outer.name, "outer");
  EXPECT_EQ(outer.size(), 1);

  const auto& inner = outer[0];

  EXPECT_STREQ(inner.name, "inner");
  EXPECT_EQ(inner.size(), 1);
  EXPECT_STREQ(inner[0].name, "prop");
  EXPECT_STREQ(inner[0], "1337");
}

void test_parse_whitespace() {
  std::string confstr =
      R"(
        prop0: 123,456;
        prop1: 123, 456;
        prop2: 123,  456;
        prop3: 123  , 456;
      )";

  PropertyListParser parser(confstr.c_str(), confstr.size());
  PropertyList plist;
  if (!parser.parse(&plist)) {
    std::cerr << parser.get_error() << std::endl;
    std::exit(1);
  }

  EXPECT_EQ(plist.size(), 4);

  EXPECT_EQ(plist[0].size(), 2);
  EXPECT_STREQ(plist[0][0], "123");
  EXPECT_STREQ(plist[0][1], "456");
  EXPECT_STREQ(plist[1][0], "123");
  EXPECT_STREQ(plist[1][1], "456");
  EXPECT_STREQ(plist[2][0], "123");
  EXPECT_STREQ(plist[2][1], "456");
  EXPECT_STREQ(plist[3][0], "123");
  EXPECT_STREQ(plist[3][1], "456");
}

void test_parse_enum() {
  std::string confstr =
      R"(
        test: blah("1337");
      )";

  PropertyListParser parser(confstr.c_str(), confstr.size());
  PropertyList plist;
  if (!parser.parse(&plist)) {
    std::cerr << parser.get_error() << std::endl;
    std::exit(1);
  }

  EXPECT_EQ(plist.size(), 1);
  EXPECT_STREQ(plist[0].name, "test");
  EXPECT_EQ(plist[0].kind, PropertyKind::ENUM);
  EXPECT_STREQ(plist[0].value, "blah");
  EXPECT_EQ(plist[0].size(), 1);
  EXPECT_EQ(plist[0][0].kind, PropertyKind::VALUE);
  EXPECT_STREQ(plist[0][0], "1337");
}

int main() {
  test_parse_simple();
  test_parse_tuple();
  test_parse_tuple_list();
  test_parse_list();
  test_parse_element();
  test_parse_nested();
  test_parse_whitespace();
  test_parse_enum();
  return EXIT_SUCCESS;
}
