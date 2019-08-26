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
#include "graphics/brush.h"
#include "graphics/color.h"
#include "graphics/page_description.h"
#include "graphics/text.h"
#include "environment.h"
#include "utils/fileutil.h"
#include "unittest.h"

using namespace clip;

void test_translate() {
  auto m = translate2({1.0, 1.0});
  auto v1 = vec3(5.0, 2.0, 1.0);
  auto v2 = mul(m, v1);
  EXPECT_FEQ(v2.x, 6.0);
  EXPECT_FEQ(v2.y, 3.0);
}

void test_scale() {
  auto m = scale2({3.0, 3.0});
  auto v1 = vec3(5.0, 2.0, 1.0);
  auto v2 = mul(m, v1);
  EXPECT_FEQ(v2.x, 15.0);
  EXPECT_FEQ(v2.y, 6.0);
}

void test_rotate() {
  {
    auto m = rotate2(90);
    auto v1 = vec3(0.0, 2.0, 1.0);
    auto v2 = mul(m, v1);
    EXPECT_FEQ(v2.x, 2.0);
    EXPECT_FEQ(v2.y, 0.0);
  }

  {
    auto m = rotate2(180);
    auto v1 = vec3(0.0, 2.0, 1.0);
    auto v2 = mul(m, v1);
    EXPECT_FEQ(v2.x, 0.0);
    EXPECT_FEQ(v2.y, -2.0);
  }

  {
    auto m = rotate2(270);
    auto v1 = vec3(0.0, 2.0, 1.0);
    auto v2 = mul(m, v1);
    EXPECT_FEQ(v2.x, -2.0);
    EXPECT_FEQ(v2.y, 0.0);
  }

  {
    auto m = rotate2(360);
    auto v1 = vec3(0.0, 2.0, 1.0);
    auto v2 = mul(m, v1);
    EXPECT_FEQ(v2.x, 0.0);
    EXPECT_FEQ(v2.y, 2.0);
  }
}

int main(int argc, char** argv) {
  test_translate();
  test_scale();
  test_rotate();
}

