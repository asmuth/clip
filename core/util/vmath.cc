/**
 * This file is part of the "libPLG" project
 *   Copyright (c) 2014 Paul Asmuth

 *
 * libplg is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <string.h>
#include <math.h>
#include <numeric>
#include "vmath.h"

std::ostream& operator<<(std::ostream& os, const vec2& v) {
  os << "<";
  os << v.d[0];
  os << ", ";
  os << v.d[1];
  os << ">";
  return os;
}

bool operator==(const vec2& a, const vec2& b) {
  return
      a.d[0] == b.d[0] &&
      a.d[1] == b.d[1];
}

vec4::vec4() : d{0.0f, 0.0f, 0.0f, 0.0f} {}
vec4::vec4(float x, float y, float z, float w) : d{x, y, z, w} {}
vec4::vec4(const vec3& v3, float w) : d{v3.d[0], v3.d[1], v3.d[2], w} {}

vec3::vec3(const vec4& v4) : vec3(v4.d[0], v4.d[1], v4.d[2]) {}

std::ostream& operator<<(std::ostream& os, const vec3& v) {
  os << "<";
  os << v.d[0];
  os << ", ";
  os << v.d[1];
  os << ", ";
  os << v.d[2];
  os << ">";
  return os;
}

std::ostream& operator<<(std::ostream& os, const vec4& v) {
  os << "<";
  os << v.d[0];
  os << ", ";
  os << v.d[1];
  os << ", ";
  os << v.d[2];
  os << ", ";
  os << v.d[3];
  os << ">";
  return os;
}

mat3::mat3(const mat4& m4) {
  d[0].d[0] = m4.d[0].d[0];
  d[0].d[1] = m4.d[0].d[1];
  d[0].d[2] = m4.d[0].d[2];
  d[1].d[0] = m4.d[1].d[0];
  d[1].d[1] = m4.d[1].d[1];
  d[1].d[2] = m4.d[1].d[2];
  d[2].d[0] = m4.d[2].d[0];
  d[2].d[1] = m4.d[2].d[1];
  d[2].d[2] = m4.d[2].d[2];
}

/**
 * Create a 3x3 Identity Matrix (column major)
 */
void mat3_mkidentity(mat4* self) {
  static float identity[] = {
    1, 0, 0,
    0, 1, 0,
    0, 0, 1
  };

  memcpy(self, identity, sizeof(mat3));
}

/**
 * Create a 4x4 Identity Matrix (column major)
 */
void mat4_mkidentity(mat4* self) {
  static float identity[] = {
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1
  };

  memcpy(self, identity, sizeof(mat4));
}

/**
 * Create a 4x4 Translation Matrix (column major) that will translate a
 * vertex by a three dimensional vector (x, y, z)
 *
 * @param vec the three dimensional translation vector (x, y, z)
 */
void mat4_mktranslate(mat4* self, const vec3* vec) {
  mat4_mkidentity(self);
  memcpy(&self->d[3], vec, sizeof(vec3));
}

/**
 * Create a 4x4 Translation Matrix (column major) that will scale a
 * vertex by a three dimensional vector (x, y, z)
 *
 * @param vec the three dimensional translation vector (x, y, z)
 */
void mat4_mkscale(mat4* self, const vec3* vec) {
  mat4_mkidentity(self);

  self->d[0].d[0] = vec->d[0];
  self->d[1].d[1] = vec->d[1];
  self->d[2].d[2] = vec->d[2];
}

void mat4_mkrotate(mat4* self, const vec3& rot) {
  mat4 rotx;
  mat4_mkrotate_x(&rotx, rot.d[0]);
  mat4 roty;
  mat4_mkrotate_y(&roty, rot.d[1]);
  mat4 rotz;
  mat4_mkrotate_z(&rotz, rot.d[2]);

  mat4 tmp;
  mat4_mul(&tmp, &rotx, &roty);
  mat4_mul(self, &tmp, &rotz);
}

/**
 * Create a 4x4 Rotation Matrix (column major) that will translate a vertex
 * by alpha degrees around the x axis
 *
 * @param alpha the angle of rotation in radians (0-2pi)
 */
void mat4_mkrotate_x(mat4* self, float alpha) {
  float a_sin = sin(alpha),
        a_cos = cos(alpha);

  mat4_mkidentity(self);

  self->d[1].d[1] = a_cos;
  self->d[1].d[2] = a_sin;
  self->d[2].d[1] = -a_sin;
  self->d[2].d[2] = a_cos;
}

/**
 * Create a 4x4 Rotation Matrix (column major) that will translate a vertex
 * by alpha degrees around the y axis
 *
 * @param alpha the angle of rotation in radians (0-2pi)
 */
void mat4_mkrotate_y(mat4* self, float alpha) {
  float a_sin = sin(alpha),
        a_cos = cos(alpha);

  mat4_mkidentity(self);

  self->d[0].d[0] = a_cos;
  self->d[0].d[2] = -a_sin;
  self->d[2].d[0] = a_sin;
  self->d[2].d[2] = a_cos;
}

/**
 * Create a 4x4 Rotation Matrix (column major) that will translate a vertex
 * by alpha degrees around the z axis
 *
 * @param alpha the angle of rotation in radians (0-2pi)
 */
void mat4_mkrotate_z(mat4* self, float alpha) {
  float a_sin = sin(alpha),
        a_cos = cos(alpha);

  mat4_mkidentity(self);

  self->d[0].d[0] = a_cos;
  self->d[0].d[1] = a_sin;
  self->d[1].d[0] = -a_sin;
  self->d[1].d[1] = a_cos;
}

