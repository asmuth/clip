/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef __FNORDMETRIC_HTTPINPUTSTREAM_H
#define __FNORDMETRIC_HTTPINPUTSTREAM_H
#include <vector>
#include <string>
#include <utility>
#include <fnord/io/inputstream.h>

using fnord::io::InputStream;

namespace fnord {
namespace http {

class HTTPInputStream {
public:
  enum kParserState {
    HTTP_STATE_METHOD,
    HTTP_STATE_URI,
    HTTP_STATE_VERSION,
    HTTP_STATE_HKEY,
    HTTP_STATE_HVAL,
    HTTP_STATE_BODY
  };

  /**
   * @param input_stream the input stream -- does not transfer ownership
   */
  HTTPInputStream(InputStream* input_stream);
  ~HTTPInputStream();

  /**
   * Read the http status line. Throws a fnord::Exception for invalid requests.
   * Might throw a io::WouldBlockException if the InputStream is non blocking.
   */
  void readStatusLine(
      std::string* method,
      std::string* url,
      std::string* version);

  /**
   * Read the http headers. Throws a fnord::Exception for invalid requests.
   * Might throw a io::WouldBlockException if the InputStream is non blocking
   */
  void readHeaders(
      std::vector<std::pair<std::string, std::string>>* target);

  InputStream* getInputStream() const;

protected:
  void readNextByte(std::string* target);
  InputStream* input_;
  kParserState state_;
};

}
}
#endif
