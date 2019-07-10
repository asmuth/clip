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
#include <sexpr_parser.h>

using namespace plotfx;

#define EXPECT(X) \
    if (!(X)) { \
      std::cerr << "ERROR: expectation failed: " << #X << " on line " << __LINE__ <<  std::endl; \
      std::exit(1); \
    }

#define EXPECT_EQ(A, B) EXPECT((A) == (B))

#define EXPECT_STREQ(A, B) EXPECT(std::string(A) == std::string(B))

#define EXPECT_OK(X) \
    do { \
      auto rc = (X); \
      if (!rc) { \
        std::cerr << "ERROR: " << rc.getMessage() << " on line " << __LINE__ <<  std::endl; \
        std::exit(1); \
      } \
    } while(0)

void test_parse_literals() {
  std::string confstr =
      R"(1337 hello world)";

  Expr e;
  EXPECT_OK(expr_parse(confstr.data(), confstr.size(), &e));

  EXPECT_EQ(e.kind, ExprKind::LIST);
  EXPECT_EQ(e.size(), 3);
  EXPECT_EQ(e[0].kind, ExprKind::VALUE_LITERAL);
  EXPECT_STREQ(e[0], "1337");
  EXPECT_EQ(e[1].kind, ExprKind::VALUE_LITERAL);
  EXPECT_STREQ(e[1], "hello");
  EXPECT_EQ(e[2].kind, ExprKind::VALUE_LITERAL);
  EXPECT_STREQ(e[2], "world");
}

void test_parse_lists() {
  std::string confstr =
      R"((1337 hello (world galaxy) xxx) yyy)";

  Expr e;
  EXPECT_OK(expr_parse(confstr.data(), confstr.size(), &e));

  EXPECT_EQ(e.kind, ExprKind::LIST);
  EXPECT_EQ(e.size(), 2);
  EXPECT_EQ(e[0].kind, ExprKind::LIST);
  EXPECT_EQ(e[0].size(), 4);
  EXPECT_EQ(e[0][0].kind, ExprKind::VALUE_LITERAL);
  EXPECT_STREQ(e[0][0], "1337");
  EXPECT_EQ(e[0][1].kind, ExprKind::VALUE_LITERAL);
  EXPECT_STREQ(e[0][1], "hello");
  EXPECT_EQ(e[0][2].kind, ExprKind::LIST);
  EXPECT_EQ(e[0][2].size(), 2);
  EXPECT_EQ(e[0][2][0].kind, ExprKind::VALUE_LITERAL);
  EXPECT_STREQ(e[0][2][0], "world");
  EXPECT_EQ(e[0][2][1].kind, ExprKind::VALUE_LITERAL);
  EXPECT_STREQ(e[0][2][1], "galaxy");
  EXPECT_EQ(e[0][3].kind, ExprKind::VALUE_LITERAL);
  EXPECT_STREQ(e[0][3], "xxx");
  EXPECT_EQ(e[1].kind, ExprKind::VALUE_LITERAL);
  EXPECT_STREQ(e[1], "yyy");
}

void test_parse_strings() {
  std::string confstr =
      R"(1337 "hello" 'world')";

  Expr e;
  EXPECT_OK(expr_parse(confstr.data(), confstr.size(), &e));

  EXPECT_EQ(e.kind, ExprKind::LIST);
  EXPECT_EQ(e.size(), 3);
  EXPECT_EQ(e[0].kind, ExprKind::VALUE_LITERAL);
  EXPECT_STREQ(e[0], "1337");
  EXPECT_EQ(e[1].kind, ExprKind::VALUE);
  EXPECT_STREQ(e[1], "hello");
  EXPECT_EQ(e[2].kind, ExprKind::VALUE);
  EXPECT_STREQ(e[2], "world");
}

void test_parse_string_escapes() {
  std::string confstr =
      R"("escape \" me" "hello \\ world")";

  Expr e;
  EXPECT_OK(expr_parse(confstr.data(), confstr.size(), &e));

  EXPECT_EQ(e.kind, ExprKind::LIST);
  EXPECT_EQ(e.size(), 2);
  EXPECT_EQ(e[0].kind, ExprKind::VALUE);
  EXPECT_STREQ(e[0], "escape \" me");
  EXPECT_EQ(e[1].kind, ExprKind::VALUE);
  EXPECT_STREQ(e[1], "hello \\ world");
}

int main() {
  test_parse_literals();
  test_parse_lists();
  test_parse_strings();
  test_parse_string_escapes();
  return EXIT_SUCCESS;
}
