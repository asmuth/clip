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

  ExprStorage es;
  EXPECT_OK(expr_parse(confstr.data(), confstr.size(), &es));

  const Expr* e = es.get();
  EXPECT(expr_is_value_literal(e, "1337"));
  EXPECT((e = expr_next(e)) != nullptr);
  EXPECT(expr_is_value_literal(e, "hello"));
  EXPECT((e = expr_next(e)) != nullptr);
  EXPECT(expr_is_value_literal(e, "world"));
  EXPECT((e = expr_next(e)) == nullptr);
}

void test_parse_lists() {
  std::string confstr =
      R"(aaa (1337 hello (world galaxy) xxx) yyy)";

  ExprStorage es;
  EXPECT_OK(expr_parse(confstr.data(), confstr.size(), &es));

  const Expr* e = es.get();
  EXPECT(expr_is_value_literal(e, "aaa"));
  EXPECT((e = expr_next(e)) != nullptr);
  EXPECT(expr_is_list(e));
  auto l1 = expr_get_list(e);
  EXPECT(expr_is_value_literal(l1, "1337"));
  EXPECT((l1 = expr_next(l1)) != nullptr);
  EXPECT(expr_is_value_literal(l1, "hello"));
  EXPECT((l1 = expr_next(l1)) != nullptr);
  EXPECT(expr_is_list(l1));
  auto l2 = expr_get_list(l1);
  EXPECT(expr_is_value_literal(l2, "world"));
  EXPECT((l2 = expr_next(l2)) != nullptr);
  EXPECT(expr_is_value_literal(l2, "galaxy"));
  EXPECT((l2 = expr_next(l2)) == nullptr);
  EXPECT((l1 = expr_next(l1)) != nullptr);
  EXPECT(expr_is_value_literal(l1, "xxx"));
  EXPECT((l1 = expr_next(l1)) == nullptr);
  EXPECT((e = expr_next(e)) != nullptr);
  EXPECT(expr_is_value_literal(e, "yyy"));
  EXPECT((e = expr_next(e)) == nullptr);
}

void test_parse_strings() {
  std::string confstr =
      R"(1337 "hello" 'world')";

  ExprStorage es;
  EXPECT_OK(expr_parse(confstr.data(), confstr.size(), &es));

  const Expr* e = es.get();
  EXPECT(expr_is_value_literal(e, "1337"));
  EXPECT((e = expr_next(e)) != nullptr);
  EXPECT(expr_is_value_quoted(e, "hello"));
  EXPECT((e = expr_next(e)) != nullptr);
  EXPECT(expr_is_value_quoted(e, "world"));
  EXPECT((e = expr_next(e)) == nullptr);
}

void test_parse_string_escapes() {
  std::string confstr =
      R"("escape \" me" "hello \\ world")";

  ExprStorage es;
  EXPECT_OK(expr_parse(confstr.data(), confstr.size(), &es));

  const Expr* e = es.get();
  EXPECT(expr_is_value(e, "escape \" me"));
  EXPECT((e = expr_next(e)) != nullptr);
  EXPECT(expr_is_value(e, "hello \\ world"));
  EXPECT((e = expr_next(e)) == nullptr);
}

int main() {
  test_parse_literals();
  test_parse_lists();
  test_parse_strings();
  test_parse_string_escapes();
  return EXIT_SUCCESS;
}
