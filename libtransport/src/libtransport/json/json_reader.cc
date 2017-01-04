/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V. <paul@asmuth.com>
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "libtransport/json/json_reader.h"

namespace libtransport {
namespace json {

JSONReader::JSONReader(
    ReadCallback read_cb) :
    read_cb_(read_cb),
    cur_(' ') {}

JSONReader::JSONReader(const std::string* str) : cur_(' '){
  size_t pos = 0;
  read_cb_ = [str, pos] (char* chr) mutable -> bool {
    if (pos < str->size()) {
      *chr = (*str)[pos++];
      return true;
    } else {
      return false;
    }
  };
}

JSONReader::JSONReader(
    JSONReader&& other) :
    read_cb_(std::move(other.read_cb_)),
    cur_(other.cur_) {
  other.cur_ = 0;
}

bool JSONReader::readNextToken(
    kTokenType* token_type,
    std::string* token_data) {
  token_data->clear();

  for (;;) {
    switch (cur_) {
      case '{':
        *token_type = JSON_OBJECT_BEGIN;
        advanceCursor();
        return true;

      case '}':
        *token_type = JSON_OBJECT_END;
        advanceCursor();
        return true;

      case '[':
        *token_type = JSON_ARRAY_BEGIN;
        advanceCursor();
        return true;

      case ']':
        *token_type = JSON_ARRAY_END;
        advanceCursor();
        return true;

      case ':':
      case ',':
      case ' ':
      case '\n':
      case '\r':
        advanceCursor();
        break;

      case '-':
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
        *token_type = JSON_NUMBER;
        readNumber(token_data);
        return true;

      case '"': {
        *token_type = JSON_STRING;
        return readString(token_data);
      }

      case 't':
        *token_type = JSON_TRUE;
        return expectString("true");

      case 'f':
        *token_type = JSON_FALSE;
        return expectString("false");

      case 'n':
        *token_type = JSON_NULL;
        return expectString("null");

      case 0:
        return false;

      default:
        error_ =
            std::string("invalid json, unexpected char: ") +
            std::string(cur_, 1);

        return false;

    }
  }
}

const std::string& JSONReader::getError() const {
  return error_;
}

void JSONReader::readNumber(std::string* dst) {
  for (;;) {
    switch (cur_) {
      case '-':
      case '+':
      case '.':
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
      case 'e':
      case 'E':
        *dst += cur_;
        /* fallthrough */

      case ' ':
        advanceCursor();
        break;

      default:
        return;
    }
  }
}

bool JSONReader::readString(std::string* dst) {
  for (;;) {
    advanceCursor();

    switch (cur_) {

      case '\\':
        advanceCursor();

        switch (cur_) {

          case '"':
            *dst += "\"";
            break;

          case '\\':
            *dst += "\\";
            break;

          case '/':
            *dst += "/";
            break;

          case 'b':
            *dst += "\b";
            break;

          case 'f':
            *dst += "\f";
            break;

          case 'n':
            *dst += "\n";
            break;

          case 'r':
            *dst += "\r";
            break;

          case 't':
            *dst += "\t";
            break;

          case 'u':
            // FIXME
            advanceCursor();
            advanceCursor();
            advanceCursor();
            advanceCursor();
            break;

          default:
            error_ = "invalid escape sequence";
            return false;

        }
        break;

      default:
        *dst += cur_;
        break;

      case '"':
        advanceCursor();
        return true;

      case 0:
        error_ = "invalid json: unterminated string";
        return false;

    }
  }
}

bool JSONReader::expectString(
    const std::string& expect) {
  std::string str;
  str += cur_;
  while (str.size() < expect.length()) {
    char chr;
    if (read_cb_(&chr)) {
      str += chr;
    } else {
      return false;
    }
  }

  if (str != expect) {
    error_ = "invalid json. expected '" + expect + "', got '" + str + "'";
    return false;
  }

  advanceCursor();
  return true;
}

void JSONReader::advanceCursor() {
  if (!read_cb_(&cur_)) {
    cur_ = 0;
  }
}

} // namespace json
} // namespace libtransport

