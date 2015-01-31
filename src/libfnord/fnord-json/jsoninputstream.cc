/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "fnord-base/exception.h"
#include "fnord-base/inspect.h"
#include "fnord-base/stringutil.h"
#include "fnord-json/jsoninputstream.h"

namespace fnord {
namespace json {

JSONInputStream::JSONInputStream(
    std::unique_ptr<InputStream> input) :
    input_(std::move(input)),
    cur_(' ') {}

JSONInputStream::JSONInputStream(
    JSONInputStream&& other) :
    input_(std::move(other.input_)),
    cur_(other.cur_) {
  other.cur_ = 0;
}

bool JSONInputStream::readNextToken(
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

      case '"':
        *token_type = JSON_STRING;
        readString(token_data);
        return true;

      case 't':
        *token_type = JSON_TRUE;
        expectString("true");
        return true;

      case 'f':
        *token_type = JSON_FALSE;
        expectString("false");
        return true;

      case 'n':
        *token_type = JSON_NULL;
        expectString("null");
        return true;

      case 0:
        return false;

      default:
        RAISE(kRuntimeError, "invalid json, unexpected char: %c", cur_);
        return false;

    }
  }
}

void JSONInputStream::readNumber(std::string* dst) {
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

// FIXPAUL profiling says this is slow slow slow (90% of rpc server time spent
// in this method)
void JSONInputStream::readString(std::string* dst) {
  bool escaped = false;
  for (;;) {
    advanceCursor();

    switch (cur_) {

      case 0:
        RAISE(kRuntimeError, "invalid json. unterminated string");
        return;

      case '\\':
        escaped = !escaped;
        break;

      case '"':
        if (!escaped) {
          advanceCursor();
          return;
        }
        /* fallthrough */

      default:
        escaped = false;
        *dst += cur_;
        break;
    }
  }
}

void JSONInputStream::expectString(const std::string& expect) {
  std::string str;
  str += cur_;
  input_->readNextBytes(&str, expect.length() - 1);

  if (str != expect) {
    RAISEF(kRuntimeError, "invalid json. expected '$0', got '$1'", expect, str);
  }

  advanceCursor();
}

void JSONInputStream::advanceCursor() {
  if (!input_->readNextByte(&cur_)) {
    cur_ = 0;
  }
}

} // namespace json

template <>
std::string inspect(const json::kTokenType& token) {
  switch (token) {
    case json::JSON_OBJECT_BEGIN: return "JSON_OBJECT_BEGIN";
    case json::JSON_OBJECT_END: return "JSON_OBJECT_END";
    case json::JSON_ARRAY_BEGIN: return "JSON_ARRAY_BEGIN";
    case json::JSON_ARRAY_END: return "JSON_ARRAY_END";
    case json::JSON_STRING: return "JSON_STRING";
    case json::JSON_NUMBER: return "JSON_NUMBER";
    case json::JSON_TRUE: return "JSON_TRUE";
    case json::JSON_FALSE: return "JSON_FALSE";
    case json::JSON_NULL: return "JSON_NULL";
  }
}

} // namespace fnord

