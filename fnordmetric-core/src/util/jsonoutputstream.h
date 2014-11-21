/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_UTIL_JSONOUTPUTSTREAM_H
#define _FNORDMETRIC_UTIL_JSONOUTPUTSTREAM_H
#include <fnordmetric/util/outputstream.h>
#include <fnord/base/exception.h>

namespace fnordmetric {
namespace util {

class JSONOutputStream {
public:

  JSONOutputStream(std::shared_ptr<OutputStream> output_stream);

  void beginObject();
  void endObject();
  void addObjectEntry(const std::string& key);
  void beginArray();
  void endArray();
  void addComma();
  void addString(const std::string& string);
  void addFloat(double value);

  template <typename T>
  void addLiteral(T integer) {
    output_->write(std::to_string(integer));
  }

protected:
  std::string escapeString(const std::string& string) const;
  std::shared_ptr<util::OutputStream> output_;
};

}
}
#endif
