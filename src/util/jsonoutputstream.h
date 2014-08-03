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
#include <fnordmetric/util/runtimeexception.h>

namespace fnordmetric {
namespace util {

class JSONOutputStream {
public:

  JSONOutputStream(OutputStream* output_stream);

  void beginObject();
  void endObject();
  void beginObjectEntry(const std::string& key);
  void endObjectEntry(bool last);
  void addString(const std::string& string);

protected:
  util::OutputStream* output_;
};

}
}
#endif
