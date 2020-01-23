/**
 * This file is part of the "clip" project
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
#pragma once

#include <functional>
#include <unordered_map>
#include <vector>
#include <string>
#include <string.h>

#include "return_code.h"

const double EPSILON = 0.0001;

#define EXPECT(X) \
    if (!(X)) { \
      std::cerr << "ERROR: expectation failed: " << #X << " on line " << __LINE__ <<  std::endl; \
      std::exit(1); \
    }

#define EXPECT_EQ(A, B) EXPECT((A) == (B))

#define EXPECT_FEQ(A, B) EXPECT(std::abs((A) - (B)) < EPSILON)

#define EXPECT_STREQ(A, B) EXPECT(std::string(A) == std::string(B))

#define EXPECT_OK(X) \
    do { \
      auto rc = (X); \
      if (!rc) { \
        std::cerr << "ERROR: " << rc.message << " on line " << __LINE__ <<  std::endl; \
        std::exit(1); \
      } \
    } while(0)

inline std::string test_data_path(std::string p) {
  p = "tests/testdata/" + p;

  if (auto srcdir = getenv("FVIZ_TEST_SRCDIR"); srcdir) {
    return std::string(srcdir) + "/" + p;
  } else {
    return p;
  }
}
