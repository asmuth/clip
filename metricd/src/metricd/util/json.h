/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V. <paul@asmuth.com>
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#pragma once
#include <set>
#include <stack>
#include <vector>
#include <metricd/util/outputstream.h>
#include <metricd/util/inputstream.h>
#include <metricd/util/option.h>
#include <metricd/util/return_code.h>

namespace json {
class JSONOutputStream;
class JSONInputStream;

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

struct JSONToken {
  JSONToken(kTokenType _type, const std::string& _data);
  JSONToken(kTokenType _type);
  kTokenType type;
  std::string data;
  uint32_t size;
};

typedef std::vector<JSONToken> JSONObject;

ReturnCode parseJSON(const std::string& json, JSONObject* obj);
ReturnCode parseJSON(const Buffer& json, JSONObject* obj);
ReturnCode parseJSON(JSONInputStream* json, JSONObject* obj);

JSONObject::const_iterator objectLookup(
    const JSONObject& obj,
    const std::string& key);

JSONObject::const_iterator objectLookup(
    JSONObject::const_iterator begin,
    JSONObject::const_iterator end,
    const std::string& key);

Option<String> objectGetString(
    const JSONObject& obj,
    const std::string& key);

Option<String> objectGetString(
    JSONObject::const_iterator begin,
    JSONObject::const_iterator end,
    const std::string& key);

Option<uint64_t> objectGetUInt64(
    const JSONObject& obj,
    const std::string& key);

Option<uint64_t> objectGetUInt64(
    JSONObject::const_iterator begin,
    JSONObject::const_iterator end,
    const std::string& key);

Option<double> objectGetFloat(
    const JSONObject& obj,
    const std::string& key);

Option<double> objectGetFloat(
    JSONObject::const_iterator begin,
    JSONObject::const_iterator end,
    const std::string& key);

Option<bool> objectGetBool(
    const JSONObject& obj,
    const std::string& key);

Option<bool> objectGetBool(
    JSONObject::const_iterator begin,
    JSONObject::const_iterator end,
    const std::string& key);

size_t arrayLength(const JSONObject& obj);

size_t arrayLength(
    JSONObject::const_iterator begin,
    JSONObject::const_iterator end);

JSONObject::const_iterator arrayLookup(
    const JSONObject& obj,
    size_t index);

JSONObject::const_iterator arrayLookup(
    JSONObject::const_iterator begin,
    JSONObject::const_iterator end,
    size_t index);

Option<String> arrayGetString(
    const JSONObject& obj,
    size_t index);

Option<String> arrayGetString(
    JSONObject::const_iterator begin,
    JSONObject::const_iterator end,
    size_t index);

std::string escapeString(const std::string& string);

class JSONOutputStream {
public:

  JSONOutputStream(std::unique_ptr<OutputStream> output_stream);

  void write(const JSONObject& obj);

  void emplace_back(kTokenType token);
  void emplace_back(kTokenType token, const std::string& data);
  void emplace_back(const JSONToken& token);

  void beginObject();
  void endObject();
  void addObjectEntry(const std::string& key);
  void beginArray();
  void endArray();
  void addComma();
  void addColon();
  void addString(const std::string& string);
  void addFloat(double value);
  void addInteger(int64_t value);
  void addNull();
  void addBool(bool val);
  void addTrue();
  void addFalse();

protected:
  std::stack<std::pair<kTokenType, int>> stack_;
  std::shared_ptr<OutputStream> output_;
};

class JSONInputStream {
public:

  explicit JSONInputStream(std::unique_ptr<InputStream> input);
  JSONInputStream(JSONInputStream&& other);
  JSONInputStream(const JSONInputStream& other) = delete;
  JSONInputStream& operator=(const JSONInputStream& other) = delete;

  bool readNextToken(
      kTokenType* token_type,
      std::string* token_data,
      std::string* error);

protected:
  void readNumber(std::string* dst);
  bool readString(std::string* dst, std::string* error);
  bool expectString(const std::string& expect, std::string* error);
  void advanceCursor();

  std::unique_ptr<InputStream> input_;
  char cur_;
};

} // namespace json

