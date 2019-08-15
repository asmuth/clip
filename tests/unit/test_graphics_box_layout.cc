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
#include "graphics/brush.h"
#include "graphics/color.h"
#include "graphics/page_description.h"
#include "graphics/text.h"
#include "environment.h"
#include "utils/fileutil.h"
#include "unittest.h"

using namespace fviz;

void test_box_rotate_bounds() {
  {
    Rectangle ri(0, 0, 1, 3);
    Rectangle ro = box_rotate_bounds(ri, 90);
    EXPECT_FEQ(ro.w, 3);
    EXPECT_FEQ(ro.h, 1);
    EXPECT_FEQ(ro.x, -1);
    EXPECT_FEQ(ro.y, 1);
  }

  {
    Rectangle ri(0, 0, 1, 3);
    Rectangle ro = box_rotate_bounds(ri, 180);
    EXPECT_FEQ(ro.w, 1);
    EXPECT_FEQ(ro.h, 3);
    EXPECT_FEQ(ro.x, 0);
    EXPECT_FEQ(ro.y, 0);
  }

  {
    Rectangle ri(10, 20, 1, 3);
    Rectangle ro = box_rotate_bounds(ri, 90);
    EXPECT_FEQ(ro.w, 3);
    EXPECT_FEQ(ro.h, 1);
    EXPECT_FEQ(ro.x, 9);
    EXPECT_FEQ(ro.y, 21);
  }
}

int main(int argc, char** argv) {
  test_box_rotate_bounds();
}

