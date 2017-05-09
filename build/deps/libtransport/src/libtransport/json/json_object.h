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
#include "libtransport/json/json.h"
#include <map>
#include <string>
#include <vector>

namespace libtransport {
namespace json {
class JSONObject;
class JSONArray;
class JSONString;
class JSONNumber;
class JSONBoolean;
class JSONNull;

class JSONStorage {
public:

  JSONStorage();
  virtual ~JSONStorage();
  JSONStorage(const JSONStorage& other) = delete;
  JSONStorage& operator=(const JSONStorage& other) = delete;

  JSONObject* allocObject();
  JSONArray* allocArray();
  JSONString* allocString(const std::string& str = "");
  JSONNumber* allocNumber();
  JSONNumber* allocNumber(const std::string& value);
  JSONBoolean* allocBoolean(bool value);
  JSONNull* allocNull();

  JSONElement* getRoot();
  bool hasRoot() const;
  const JSONElement* getRoot() const;
  JSONObject* getRootAsObject();
  const JSONObject* getRootAsObject() const;
  bool hasRootObject() const;
  JSONArray* getRootAsArray();
  const JSONArray* getRootAsArray() const;
  bool hasRootArray() const;
  void setRoot(JSONElement* root);

protected:
  std::vector<JSONElement*> elements_;
  JSONElement* root_;
};

class JSONObjectStorage : public JSONStorage {
public:
  JSONObjectStorage();
  JSONObject* operator*();
  JSONObject* operator*() const;
  JSONObject* operator->();
  JSONObject* operator->() const;
};

class JSONArrayStorage : public JSONStorage {
public:
  JSONArrayStorage();
  JSONArray* operator*();
  JSONArray* operator*() const;
  JSONArray* operator->();
  JSONArray* operator->() const;
};

class JSONElement {
public:

  virtual ~JSONElement() = default;

  JSONObject* getAsObject();
  const JSONObject* getAsObject() const;

  JSONArray* getAsArray();
  const JSONArray* getAsArray() const;

  JSONString* getAsString();
  const JSONString* getAsString() const;

  JSONNumber* getAsNumber();
  const JSONNumber* getAsNumber() const;

  JSONBoolean* getAsBoolean();
  const JSONBoolean* getAsBoolean() const;

  bool isObject() const;
  bool isArray() const;
  bool isString() const;
  bool isNumber() const;
  bool isBoolean() const;
  bool isNull() const;

  virtual std::string toString() const = 0;

};

class JSONObject : public JSONElement {
public:

  JSONObject(JSONStorage* storage);

  bool has(const std::string& key);

  size_t size() const;

  JSONElement* get(const std::string& key) const;

  void get(
      size_t index,
      const JSONElement** key,
      const JSONElement** value) const;

  JSONObject* getObject(const std::string& key) const;

  JSONArray* getArray(const std::string& key) const;

  std::string getString(
      const std::string& key,
      const std::string& fallback = "") const;

  uint64_t getUInt64(
      const std::string& key,
      uint64_t fallback = 0) const;

  int64_t getInt64(
      const std::string& key,
      int64_t fallback = 0) const;

  double getDouble(
      const std::string& key,
      double fallback = 0) const;

  bool getBoolean(
      const std::string& key,
      bool fallback = false) const;

  void set(const std::string& key, JSONElement* value);

  JSONObject* setObject(const std::string& key);

  JSONArray* setArray(const std::string& key);

  void setString(const std::string& key, const std::string value);

  void setUInt64(const std::string& key, uint64_t value);

  void setInt64(const std::string& key, int64_t value);

  void setDouble(const std::string& key, double value);

  void setBoolean(const std::string& key, bool value);

  void setNull(const std::string& key);

  std::string toString() const override;

protected:
  JSONStorage* storage_;
  std::vector<std::pair<const JSONElement*, JSONElement*>> properties_;
  std::map<std::string, uint64_t> properties_index_;
};

class JSONArray : public JSONElement {
public:

  JSONArray(JSONStorage* storage);

  size_t size() const;

  JSONElement* get(size_t index) const;

  JSONObject* getObject(size_t index) const;

  JSONArray* getArray(size_t index) const;

  std::string getString(
      size_t index,
      const std::string& fallback = "") const;

  uint64_t getUInt64(
      size_t index,
      uint64_t fallback = 0) const;

  int64_t getInt64(
      size_t index,
      int64_t fallback = 0) const;

  double getDouble(
      size_t index,
      double fallback = 0) const;

  bool getBoolean(
      size_t index,
      bool fallback = false) const;

  void append(JSONElement* value);

  JSONObject* appendObject();

  JSONArray* appendArray();

  void appendString(const std::string value);

  void appendUInt64(uint64_t value);

  void appendInt64(int64_t value);

  void appendDouble(double value);

  void appendBoolean(bool value);

  void appendNull();

  std::string toString() const override;

protected:
  JSONStorage* storage_;
  std::vector<JSONElement*> elements_;
};

class JSONString : public JSONElement {
public:

  JSONString(const std::string& value);

  const std::string& getString() const;
  void setString(const std::string& value);

  std::string toString() const override;

protected:
  std::string value_;
};

class JSONNumber : public JSONElement {
public:

  static JSONNumber fromUInt64(uint64_t value);
  static JSONNumber fromInt64(int64_t value);
  static JSONNumber fromDouble(double value);

  JSONNumber();
  JSONNumber(const std::string& value);

  uint64_t getUInt64() const;
  int64_t getInt64() const;
  double getDouble() const;

  void setUInt64(uint64_t value);
  void setInt64(int64_t value);
  void setDouble(double value);

  std::string toString() const override;

protected:
  std::string value_;
};

class JSONBoolean : public JSONElement {
public:

  JSONBoolean(bool value);

  bool getBoolean() const;
  void setBoolean(bool value);

  std::string toString() const override;

protected:
  bool value_;
};

class JSONNull : public JSONElement {
public:

  std::string toString() const override;

};

} // namespace json
} // namespace libtransport

