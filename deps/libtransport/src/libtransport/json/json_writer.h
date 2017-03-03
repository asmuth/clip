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

class JSONWriter {
public:

  JSONWriter(std::string* str);
  JSONWriter(WriteCallback write_cb);

  bool beginObject();
  bool endObject();
  bool beginArray();
  bool endArray();
  bool addString(const std::string& string);
  bool addFloat(double value);
  bool addInteger(int64_t value);
  bool addNull();
  bool addBool(bool val);
  bool addTrue();
  bool addFalse();

  bool addToken(
      kTokenType token_type,
      const std::string& token_data);

protected:

  bool addScalar(const std::string& str);
  bool write(const std::string& str);
  void advanceState();

  enum class WriterState {
    OBJECT_ENTRY_KEY,
    OBJECT_ENTRY_VALUE,
    OBJECT_NEXT,
    ARRAY,
    ARRAY_NEXT
  };

  std::stack<WriterState> stack_;
  WriteCallback write_cb_;
};

} // namespace json
} // namespace libtransport

