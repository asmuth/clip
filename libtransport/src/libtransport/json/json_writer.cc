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
#include "libtransport/json/json_writer.h"
#include <cmath>

namespace libtransport {
namespace json {

static bool writeToString(
    std::string* str,
    const char* data,
    size_t len) {
  str->append(data, len);
  return true;
}

JSONWriter::JSONWriter(
    std::string* str) :
    JSONWriter(
        std::bind(
            &writeToString,
            str,
            std::placeholders::_1,
            std::placeholders::_2)) {}

JSONWriter::JSONWriter(WriteCallback write_cb) : write_cb_(write_cb) {}

bool JSONWriter::beginObject() {
  if (!stack_.empty() && stack_.top() == WriterState::OBJECT_ENTRY_KEY) {
    return false;
  }

  stack_.push(WriterState::OBJECT_ENTRY_KEY);
  return write("{");
}

bool JSONWriter::endObject() {
  if (stack_.empty()) {
    return false;
  }

  switch (stack_.top()) {
    case WriterState::OBJECT_NEXT:
    case WriterState::OBJECT_ENTRY_KEY:
      break;
    default:
      return false;
  }

  stack_.pop();
  advanceState();
  return write("}");
}

bool JSONWriter::beginArray() {
  if (!stack_.empty() && stack_.top() == WriterState::OBJECT_ENTRY_KEY) {
    return false;
  }

  stack_.push(WriterState::ARRAY);
  return write("[");
}

bool JSONWriter::endArray() {
  if (stack_.empty()) {
    return false;
  }

  switch (stack_.top()) {
    case WriterState::ARRAY:
    case WriterState::ARRAY_NEXT:
      break;
    default:
      return false;
  }

  stack_.pop();
  advanceState();
  return write("]");
}

bool JSONWriter::addString(const std::string& string) {
  return addScalar("\"" + escapeString(string) + "\"");
}

bool JSONWriter::addInteger(int64_t value) {
  return addScalar(std::to_string(value));
}

bool JSONWriter::addNull() {
  return addScalar("null");
}

bool JSONWriter::addBool(bool val) {
  if (val) {
    return addTrue();
  } else {
    return addFalse();
  }
}

bool JSONWriter::addTrue() {
  return addScalar("true");
}

bool JSONWriter::addFalse() {
  return addScalar("false");
}

bool JSONWriter::addFloat(double value) {
  if (std::isnormal(value) || value == 0.0f) {
    return addScalar(std::to_string(value));
  } else {
    return addNull();
  }
}

bool JSONWriter::addToken(
    kTokenType token_type,
    const std::string& token_data) {
  switch (token_type) {
    case JSON_ARRAY_BEGIN:
      return beginArray();
    case JSON_ARRAY_END:
      return endArray();
    case JSON_OBJECT_BEGIN:
      return beginObject();
    case JSON_OBJECT_END:
      return endObject();
    case JSON_STRING:
      return addString(token_data);
    case JSON_NUMBER:
      return addScalar(token_data);
    case JSON_TRUE:
      return addTrue();
    case JSON_FALSE:
      return addFalse();
    case JSON_NULL:
      return addNull();
    default:
      return false;
  }
}

bool JSONWriter::write(const std::string& str) {
  return write_cb_(str.data(), str.size());
}

bool JSONWriter::addScalar(const std::string& val) {
  if (stack_.empty()) {
    return false;
  }

  switch (stack_.top()) {

    case WriterState::OBJECT_NEXT:
      write("," + val + ":");
      break;

    case WriterState::OBJECT_ENTRY_KEY:
      write(val + ":");
      break;

    case WriterState::OBJECT_ENTRY_VALUE:
      write(val);
      break;

    case WriterState::ARRAY:
      write(val);
      break;

    case WriterState::ARRAY_NEXT:
      write("," + val);
      break;

  }

  advanceState();
  return true;
}

void JSONWriter::advanceState() {
  if (stack_.empty()) {
    return;
  }

  switch (stack_.top()) {

    case WriterState::OBJECT_NEXT:
      stack_.top() = WriterState::OBJECT_ENTRY_VALUE;
      break;

    case WriterState::OBJECT_ENTRY_KEY:
      stack_.top() = WriterState::OBJECT_ENTRY_VALUE;
      break;

    case WriterState::OBJECT_ENTRY_VALUE:
      stack_.top() = WriterState::OBJECT_NEXT;
      break;

    case WriterState::ARRAY:
      stack_.top() = WriterState::ARRAY_NEXT;
      break;

    case WriterState::ARRAY_NEXT:
      break;

  }
}

} // namespace json
} // namespace libtransport

