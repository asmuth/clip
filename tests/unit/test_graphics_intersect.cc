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
#include "graphics/intersect.h"
#include "unittest.h"

using namespace fviz;

void test_intersect_parallel() {
  // some slope but offset in y -> parallel
  EXPECT_EQ(
      intersect_line_line(
          {0, 0},
          vec2_from_deg(45),
          {0, 1},
          vec2_from_deg(45),
          nullptr),
      false);

  // some slope but offset in x -> parallel
  EXPECT_EQ(
      intersect_line_line(
          {0, 1},
          vec2_from_deg(45),
          {0, 0},
          vec2_from_deg(45),
          nullptr),
      false);

  // coincident -> parallel
  EXPECT_EQ(
      intersect_line_line(
          {0, 0},
          vec2_from_deg(45),
          {0, 0},
          vec2_from_deg(45),
          nullptr),
      false);

  // 180 degree shifted angles -> parallel
  EXPECT_EQ(
      intersect_line_line(
          {0, 0},
          vec2_from_deg(45),
          {0, 0},
          vec2_from_deg(45 + 180),
          nullptr),
     false);

  // -180 degree shifted angles -> parallel
  EXPECT_EQ(
      intersect_line_line(
          {0, 0},
          vec2_from_deg(45),
          {0, 0},
          vec2_from_deg(45 - 180),
          nullptr),
      false);

  // 360 degree shifted angles -> parallel
  EXPECT_EQ(
      intersect_line_line(
          {0, 0},
          vec2_from_deg(45),
          {0, 0},
          vec2_from_deg(45 + 360),
          nullptr),
      false);

  // -360 degree shifted angles -> parallel
  EXPECT_EQ(
      intersect_line_line(
          {0, 0},
          vec2_from_deg(45),
          {0, 0},
          vec2_from_deg(45 + -360),
          nullptr),
      false);

  // angles very close to each other -> "parallel"
  EXPECT_EQ(
      intersect_line_line(
          {0, 0},
          vec2_from_deg(45),
          {0, 0},
          vec2_from_deg(45.0000001),
          nullptr),
      false);

  EXPECT_EQ(
      intersect_line_line(
          {0, 0},
          vec2_from_deg(45),
          {0, 0},
          vec2_from_deg(44.9999999),
          nullptr),
      false);

  // angles less close to each other -> not parallel
  EXPECT_EQ(
      intersect_line_line(
          {0, 0},
          vec2_from_deg(45),
          {0, 0},
          vec2_from_deg(45.01),
          nullptr),
      true);

  EXPECT_EQ(
      intersect_line_line(
          {0, 0},
          vec2_from_deg(45),
          {0, 0},
          vec2_from_deg(44.99),
          nullptr),
      true);

  // angles different and x offset -> not parallel
  EXPECT_EQ(
      intersect_line_line(
          {0, 0},
          vec2_from_deg(45),
          {1, 0},
          vec2_from_deg(45.1),
          nullptr),
      true);

  // angles different and x offset -> not parallel
  EXPECT_EQ(
      intersect_line_line(
          {0, 0},
          vec2_from_deg(45),
          {0, 1},
          vec2_from_deg(45.1),
          nullptr),
      true);
}

void test_intersect_point() {
  Point p;

  EXPECT_EQ(
      intersect_line_line(
          {0, 0},
          {1, 2},
          {0, 2},
          {1, 1},
          &p),
      true);

  EXPECT_EQ(p.x, 2);
  EXPECT_EQ(p.y, 4);
}

int main(int argc, char** argv) {
  test_intersect_parallel();
  test_intersect_point();
}

