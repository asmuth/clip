/*
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "fnord-base/UTF8.h"
#include "fnord-base/exception.h"

namespace fnord {

char32_t UTF8::nextCodepoint(const char** cur, const char* end_) {
  auto begin = reinterpret_cast<const uint8_t*>(*cur);
  auto end = reinterpret_cast<const uint8_t*>(end_);

  if (begin >= end) {
    return 0;
  }

  if (*begin < 0b10000000) {
    return *(*cur)++;
  }

  if ((*begin & 0b11100000) == 0b11000000) {
    if (begin + 1 >= end) {
      RAISE(kEncodingError, "invalid UTF8 encoding");
    }

    char32_t chr;
    chr  = (*(*cur)++ & 0b00011111) << 6;
    chr |= (*(*cur)++ & 0b00111111);

    return chr;
  }

  if ((*begin & 0b11110000) == 0b11100000) {
    if (begin + 2 >= end) {
      RAISE(kEncodingError, "invalid UTF8 encoding");
    }

    char32_t chr;
    chr  = (*(*cur)++ & 0b00001111) << 12;
    chr |= (*(*cur)++ & 0b00111111) << 6;
    chr |= (*(*cur)++ & 0b00111111);

    return chr;
  }

  if ((*begin & 0b11111000) == 0b11110000) {
    if (begin + 3 >= end) {
      RAISE(kEncodingError, "invalid UTF8 encoding");
    }

    char32_t chr;
    chr  = (*(*cur)++ & 0b00000111) << 18;
    chr |= (*(*cur)++ & 0b00111111) << 12;
    chr |= (*(*cur)++ & 0b00111111) << 6;
    chr |= (*(*cur)++ & 0b00111111);

    return chr;
  }

  if ((*begin & 0b11111100) == 0b11111000) {
    if (begin + 4 >= end) {
      RAISE(kEncodingError, "invalid UTF8 encoding");
    }

    char32_t chr;
    chr  = (*(*cur)++ & 0b00000011) << 14;
    chr |= (*(*cur)++ & 0b00111111) << 18;
    chr |= (*(*cur)++ & 0b00111111) << 12;
    chr |= (*(*cur)++ & 0b00111111) << 6;
    chr |= (*(*cur)++ & 0b00111111);

    return chr;
  }

  if ((*begin & 0b11111110) == 0b11111100) {
    if (begin + 5 >= end) {
      RAISE(kEncodingError, "invalid UTF8 encoding");
    }

    char32_t chr;
    chr  = (*(*cur)++ & 0b00000001) << 30;
    chr |= (*(*cur)++ & 0b00111111) << 24;
    chr |= (*(*cur)++ & 0b00111111) << 18;
    chr |= (*(*cur)++ & 0b00111111) << 12;
    chr |= (*(*cur)++ & 0b00111111) << 6;
    chr |= (*(*cur)++ & 0b00111111);

    return chr;
  }

  RAISE(kEncodingError, "invalid UTF8 encoding");
}

void UTF8::encodeCodepoint(char32_t codepoint, String* target) {
  if (codepoint < 0b10000000) {
    *target += (char) codepoint;
    return;
  }

  if (codepoint < 0b100000000000) {
    *target += (char) (0b11000000 | ((codepoint >> 6) & 0b00011111));
    *target += (char) (0b10000000 | (codepoint        & 0b00111111));
    return;
  }

  else if (codepoint < 0b10000000000000000) {
    *target += (char) (0b11100000 | ((codepoint >> 12) & 0b00001111));
    *target += (char) (0b10000000 | ((codepoint >> 6)  & 0b00111111));
    *target += (char) (0b10000000 | (codepoint         & 0b00111111));
    return;
  }

  else if (codepoint < 0b1000000000000000000000) {
    *target += (char) (0b11110000 | ((codepoint >> 18) & 0b00000111));
    *target += (char) (0b10000000 | ((codepoint >> 12) & 0b00111111));
    *target += (char) (0b10000000 | ((codepoint >> 6)  & 0b00111111));
    *target += (char) (0b10000000 | (codepoint         & 0b00111111));
    return;
  }

  else if (codepoint < 0b100000000000000000000000000) {
    *target += (char) (0b11111000 | ((codepoint >> 24) & 0b00000011));
    *target += (char) (0b10000000 | ((codepoint >> 18) & 0b00111111));
    *target += (char) (0b10000000 | ((codepoint >> 12) & 0b00111111));
    *target += (char) (0b10000000 | ((codepoint >> 6)  & 0b00111111));
    *target += (char) (0b10000000 | (codepoint         & 0b00111111));
  }

  else {
    *target += (char) (0b11111100 | ((codepoint >> 30) & 0b00000001));
    *target += (char) (0b10000000 | ((codepoint >> 24) & 0b00111111));
    *target += (char) (0b10000000 | ((codepoint >> 18) & 0b00111111));
    *target += (char) (0b10000000 | ((codepoint >> 11) & 0b00111111));
    *target += (char) (0b10000000 | ((codepoint >> 6)  & 0b00111111));
    *target += (char) (0b10000000 | (codepoint         & 0b00111111));
  }
}

}
