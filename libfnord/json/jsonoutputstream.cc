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
#include <fnord/json/jsonoutputstream.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

namespace fnord {
namespace json {

JSONOutputStream::JSONOutputStream(
    std::shared_ptr<io::OutputStream> output_stream) :
    output_(output_stream) {}

JSONOutputStream::JSONOutputStream(
    std::unique_ptr<io::OutputStream> output_stream) {
  output_.reset(output_stream.release());
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

template <>
void JSONOutputStream::addValue(const std::string& value) {
  addString(value);
}

template <>
void JSONOutputStream::addValue(const int& value) {
  addInteger(value);
}

template <>
void JSONOutputStream::addValue(const unsigned long& value) {
  addInteger(value);
}

template <>
void JSONOutputStream::addValue(const unsigned long long& value) {
  addInteger(value);
}

template <>
void JSONOutputStream::addValue(const double& value) {
  addFloat(value);
}

template <>
void JSONOutputStream::addValue(const bool& value) {
  value ? addTrue() : addFalse();
}

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

