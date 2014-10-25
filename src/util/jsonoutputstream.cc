/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnordmetric/util/jsonoutputstream.h>

namespace fnordmetric {
namespace util {

JSONOutputStream::JSONOutputStream(
    std::shared_ptr<OutputStream> output_stream) :
    output_(output_stream) {}


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
  output_->printf("\"%s\"", escapeString(string).c_str());
}

void JSONOutputStream::addFloat(double value) {
  output_->printf("\"%f\"", value);
}

void JSONOutputStream::addInteger(int value) {
  output_->printf("\"%i\"", value);
}

void JSONOutputStream::beginArray() {
  output_->printf("[");
}

void JSONOutputStream::endArray() {
  output_->printf("]");
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


}
}
