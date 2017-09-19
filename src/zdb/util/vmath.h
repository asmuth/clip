/**
 * This file is part of the "libPLG" project
 *   Copyright (c) 2014 Paul Asmuth

 *
 * libplg is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef VMATH_H
#define VMATH_H
#include <ostream>
#include <memory>

#define VMATH_TAU 6.283185307179586
struct mat4;
struct vec3;

/**
 * 2D Vector
 */
struct __attribute__((packed)) vec2 {
  vec2() : d{0.0f, 0.0f} {}
  vec2(float x, float y) : d{x, y} {}
  float d[2];
};

std::ostream& operator<<(std::ostream& os, const vec2& v);
bool operator==(const vec2& a, const vec2& b);

/**
 * 4D Vector
 */
struct __attribute__((packed)) vec4 {
  vec4();
  vec4(float x, float y, float z, float w);
  vec4(const vec3& v3, float w);
  float d[4];
};

std::ostream& operator<<(std::ostream& os, const vec4& v);

/**
 * 3D Vector
 */
struct __attribute__((packed)) vec3 {
  vec3() : d{0.0f, 0.0f, 0.0f} {}
  vec3(float x, float y, float z) : d{x, y, z} {}
  explicit vec3(const vec4& v4);
  float d[3];
};

std::ostream& operator<<(std::ostream& os, const vec3& v);

/**
 * 3x3 Matrix (column major)
 */
struct __attribute__((packed)) mat3 {
  //mat3(nullptr_t _) {}
  mat3() {}
  explicit mat3(const mat4& m4);
  vec3 d[3];
};

/**
 * 4x4 Matrix (column major)
 */
struct __attribute__((packed)) mat4 {
  vec4 d[4];
};

/**
 * Create a 3x3 Identity Matrix (column major)
 */
void mat3_mkidentity(mat4* self);

/**
 * Create a 4x4 Identity Matrix (column major)
 */
void mat4_mkidentity(mat4* self);

/**
 * Create a 4x4 Translation Matrix (column major) that will translate a
 * vertex by a three dimensional vector (x, y, z)
 *
 * @param vec the three dimensional translation vector (x, y, z)
 */
void mat4_mktranslate(mat4* self, const vec3* vec);

/**
 * Create a 4x4 Translation Matrix (column major) that will scale a
 * vertex by a three dimensional vector (x, y, z)
 *
 * @param vec the three dimensional translation vector (x, y, z)
 */
void mat4_mkscale(mat4* self, const vec3* vec);

void mat4_mkrotate(mat4* self, const vec3& rot);

/**
 * Create a 4x4 Rotation Matrix (column major) that will translate a vertex
 * by alpha degrees around the x axis
 *
 * @param alpha the angle of rotation in radians (0-2pi)
 */
void mat4_mkrotate_x(mat4* self, float alpha);

/**
 * Create a 4x4 Rotation Matrix (column major) that will translate a vertex
 * by alpha degrees around the y axis
 *
 * @param alpha the angle of rotation in radians (0-2pi)
 */
void mat4_mkrotate_y(mat4* self, float alpha);

/**
 * Create a 4x4 Rotation Matrix (column major) that will translate a vertex
 * by alpha degrees around the z axis
 *
 * @param alpha the angle of rotation in radians (0-2pi)
 */
void mat4_mkrotate_z(mat4* self, float alpha);

/**
 * Multiply two 4x4 (column major) matrices (matrix product)
 */
void mat4_mul(mat4* dst, const mat4* left, const mat4* right);

mat3 mat4_tomat3(const mat4& mat4);

/**
 * Multiply a three dimensional vector with a scalar
 */
vec3 vec3_mul(const vec3& left, float right);

/**
 * Subtract two three-dimensional vectors
 */
void vec3_sub(vec3* dst, const vec3* left, const vec3* right);

/**
 * Normalize a three-dimensional vector
 */
void vec3_normalize(vec3* self);

/**
 * Returns the magnitude of a three-dimensional vector
 */
float vec3_magnitude(const vec3* self);

/**
 * Takes the cross product of two three-dimensional vectors
 */
void vec3_cross(vec3* dst, const vec3* left, const vec3* right);


/**
 * Add two vectors
 */
vec2 vadd(const vec2& left, const vec2& right);
vec3 vadd(const vec3& left, const vec3& right);

/**
 * Subtract two vectors
 */
vec2 vsub(const vec2& left, const vec2& right);
vec3 vsub(const vec3& left, const vec3& right);

/**
 * Multiply a three-dimensional vector with a scalar
 */
vec2 vsmul(const vec2& vec, float scalar);
vec3 vsmul(const vec3& vec, float scalar);

/**
 * Return a component/entry-wise product of the vector and a scalar
 */
//vec3 

/**
 * Calculate a matrix vector product from a 3x3 matrix and a 3-dimensional vector
 *
 * `mat` is assumed to be in column major format
 */
vec3 mvprod(const mat3& mat, const vec3& vec);

vec4 mvprod(const mat4& mat, const vec4& vec);

float vdot(const vec3& left, const vec3& right);
vec3 vcross(const vec3& left, const vec3& right);
double vmag(const vec3& vec);
double vmag(const vec2& vec);

vec2 vnormalize(const vec2& vec);

double fclamp(double val, double low, double high);

#endif
