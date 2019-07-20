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
#include "unittest.h"

using namespace fviz;

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
