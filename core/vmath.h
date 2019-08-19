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
struct vec2;
struct vec3;


/**
 * A two-dimensional vector of the following form:
 *
 *   | X |
 *   | Y |
 *
 */
struct vec2 {
  vec2();
  vec2(double x, double y);
  explicit vec2(const vec3& v);
  double x;
  double y;
};

/**
 * A three-dimensional vector of the following form:
 *
 *   | X |
 *   | Y |
 *   | Z |
 *
 */
struct vec3 {
  vec3();
  vec3(double x, double y, double z);
  vec3(const vec2& v, double z);
  double x;
  double y;
  double z;
};

/**
 * A 3x3 matrix of the following form:
 *
 *   | A B C |
 *   | D E F |
 *   | G H I |
 *
 */
struct mat3 {
  mat3();
  double a;
  double d;
  double g;
  double b;
  double e;
  double h;
  double c;
  double f;
  double i;
};

/**
 * Add two vectors
 */
vec2 add(const vec2& a, const vec2& b);

/**
 * Subtract two vectors
 */
vec2 sub(const vec2& a, const vec2& b);

/**
 * Multiply a vector with a scalar
 */
vec2 mul(const vec2& v, double s);

/**
 * Multiply a 3x3 matrix with a 3x3 matrix
 */
mat3 mul(const mat3& a, const mat3& b);

/**
 * Multiply a 3x3 matrix with a three-dimensional vector
 */
vec3 mul(const mat3& m, const vec3& v);

/**
 * Compute the dot (scalar) product of two vectors
 */
double dot(const vec2& a, const vec2& b);

/**
 * Produce a vector from an angle in degrees so that zero degress maps to a
 * vector pointing "right" along the x axis (1, 0) and positive degree values
 * rotate the vector clockwise around the origin.
 */
vec2 from_deg(double deg);

/**
 * Return the magnitude of a vector
 */
double magnitude(const vec2& v);

/**
 * Normalize a vector
 */
vec2 normalize(const vec2& v);

/**
 * Find the mean value of a list of vectors
 */
vec2 mean(const vec2* v, size_t v_len);

/**
 * Create a 2D "rotation matrix" that rotates `angle` degrees clockwise around
 * origin
 */
mat3 rotate2(double angle);

/**
 * Create a 2D "scale matrix"
 */
mat3 scale2(const vec2& s);

/**
 * Create a 2D "translation matrix"
 */
mat3 translate2(const vec2& t);

/**
 * Sort an array of vec2 objects in clockwise order
 */
void sort_cw(vec2* v, size_t v_len);

/**
 * Sort an array of vec2 objects in counter-clockwise order
 */
void sort_ccw(vec2* v, size_t v_len);

/**
 * Print a 2D vector
 */
std::ostream& operator <<(std::ostream& os, const vec2& c);

} // namespace fviz
