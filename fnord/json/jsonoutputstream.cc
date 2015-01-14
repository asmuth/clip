/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnord/base/stringutil.h>
#include <fnord/base/inspect.h>
#include <fnord/json/jsonoutputstream.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

namespace fnord {
namespace json {

JSONOutputStream::JSONOutputStream(
    std::shared_ptr<OutputStream> output_stream) :
    output_(output_stream) {
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

void JSONOutputStream::addInteger(int value) {
  output_->write(StringUtil::toString(value));
}

void JSONOutputStream::addNull() {
  output_->write("null");
}

void JSONOutputStream::addTrue() {
  output_->write("true");
}

void JSONOutputStream::addFalse() {
  output_->write("false");
}

void JSONOutputStream::addFloat(double value) {
  output_->write(StringUtil::toString(value));
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

std::string JSONOutputStream::escapeString(const std::string& string) const {
  std::string new_str;

  for (int i = 0; i < string.size(); ++i) {
    switch (string.at(i)) {
      case '"':
        new_str += "\\\"";
        break;
      case '\n':
        new_str += "\\n";
        break;
      case '\t':
        new_str += "\\t";
        break;
      default:
        new_str += string.at(i);
    }
  }

  return new_str;
}


} // namespace json
} // namespace fnord

