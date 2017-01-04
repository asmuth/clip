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
#pragma once
#include <functional>
#include <memory>
#include <set>
#include <stack>
#include <string>
#include <vector>

namespace libtransport {
namespace json {
class JSONReader;
class JSONWriter;
class JSONStorage;
class JSONElement;
using JSONElementRef = std::unique_ptr<JSONElement>;

enum kTokenType {
  JSON_OBJECT_BEGIN,
  JSON_OBJECT_END,
  JSON_ARRAY_BEGIN,
  JSON_ARRAY_END,
  JSON_STRING,
  JSON_NUMBER,
  JSON_TRUE,
  JSON_FALSE,
  JSON_NULL
};

using ReadCallback = std::function<bool (char* chr)>;
using WriteCallback = std::function<bool (const char*, size_t)>;

bool readJSON(
    JSONStorage* storage,
    JSONReader* reader);

bool readJSON(
    JSONStorage* storage,
    ReadCallback read_cb);

bool readJSON(
    JSONStorage* storage,
    const char* str,
    size_t len);

bool readJSON(
    JSONStorage* storage,
    const std::string* str);

bool writeJSON(
    const JSONElement* root,
    JSONWriter* writer);

bool writeJSON(
    const JSONElement* root,
    WriteCallback write_cb);

bool writeJSON(
    const JSONElement* root,
    std::string* str);

std::string escapeString(const std::string& string);

} // namespace json
} // namespace libtransport

