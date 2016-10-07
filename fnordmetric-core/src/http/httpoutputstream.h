/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef __FNORDMETRIC_HTTPOUTPUTSTREAM_H
#define __FNORDMETRIC_HTTPOUTPUTSTREAM_H
#include <vector>
#include <string>
#include <utility>
#include <metricd/util/outputstream.h>

using fnordmetric::util::OutputStream;

namespace fnord {
namespace http {

class HTTPOutputStream {
public:

  /**
   * @param output_stream the output stream -- does not transfer ownership
   */
  HTTPOutputStream(OutputStream* output_stream);

  void writeStatusLine(
      const std::string& version,
      int status_code,
      const std::string& status);

  void writeHeaders(
      const std::vector<std::pair<std::string, std::string>>& headers);

  OutputStream* getOutputStream() const;

protected:
  OutputStream* output_;
};

}
}
#endif
