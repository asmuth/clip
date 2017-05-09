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
#include <functional>
#include <memory>
#include <set>
#include <stack>
#include <vector>

namespace libtransport {
namespace json {

class JSONReader {
public:

  JSONReader(ReadCallback read_cb);
  JSONReader(const std::string* str);

  JSONReader(JSONReader&& other);
  JSONReader(const JSONReader& other) = delete;
  JSONReader& operator=(const JSONReader& other) = delete;

  bool readNextToken(
      kTokenType* token_type,
      std::string* token_data);

  const std::string& getError() const;

protected:
  void readNumber(std::string* dst);
  bool readString(std::string* dst);
  bool expectString(const std::string& expect);
  void advanceCursor();

  ReadCallback read_cb_;
  char cur_;
  std::string error_;
};


} // namespace json
} // namespace libtransport

