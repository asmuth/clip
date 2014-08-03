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
    OutputStream* output_stream) :
    output_(output_stream) {}


void JSONOutputStream::beginObject() {
  output_->printf("{");
}

void JSONOutputStream::endObject() {
  output_->printf("}");
}

// FIXPAUL escaping
void JSONOutputStream::addObjectEntry(const std::string& key) {
  output_->printf("\"%s\": ", key.c_str());
}

void JSONOutputStream::addComma() {
  output_->printf(",");
}

// FIXPAUL escaping
void JSONOutputStream::addString(const std::string& key) {
  output_->printf("\"%s\"", key.c_str());
}

void JSONOutputStream::beginArray() {
  output_->printf("[");
}

void JSONOutputStream::endArray() {
  output_->printf("]");
}

}
}
