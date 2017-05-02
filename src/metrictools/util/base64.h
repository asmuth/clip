/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#pragma once
#include <stdlib.h>
#include <stdint.h>
#include <string>

class Base64 {
public:

  static void encode(const std::string& in, std::string* out);
  static void encode(const void* data, size_t size, std::string* out);
  static std::string encode(const std::string& in);
  static std::string encode(const void* data, size_t size);

  static void decode(const std::string& in, std::string* out);

};

/** IMPLEMENTATION **/

/**
 * based on the BASE64 encoding/decoding routines from
 *   http://www.opensource.apple.com/source/QuickTimeStreamingServer/QuickTimeStreamingServer-452/CommonUtilitiesLib/base64.c
 */

/* aaaack but it's fast and const should make it shared text page. */
static const unsigned char pr2six[256] = {
    /* ASCII table */
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 62, 64, 64, 64, 63,
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 64, 64, 64, 64, 64, 64,
    64,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 64, 64, 64, 64, 64,
    64, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
    41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64
};

void Base64::decode(const std::string& in, std::string* out) {
  auto bufcoded = in.c_str();
  const unsigned char* bufin = (const unsigned char *) bufcoded;
  while (pr2six[*(bufin++)] <= 63);
  int nprbytes = (bufin - (const unsigned char *) bufcoded) - 1;
  bufin = (const unsigned char *) bufcoded;

  out->reserve(in.length());

  while (nprbytes > 4) {
    *out += (unsigned char) (pr2six[*bufin] << 2 | pr2six[bufin[1]] >> 4);
    *out += (unsigned char) (pr2six[bufin[1]] << 4 | pr2six[bufin[2]] >> 2);
    *out += (unsigned char) (pr2six[bufin[2]] << 6 | pr2six[bufin[3]]);
    bufin += 4;
    nprbytes -= 4;
  }

  /* Note: (nprbytes == 1) would be an error, so just ingore that case */
  if (nprbytes > 1) {
    *out += (unsigned char) (pr2six[*bufin] << 2 | pr2six[bufin[1]] >> 4);
  }
  if (nprbytes > 2) {
    *out += (unsigned char) (pr2six[bufin[1]] << 4 | pr2six[bufin[2]] >> 2);
  }
  if (nprbytes > 3) {
    *out += (unsigned char) (pr2six[bufin[2]] << 6 | pr2six[bufin[3]]);
  }
}

std::string Base64::encode(const std::string& in) {
  std::string out;
  encode(in.data(), in.size(), &out);
  return out;
}

void Base64::encode(const std::string& in, std::string* out) {
  encode(in.data(), in.size(), out);
}

std::string Base64::encode(const void* data, size_t size) {
  std::string out;
  encode(data, size, &out);
  return out;
}

void Base64::encode(const void* data, size_t size, std::string* out) {
  static const char chrtbl[] =
      "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

  auto len = size;
  auto s = (const char*) data;
  size_t i = 0;
  for (; i < len - 2; i += 3) {
    *out += chrtbl[(s[i] >> 2) & 0x3F];
    *out += chrtbl[((s[i] & 0x3) << 4) | ((int) (s[i + 1] & 0xF0) >> 4)];
    *out += chrtbl[((s[i + 1] & 0xF) << 2) | ((int) (s[i + 2] & 0xC0) >> 6)];
    *out += chrtbl[s[i + 2] & 0x3F];
  }

  if (i < len) {
    *out += chrtbl[(s[i] >> 2) & 0x3F];

    if (i == (len - 1)) {
      *out += chrtbl[((s[i] & 0x3) << 4)];
      *out += '=';
    } else {
      *out += chrtbl[((s[i] & 0x3) << 4) |((int) (s[i + 1] & 0xF0) >> 4)];
      *out += chrtbl[((s[i + 1] & 0xF) << 2)];
    }

    *out += '=';
  }
}