/**
 * Multiply two 4x4 (column major) matrices (matrix product)
 */
void mat4_mul(mat4* dst, const mat4* left, const mat4* right) {
  int j, k, i;
  float s;

  for (j = 0; j < 4; j++) {
    for (i = 0; i < 4; i++) {
      s = 0.0f;

      for (k = 0; k < 4; k++) {
        s += left->d[k].d[i] * right->d[j].d[k];
      }

      dst->d[j].d[i] = s;
    }
  }
}

/**
 * Subtract two three-dimensional vectors
 */
void vec3_sub(vec3* dst, const vec3* left, const vec3* right) {
  dst->d[0] = left->d[0] - right->d[0];
  dst->d[1] = left->d[1] - right->d[1];
  dst->d[2] = left->d[2] - right->d[2];
}

/**
 * Normalize a three-dimensional vector
 */
void vec3_normalize(vec3* self) {
  float m = vec3_magnitude(self);

  self->d[0] /= m;
  self->d[1] /= m;
  self->d[2] /= m;
}

/**
 * Returns the magnitude of a three-dimensional vector
 */
float vec3_magnitude(const vec3* self) {
  float x = self->d[0],
        y = self->d[1],
        z = self->d[2];

  return sqrt(x * x + y * y + z * z);
}

/**
 * Takes the cross product of two three-dimensional vectors
 */
void vec3_cross(vec3* dst, const vec3* left, const vec3* right) {
  dst->d[0] = left->d[1] * right->d[2] - left->d[2] * right->d[1];
  dst->d[1] = left->d[2] * right->d[0] - left->d[0] * right->d[2];
  dst->d[2] = left->d[0] * right->d[1] - left->d[1] * right->d[0];
}

vec2 vadd(const vec2& left, const vec2& right) {
  return {
    left.d[0] + right.d[0],
    left.d[1] + right.d[1]
  };
}

vec3 vadd(const vec3& left, const vec3& right) {
  return {
    left.d[0] + right.d[0],
    left.d[1] + right.d[1],
    left.d[2] + right.d[2]
  };
}

vec2 vsub(const vec2& left, const vec2& right) {
  return {
    left.d[0] - right.d[0],
    left.d[1] - right.d[1]
  };
}

vec3 vsub(const vec3& left, const vec3& right) {
  return {
    left.d[0] - right.d[0],
    left.d[1] - right.d[1],
    left.d[2] - right.d[2]
  };
}

vec2 vsmul(const vec2& vec, float scalar) {
  return {
    vec.d[0] * scalar,
    vec.d[1] * scalar,
  };
}

vec3 vsmul(const vec3& vec, float scalar) {
  return {
    vec.d[0] * scalar,
    vec.d[1] * scalar,
    vec.d[2] * scalar
  };
}

vec3 mvprod(const mat3& mat, const vec3& vec) {
  return {
    mat.d[0].d[0] * vec.d[0] + mat.d[1].d[0] * vec.d[1] + mat.d[2].d[0] * vec.d[2],
    mat.d[0].d[1] * vec.d[0] + mat.d[1].d[1] * vec.d[1] + mat.d[2].d[1] * vec.d[2],
    mat.d[0].d[2] * vec.d[0] + mat.d[1].d[2] * vec.d[1] + mat.d[2].d[2] * vec.d[2],
  };
}

vec4 mvprod(const mat4& mat, const vec4& vec) {
  return {
    mat.d[0].d[0] * vec.d[0] + mat.d[1].d[0] * vec.d[1] + mat.d[2].d[0] * vec.d[2] + mat.d[3].d[0] * vec.d[3],
    mat.d[0].d[1] * vec.d[0] + mat.d[1].d[1] * vec.d[1] + mat.d[2].d[1] * vec.d[2] + mat.d[3].d[1] * vec.d[3],
    mat.d[0].d[2] * vec.d[0] + mat.d[1].d[2] * vec.d[1] + mat.d[2].d[2] * vec.d[2] + mat.d[3].d[2] * vec.d[3],
    mat.d[0].d[3] * vec.d[0] + mat.d[1].d[3] * vec.d[1] + mat.d[2].d[3] * vec.d[2] + mat.d[3].d[3] * vec.d[3],
  };
}

float vdot(const vec3& left, const vec3& right) {
  return left.d[0] * right.d[0] + left.d[1] * right.d[1] + left.d[2] * right.d[2];
}

vec3 vcross(const vec3& left, const vec3& right) {
  return {
    left.d[1] * right.d[2] - left.d[2] * right.d[1],
    left.d[2] * right.d[0] - left.d[0] * right.d[2],
    left.d[0] * right.d[1] - left.d[1] * right.d[0],
  };
}

vec2 vnormalize(const vec2& vec) {
  float m = vmag(vec);

  return {
    vec.d[0] / m,
    vec.d[1] / m
  };
}

vec3 normalize(const vec3& vec) {
  float m = vec3_magnitude(&vec);

  return {
    vec.d[0] / m,
    vec.d[1] / m,
    vec.d[2] / m
  };
}

double fclamp(double val, double low, double high) {
  if (val < low) {
    return low;
  } else if (val > high) {
    return high;
  } else {
    return val;
  }
}

double vmag(const vec2& vec) {
  float x = vec.d[0],
        y = vec.d[1];

  return sqrt(x * x + y * y);
}

double vmag(const vec3& vec) {
  return vec3_magnitude(&vec);
}

