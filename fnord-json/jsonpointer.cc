/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <stdlib.h>
#include <string>
#include "fnord-base/inspect.h"
#include "fnord-base/stringutil.h"
#include "fnord/json/jsonpointer.h"

namespace fnord {
namespace json {

JSONPointer::JSONPointer() : path_("") {}

JSONPointer::JSONPointer(const char* path) : path_(path) {}

JSONPointer::JSONPointer(std::string path) : path_(path) {}

void JSONPointer::escape(std::string* str) {
  StringUtil::replaceAll(str, "~", "~0");
  StringUtil::replaceAll(str, "/", "~1");
}

void JSONPointer::push(std::string str) {
  JSONPointer::escape(&str);
  path_ += "/";
  path_ += str;
}

void JSONPointer::pop() {
  auto cur = path_.end() - 1;
  for (; cur >= path_.begin() && *cur != '/'; cur--);
  path_.erase(cur, path_.end());
}

std::string JSONPointer::head() const {
  auto cur = path_.end() - 1;
  for (; cur >= path_.begin() && *cur != '/'; cur--);
  return std::string(cur + 1, path_.end());
}


} // namespace json

template <>
std::string inspect(const json::JSONPointer& ptr) {
  return ptr.toString();
}

} // namsepace fnord

