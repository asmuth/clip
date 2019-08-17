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
#pragma once
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <iostream>

namespace fviz {

struct vec2 {
  vec2();
  vec2(double x, double y);
  double x;
  double y;
};

/**
 * Return the magnitude of a vector
 */
double vec2_magnitude(const vec2& v);

/**
 * Normalize a vector
 */
vec2 vec2_normalize(const vec2& v);

/**
 * Add two vectors
 */
vec2 vec2_add(const vec2& a, const vec2& b);

/**
 * Subtract two vectors
 */
vec2 vec2_sub(const vec2& a, const vec2& b);

/**
 * Multiply a vector with a scalar
 */
vec2 vec2_mul(const vec2& v, double s);

/**
 * Compute the dot (scalar) product of two vectors
 */
double vec2_dot(const vec2& a, const vec2& b);

/**
 * Produce a vector from an angle in degrees so that zero degress maps to a
 * vector pointing "right" along the x axis (1, 0) and positive degree values
 * rotate the vector clockwise around the origin.
 */
vec2 vec2_from_deg(double deg);

/**
 * Find the mean value of a list of vectors
 */
vec2 vec2_mean(const vec2* v, size_t v_len);

/**
 * Sort an array of vec2 objects in clockwise order
 */
void vec2_sort_cw(vec2* v, size_t v_len);

/**
 * Sort an array of vec2 objects in counter-clockwise order
 */
void vec2_sort_ccw(vec2* v, size_t v_len);

std::ostream& operator <<(std::ostream& os, const vec2& c);

} // namespace fviz

