/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V. <paul@asmuth.com>
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <cmath>
#include "json.h"
#include "stringutil.h"

namespace json {

JSONToken::JSONToken(
    kTokenType _type,
    const std::string& _data) :
    type(_type),
    data(_data),
    size(1) {}

JSONToken::JSONToken(
    kTokenType _type) :
    type(_type),
    size(1) {}

ReturnCode parseJSON(const std::string& json, JSONObject* obj) {
  JSONInputStream json_is(StringInputStream::fromString(json));
  return parseJSON(&json_is, obj);
}

ReturnCode parseJSON(const Buffer& json, JSONObject* obj) {
  JSONInputStream json_is(BufferInputStream::fromBuffer(&json));
  return parseJSON(&json_is, obj);
}

ReturnCode parseJSON(JSONInputStream* json, JSONObject* obj) {
  std::stack<size_t> stack;

  kTokenType token;
  std::string token_str;
  std::string err;
  while (json->readNextToken(&token, &token_str, &err)) {
    switch (token) {
      case JSON_ARRAY_BEGIN:
      case JSON_OBJECT_BEGIN:
        obj->emplace_back(token);
        stack.push(obj->size() - 1);
        break;

      case JSON_ARRAY_END:
      case JSON_OBJECT_END: {
        obj->emplace_back(token);

        if (stack.empty()) {
          return ReturnCode::error("EPARSE", "unbalanced braces");
        }

        (*obj)[stack.top()].size = obj->size() - stack.top();
        stack.pop();
        break;
      }

      case JSON_STRING:
      case JSON_NUMBER:
        obj->emplace_back(token, token_str);
        break;

      case JSON_TRUE:
      case JSON_FALSE:
      case JSON_NULL:
        obj->emplace_back(token);
        break;
    }

    token_str.clear();
  }

  if (!err.empty()) {
    return ReturnCode::error("EPARSE", err);
  }

  if (!stack.empty()) {
    return ReturnCode::error(
        "EPARSE",
        "invalid JSON. unexpected end of stream");
  }

  return ReturnCode::success();
}

Option<String> objectGetString(
    const JSONObject& obj,
    const std::string& key) {
  return objectGetString(obj.begin(), obj.end(), key);
}

Option<String> objectGetString(
    JSONObject::const_iterator begin,
    JSONObject::const_iterator end,
    const std::string& key) {
  auto iter = objectLookup(begin, end, key);

  if (iter != end && iter->type == JSON_STRING) {
    return Some(iter->data);
  } else {
    return None<String>();
  }
}

Option<uint64_t> objectGetUInt64(
    const JSONObject& obj,
    const std::string& key) {
  return json::objectGetUInt64(obj.begin(), obj.end(), key);
}

Option<uint64_t> objectGetUInt64(
    JSONObject::const_iterator begin,
    JSONObject::const_iterator end,
    const std::string& key) {
  auto iter = objectLookup(begin, end, key);

  if (iter != end && (iter->type == JSON_STRING || iter->type == JSON_NUMBER)) {
    return Some<uint64_t>(std::stoul(iter->data));
  } else {
    return None<uint64_t>();
  }
}

Option<double> objectGetFloat(
    const JSONObject& obj,
    const std::string& key) {
  return json::objectGetFloat(obj.begin(), obj.end(), key);
}

Option<double> objectGetFloat(
    JSONObject::const_iterator begin,
    JSONObject::const_iterator end,
    const std::string& key) {
  auto iter = objectLookup(begin, end, key);

  if (iter != end && (iter->type == JSON_STRING || iter->type == JSON_NUMBER)) {
    return Some<double>(std::stod(iter->data));
  } else {
    return None<double>();
  }
}

Option<bool> objectGetBool(
    const JSONObject& obj,
    const std::string& key) {
  return json::objectGetBool(obj.begin(), obj.end(), key);
}

Option<bool> objectGetBool(
    JSONObject::const_iterator begin,
    JSONObject::const_iterator end,
    const std::string& key) {
  auto iter = objectLookup(begin, end, key);

  if (iter != end) {
    switch (iter->type) {

      case JSON_TRUE:
        return Some<bool>(true);

      case JSON_FALSE:
        return Some<bool>(false);

      case JSON_STRING:
        if (iter->data == "true" || iter->data == "TRUE")
          return Some<bool>(true);
        if (iter->data == "false" || iter->data == "FALSE")
          return Some<bool>(false);

      default:
        break;

    }
  }

  return None<bool>();
}

JSONObject::const_iterator objectLookup(
    const JSONObject& obj,
    const std::string& key) {
  return objectLookup(obj.begin(), obj.end(), key);
}

JSONObject::const_iterator objectLookup(
    JSONObject::const_iterator begin,
    JSONObject::const_iterator end,
    const std::string& key) {
  if (begin == end) {
    return end;
  }

  if (begin->type != json::JSON_OBJECT_BEGIN) {
    return end;
  }

  auto oend = begin + begin->size;
  if (oend > end) {
    return end;
  }

  for (++begin; begin < end;) {
    switch (begin->type) {
      case json::JSON_STRING:
        if (begin->data == key) {
          return ++begin;
        }

      default:
        break;
    }

    if (++begin == oend) {
      break;
    } else {
      begin += begin->size;
    }
  }

  return end;
}

size_t arrayLength(const JSONObject& obj) {
  return arrayLength(obj.begin(), obj.end());
}

size_t arrayLength(
    JSONObject::const_iterator begin,
    JSONObject::const_iterator end) {
  if (begin == end) {
    return 0;
  }

  if (begin->type != json::JSON_ARRAY_BEGIN) {
    return 0;
  }

  auto aend = begin + begin->size - 1;
  if (aend > end) {
    return 0;
  }

  size_t length = 0;
  for (++begin; begin < aend; begin += begin->size) {
    length++;
  }

  return length;
}

JSONObject::const_iterator arrayLookup(
    const JSONObject& obj,
    size_t index) {
  return arrayLookup(obj.begin(), obj.end(), index);
}

JSONObject::const_iterator arrayLookup(
    JSONObject::const_iterator begin,
    JSONObject::const_iterator end,
    size_t index) {
  if (begin == end) {
    return end;
  }

  if (begin->type != json::JSON_ARRAY_BEGIN) {
    return end;
  }

  auto aend = begin + begin->size - 1;
  if (aend > end || begin == aend) {
    return end;
  }

  size_t idx = 0;
  for (++begin; begin < aend; begin += begin->size) {
    if (idx++ == index) {
      return begin;
    }
  }

  return end;
}

Option<String> arrayGetString(
    const JSONObject& obj,
    size_t index) {
  return arrayGetString(obj.begin(), obj.end(), index);
}

Option<String> arrayGetString(
    JSONObject::const_iterator begin,
    JSONObject::const_iterator end,
    size_t index) {
  auto iter = arrayLookup(begin, end, index);

  if (iter != end && iter->type == JSON_STRING) {
    return Some(iter->data);
  } else {
    return None<String>();
  }
}


JSONOutputStream::JSONOutputStream(
    std::unique_ptr<OutputStream> output_stream) {
  output_.reset(output_stream.release());
}

void JSONOutputStream::write(const JSONObject& obj) {
  for (const auto& t : obj) {
    emplace_back(t.type, t.data);
  }
}

void JSONOutputStream::emplace_back(kTokenType token) {
  emplace_back(token, "");
}

void JSONOutputStream::emplace_back(const JSONToken& token) {
  emplace_back(token.type, token.data);
}

void JSONOutputStream::emplace_back(
    kTokenType token,
    const std::string& data) {

  switch (token) {
    case JSON_ARRAY_END:
      endArray();

      if (!stack_.empty()) {
        stack_.pop();
      }

      if (!stack_.empty()) {
        stack_.top().second++;
      }
      return;

    case JSON_OBJECT_END:
      endObject();

      if (!stack_.empty()) {
        stack_.pop();
      }

      if (!stack_.empty()) {
        stack_.top().second++;
      }
      return;

    default:
      break;

  }

  if (!stack_.empty() && stack_.top().second > 0) {
    switch (stack_.top().first) {
      case JSON_ARRAY_BEGIN:
        addComma();
        break;

      case JSON_OBJECT_BEGIN:
        (stack_.top().second % 2 == 0) ? addComma() : addColon();
        break;

      default:
        break;
    }
  }

  switch (token) {

    case JSON_ARRAY_BEGIN:
      beginArray();
      stack_.emplace(JSON_ARRAY_BEGIN, 0);
      break;

    case JSON_OBJECT_BEGIN:
      beginObject();
      stack_.emplace(JSON_OBJECT_BEGIN, 0);
      break;

    case JSON_STRING:
      addString(data);

      if (!stack_.empty()) {
        stack_.top().second++;
      }
      break;

    case JSON_NUMBER:
      addString(data);

      if (!stack_.empty()) {
        stack_.top().second++;
      }
      break;

    case JSON_TRUE:
      addTrue();

      if (!stack_.empty()) {
        stack_.top().second++;
      }
      break;

    case JSON_FALSE:
      addFalse();

      if (!stack_.empty()) {
        stack_.top().second++;
      }
      break;

    case JSON_NULL:
      addNull();

      if (!stack_.empty()) {
        stack_.top().second++;
      }
      break;

    default:
      break;
  }
}

void JSONOutputStream::beginObject() {
  output_->printf("{");
}

void JSONOutputStream::endObject() {
  output_->printf("}");
}

void JSONOutputStream::addObjectEntry(const std::string& key) {
  output_->printf("\"%s\": ", escapeString(key).c_str());
}

void JSONOutputStream::addComma() {
  output_->printf(",");
}

void JSONOutputStream::addColon() {
  output_->printf(":");
}

void JSONOutputStream::addString(const std::string& string) {
  output_->write("\"");
  output_->write(escapeString(string));
  output_->write("\"");
}

void JSONOutputStream::addInteger(int64_t value) {
  output_->write(StringUtil::toString(value));
}

void JSONOutputStream::addNull() {
  output_->write("null");
}

void JSONOutputStream::addBool(bool val) {
  if (val) {
    addTrue();
  } else {
    addFalse();
  }
}

void JSONOutputStream::addTrue() {
  output_->write("true");
}

void JSONOutputStream::addFalse() {
  output_->write("false");
}

void JSONOutputStream::addFloat(double value) {
  if (std::isnormal(value) || value == 0.0f) {
    output_->write(StringUtil::toString(value));
  } else {
    addNull();
  }
}

void JSONOutputStream::beginArray() {
  output_->printf("[");
}

void JSONOutputStream::endArray() {
  output_->printf("]");
}

/*
template <>
void JSONOutputStream::addValue(const std::string& value) {
  addString(value);
}

template <>
void JSONOutputStream::addValue(const int& value) {
  output_->write(StringUtil::toString(value));
}

template <>
void JSONOutputStream::addValue(const unsigned long& value) {
  output_->write(StringUtil::toString(value));
}

template <>
void JSONOutputStream::addValue(const unsigned long long& value) {
  output_->write(StringUtil::toString(value));
}

template <>
void JSONOutputStream::addValue(const double& value) {
  addFloat(value);
}

template <>
void JSONOutputStream::addValue(const bool& value) {
  value ? addTrue() : addFalse();
}

template <>
void JSONOutputStream::addValue(const std::nullptr_t& value) {
  addNull();
}
*/

std::string escapeString(const std::string& string) {
  std::string new_str;

  for (int i = 0; i < string.size(); ++i) {
    switch (string.at(i)) {
      case 0x00:
        new_str += "\\u0000";
        break;
      case 0x01:
        new_str += "\\u0001";
        break;
      case 0x02:
        new_str += "\\u0002";
        break;
      case 0x03:
        new_str += "\\u0003";
        break;
      case 0x04:
        new_str += "\\u0004";
        break;
      case 0x05:
        new_str += "\\u0005";
        break;
      case 0x06:
        new_str += "\\u0006";
        break;
      case 0x07:
        new_str += "\\u0007";
        break;
      case '\b':
        new_str += "\\b";
        break;
      case '\t':
        new_str += "\\t";
        break;
      case '\n':
        new_str += "\\n";
        break;
      case 0x0b:
        new_str += "\\u000b";
        break;
      case '\f':
        new_str += "\\f";
        break;
      case '\r':
        new_str += "\\r";
        break;
      case 0x0e:
        new_str += "\\u000e";
        break;
      case 0x0f:
        new_str += "\\u000f";
        break;
      case 0x10:
        new_str += "\\u0010";
        break;
      case 0x11:
        new_str += "\\u0011";
        break;
      case 0x12:
        new_str += "\\u0012";
        break;
      case 0x13:
        new_str += "\\u0013";
        break;
      case 0x14:
        new_str += "\\u0014";
        break;
      case 0x15:
        new_str += "\\u0015";
        break;
      case 0x16:
        new_str += "\\u0016";
        break;
      case 0x17:
        new_str += "\\u0017";
        break;
      case 0x18:
        new_str += "\\u0018";
        break;
      case 0x19:
        new_str += "\\u0019";
        break;
      case 0x1a:
        new_str += "\\u001a";
        break;
      case 0x1b:
        new_str += "\\u001b";
        break;
      case 0x1c:
        new_str += "\\u001c";
        break;
      case 0x1d:
        new_str += "\\u001d";
        break;
      case 0x1e:
        new_str += "\\u001e";
        break;
      case 0x1f:
        new_str += "\\u001f";
        break;
      case '"':
        new_str += "\\\"";
        break;
      case '\\':
        new_str += "\\\\";
        break;
      default:
        new_str += string.at(i);
    }
  }

  return new_str;
}

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
    std::string* token_data,
    std::string* error) {
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
        return readString(token_data, error);
      }

      case 't':
        *token_type = JSON_TRUE;
        return expectString("true", error);

      case 'f':
        *token_type = JSON_FALSE;
        return expectString("false", error);

      case 'n':
        *token_type = JSON_NULL;
        return expectString("null", error);

      case 0:
        return false;

      default:
        *error =
            std::string("invalid json, unexpected char: ") +
            std::string(cur_, 1);

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

bool JSONInputStream::readString(std::string* dst, std::string* err) {
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
            *err = "invalid escape sequence";
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
        *err = "invalid json: unterminated string";
        return false;

    }
  }
}

bool JSONInputStream::expectString(
    const std::string& expect,
    std::string* error) {
  std::string str;
  str += cur_;
  input_->readNextBytes(&str, expect.length() - 1);

  if (str != expect) {
    *error = "invalid json. expected '" + expect + "', got '" + str + "'";
    return false;
  }

  advanceCursor();
  return true;
}

void JSONInputStream::advanceCursor() {
  if (!input_->readNextByte(&cur_)) {
    cur_ = 0;
  }
}

} // namespace json

template <>
std::string StringUtil::toString(json::kTokenType type) {
  switch (type) {
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

