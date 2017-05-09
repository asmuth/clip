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
#include <assert.h>
#include "libtransport/json/json_object.h"

namespace libtransport {
namespace json {

JSONStorage::JSONStorage() : root_(nullptr) {}

JSONStorage::~JSONStorage() {
  for (auto e : elements_) {
    delete e;
  }
}

JSONObject* JSONStorage::allocObject() {
  auto elem = new JSONObject(this);
  elements_.emplace_back(elem);
  return elem;
}

JSONArray* JSONStorage::allocArray() {
  auto elem = new JSONArray(this);
  elements_.emplace_back(elem);
  return elem;
}

JSONString* JSONStorage::allocString(const std::string& str /* = "" */) {
  auto elem = new JSONString(str);
  elements_.emplace_back(elem);
  return elem;
}

JSONNumber* JSONStorage::allocNumber() {
  auto elem = new JSONNumber();
  elements_.emplace_back(elem);
  return elem;
}

JSONNumber* JSONStorage::allocNumber(const std::string& value) {
  auto elem = new JSONNumber(value);
  elements_.emplace_back(elem);
  return elem;
}

JSONBoolean* JSONStorage::allocBoolean(bool value) {
  auto elem = new JSONBoolean(value);
  elements_.emplace_back(elem);
  return elem;
}

JSONNull* JSONStorage::allocNull() {
  auto elem = new JSONNull();
  elements_.emplace_back(elem);
  return elem;
}

bool JSONStorage::hasRoot() const {
  return !!root_;
}

JSONElement* JSONStorage::getRoot() {
  return root_;
}

JSONObject* JSONStorage::getRootAsObject() {
  return dynamic_cast<JSONObject*>(root_);
}

const JSONObject* JSONStorage::getRootAsObject() const {
  return dynamic_cast<JSONObject*>(root_);
}

bool JSONStorage::hasRootObject() const {
  return !!dynamic_cast<JSONObject*>(root_);
}

JSONArray* JSONStorage::getRootAsArray() {
  return dynamic_cast<JSONArray*>(root_);
}

const JSONArray* JSONStorage::getRootAsArray() const {
  return dynamic_cast<JSONArray*>(root_);
}

bool JSONStorage::hasRootArray() const {
  return !!dynamic_cast<JSONArray*>(root_);
}

const JSONElement* JSONStorage::getRoot() const {
  return root_;
}

void JSONStorage::setRoot(JSONElement* root) {
  root_ = root;
}

JSONObjectStorage::JSONObjectStorage() {
  setRoot(allocObject());
}

JSONObject* JSONObjectStorage::operator*() {
  return (JSONObject*) root_;
}

JSONObject* JSONObjectStorage::operator*() const {
  return (JSONObject*) root_;
}

JSONObject* JSONObjectStorage::operator->() {
  return (JSONObject*) root_;
}

JSONObject* JSONObjectStorage::operator->() const {
  return (JSONObject*) root_;
}

JSONArrayStorage::JSONArrayStorage() {
  setRoot(allocArray());
}

JSONArray* JSONArrayStorage::operator*() {
  return (JSONArray*) root_;
}

JSONArray* JSONArrayStorage::operator*() const {
  return (JSONArray*) root_;
}

JSONArray* JSONArrayStorage::operator->() {
  return (JSONArray*) root_;
}

JSONArray* JSONArrayStorage::operator->() const {
  return (JSONArray*) root_;
}

JSONObject* JSONElement::getAsObject() {
  return dynamic_cast<JSONObject*>(this);
}

const JSONObject* JSONElement::getAsObject() const {
  return dynamic_cast<const JSONObject*>(this);
}

JSONArray* JSONElement::getAsArray() {
  return dynamic_cast<JSONArray*>(this);
}

const JSONArray* JSONElement::getAsArray() const {
  return dynamic_cast<const JSONArray*>(this);
}

JSONString* JSONElement::getAsString() {
  return dynamic_cast<JSONString*>(this);
}

const JSONString* JSONElement::getAsString() const {
  return dynamic_cast<const JSONString*>(this);
}

JSONNumber* JSONElement::getAsNumber() {
  return dynamic_cast<JSONNumber*>(this);
}

const JSONNumber* JSONElement::getAsNumber() const {
  return dynamic_cast<const JSONNumber*>(this);
}

JSONBoolean* JSONElement::getAsBoolean() {
  return dynamic_cast<JSONBoolean*>(this);
}

const JSONBoolean* JSONElement::getAsBoolean() const {
  return dynamic_cast<const JSONBoolean*>(this);
}

bool JSONElement::isObject() const {
  return !!dynamic_cast<const JSONObject*>(this);
}

bool JSONElement::isArray() const {
  return !!dynamic_cast<const JSONArray*>(this);
}

bool JSONElement::isString() const {
  return !!dynamic_cast<const JSONString*>(this);
}

bool JSONElement::isNumber() const {
  return !!dynamic_cast<const JSONNumber*>(this);
}

bool JSONElement::isBoolean() const {
  return !!dynamic_cast<const JSONBoolean*>(this);
}

bool JSONElement::isNull() const {
  return !!dynamic_cast<const JSONNull*>(this);
}

JSONObject::JSONObject(JSONStorage* storage) : storage_(storage) {}

size_t JSONObject::size() const {
  return properties_.size();
}

bool JSONObject::has(const std::string& key) {
  return properties_index_.count(key) > 0;
}

JSONElement* JSONObject::get(const std::string& key) const {
  auto iter = properties_index_.find(key);
  if (iter == properties_index_.end()) {
    return nullptr;
  } else {
    return properties_[iter->second].second;
  }
}

void JSONObject::get(
    size_t index,
    const JSONElement** key,
    const JSONElement** value) const {
  assert(index < properties_.size());
  *key = properties_[index].first;
  *value = properties_[index].second;
}

JSONObject* JSONObject::getObject(const std::string& key) const {
  auto elem = get(key);
  if (elem && elem->isObject()) {
    return elem->getAsObject();
  } else {
    return nullptr;
  }
}

JSONArray* JSONObject::getArray(const std::string& key) const {
  auto elem = get(key);
  if (elem && elem->isArray()) {
    return elem->getAsArray();
  } else {
    return nullptr;
  }
}

std::string JSONObject::getString(
    const std::string& key,
    const std::string& fallback /* = "" */) const {
  auto elem = get(key);
  if (elem && elem->isString()) {
    return elem->getAsString()->getString();
  } else {
    return fallback;
  }
}

uint64_t JSONObject::getUInt64(
    const std::string& key,
    uint64_t fallback /* = 0 */) const {
  auto elem = get(key);
  if (elem && elem->isNumber()) {
    return elem->getAsNumber()->getUInt64();
  } else {
    return fallback;
  }
}

int64_t JSONObject::getInt64(
    const std::string& key,
    int64_t fallback /* = 0 */) const {
  auto elem = get(key);
  if (elem && elem->isNumber()) {
    return elem->getAsNumber()->getInt64();
  } else {
    return fallback;
  }
}

double JSONObject::getDouble(
    const std::string& key,
    double fallback /* = 0 */) const {
  auto elem = get(key);
  if (elem && elem->isNumber()) {
    return elem->getAsNumber()->getDouble();
  } else {
    return fallback;
  }
}

bool JSONObject::getBoolean(
    const std::string& key,
    bool fallback /* = false */) const {
  auto elem = get(key);
  if (elem && elem->isBoolean()) {
    return elem->getAsBoolean()->getBoolean();
  } else {
    return fallback;
  }
}

void JSONObject::set(const std::string& key, JSONElement* value) {
  auto idx_iter = properties_index_.find(key);
  if (idx_iter == properties_index_.end()) {
    properties_index_.emplace(key, properties_.size());
    properties_.emplace_back(
        storage_->allocString(key),
        value);
  } else {
    properties_[idx_iter->second].second = value;
  }
}

JSONObject* JSONObject::setObject(const std::string& key) {
  auto elem = storage_->allocObject();
  set(key, elem);
  return elem;
}

JSONArray* JSONObject::setArray(const std::string& key) {
  auto elem = storage_->allocArray();
  set(key, elem);
  return elem;
}

void JSONObject::setString(const std::string& key, const std::string value) {
  set(key, storage_->allocString(value));
}

void JSONObject::setUInt64(const std::string& key, uint64_t value) {
  auto elem = storage_->allocNumber();
  elem->setUInt64(value);
  set(key, elem);
}

void JSONObject::setInt64(const std::string& key, int64_t value) {
  auto elem = storage_->allocNumber();
  elem->setInt64(value);
  set(key, elem);
}

void JSONObject::setDouble(const std::string& key, double value) {
  auto elem = storage_->allocNumber();
  elem->setDouble(value);
  set(key, elem);
}

void JSONObject::setBoolean(const std::string& key, bool value) {
  set(key, storage_->allocBoolean(value));
}

void JSONObject::setNull(const std::string& key) {
  set(key, storage_->allocNull());
}

std::string JSONObject::toString() const {
  std::string str;
  writeJSON(this, &str);
  return str;
}

JSONArray::JSONArray(JSONStorage* storage) : storage_(storage) {}

size_t JSONArray::size() const {
  return elements_.size();
}

JSONElement* JSONArray::get(size_t index) const {
  assert(index < elements_.size());
  return elements_[index];
}

JSONObject* JSONArray::getObject(size_t index) const {
  auto elem = get(index);
  if (elem && elem->isObject()) {
    return elem->getAsObject();
  } else {
    return nullptr;
  }
}

JSONArray* JSONArray::getArray(size_t index) const {
  auto elem = get(index);
  if (elem && elem->isArray()) {
    return elem->getAsArray();
  } else {
    return nullptr;
  }
}

std::string JSONArray::getString(
    size_t index,
    const std::string& fallback /* = "" */) const {
  auto elem = get(index);
  if (elem && elem->isString()) {
    return elem->getAsString()->getString();
  } else {
    return fallback;
  }
}

uint64_t JSONArray::getUInt64(
    size_t index,
    uint64_t fallback /* = 0 */) const {
  auto elem = get(index);
  if (elem && elem->isNumber()) {
    return elem->getAsNumber()->getUInt64();
  } else {
    return fallback;
  }
}

int64_t JSONArray::getInt64(
    size_t index,
    int64_t fallback /* = 0 */) const {
  auto elem = get(index);
  if (elem && elem->isNumber()) {
    return elem->getAsNumber()->getInt64();
  } else {
    return fallback;
  }
}

double JSONArray::getDouble(
    size_t index,
    double fallback /* = 0 */) const {
  auto elem = get(index);
  if (elem && elem->isNumber()) {
    return elem->getAsNumber()->getDouble();
  } else {
    return fallback;
  }
}

bool JSONArray::getBoolean(
    size_t index,
    bool fallback /* = false */) const {
  auto elem = get(index);
  if (elem && elem->isBoolean()) {
    return elem->getAsBoolean()->getBoolean();
  } else {
    return fallback;
  }
}

void JSONArray::append(JSONElement* value) {
  elements_.emplace_back(value);
}

JSONObject* JSONArray::appendObject() {
  auto elem = storage_->allocObject();
  elements_.emplace_back(elem);
  return elem;
}

JSONArray* JSONArray::appendArray() {
  auto elem = storage_->allocArray();
  elements_.emplace_back(elem);
  return elem;
}

void JSONArray::appendString(const std::string value) {
  elements_.emplace_back(storage_->allocString(value));
}

void JSONArray::appendUInt64(uint64_t value) {
  auto elem = storage_->allocNumber();
  elem->setUInt64(value);
  elements_.emplace_back(elem);
}

void JSONArray::appendInt64(int64_t value) {
  auto elem = storage_->allocNumber();
  elem->setInt64(value);
  elements_.emplace_back(elem);
}

void JSONArray::appendDouble(double value) {
  auto elem = storage_->allocNumber();
  elem->setDouble(value);
  elements_.emplace_back(elem);
}

void JSONArray::appendBoolean(bool value) {
  elements_.emplace_back(storage_->allocBoolean(value));
}

void JSONArray::appendNull() {
  elements_.emplace_back(storage_->allocNull());
}

std::string JSONArray::toString() const {
  std::string str;
  writeJSON(this, &str);
  return str;
}

JSONString::JSONString(const std::string& value) : value_(value) {}

const std::string& JSONString::getString() const {
  return value_;
}

void JSONString::setString(const std::string& value) {
  value_ = value;
}

std::string JSONString::toString() const {
  return value_;
}

JSONNumber JSONNumber::fromUInt64(uint64_t value) {
  return JSONNumber(std::to_string(value));
}

JSONNumber JSONNumber::fromInt64(int64_t value) {
  return JSONNumber(std::to_string(value));
}

JSONNumber JSONNumber::fromDouble(double value) {
  return JSONNumber(std::to_string(value));
}

JSONNumber::JSONNumber() {}

JSONNumber::JSONNumber(const std::string& value) : value_(value) {}

uint64_t JSONNumber::getUInt64() const {
  return std::stoull(value_);
}

int64_t JSONNumber::getInt64() const {
  return std::stoll(value_);
}

double JSONNumber::getDouble() const {
  return std::stod(value_);
}

void JSONNumber::setUInt64(uint64_t value) {
  value_ = std::to_string(value);
}

void JSONNumber::setInt64(int64_t value) {
  value_ = std::to_string(value);
}

void JSONNumber::setDouble(double value) {
  value_ = std::to_string(value);
}

std::string JSONNumber::toString() const {
  return value_;
}

JSONBoolean::JSONBoolean(bool value) : value_(value) {}

bool JSONBoolean::getBoolean() const {
  return value_;
}

void JSONBoolean::setBoolean(bool value) {
  value_ = value;
}

std::string JSONBoolean::toString() const {
  return value_ ? "true" : "false";
}

std::string JSONNull::toString() const {
  return "null";
}

} // namespace json
} // namespace libtransport

