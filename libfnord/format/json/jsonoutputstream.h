/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_JSON_JSONOUTPUTSTREAM_H
#define _FNORD_JSON_JSONOUTPUTSTREAM_H
#include <set>
#include <vector>
#include <fnord/base/exception.h>
#include <fnord/io/outputstream.h>

namespace fnord {
namespace json {

class JSONOutputStream {
public:

  JSONOutputStream(std::shared_ptr<io::OutputStream> output_stream);
  JSONOutputStream(std::unique_ptr<io::OutputStream> output_stream);

  void beginObject();
  void endObject();
  void addObjectEntry(const std::string& key);
  void beginArray();
  void endArray();
  void addComma();
  void addString(const std::string& string);
  void addFloat(double value);
  void addInteger(int value);
  void addNull();
  void addTrue();
  void addFalse();

  template <typename T>
  void addValue(const std::set<T>& value);

  template <typename T>
  void addValue(const T& value);

protected:
  std::string escapeString(const std::string& string) const;
  std::shared_ptr<io::OutputStream> output_;
};

} // namespace json
} // namespace fnord

#include "jsonoutputstream_impl.h"
#endif
