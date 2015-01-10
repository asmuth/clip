/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_WEB_HTTPMESSAGE_H
#define _FNORDMETRIC_WEB_HTTPMESSAGE_H
#include <vector>
#include <string>
#include <utility>
#include <fnord/base/buffer.h>
#include <fnord/base/io/inputstream.h>
#include <fnord/base/io/outputstream.h>

namespace fnord {
namespace http {

class HTTPMessage {
public:
  typedef std::vector<std::pair<std::string, std::string>> HeaderList;

  enum kHTTPMethod {
    M_CONNECT,
    M_DELETE,
    M_GET,
    M_HEAD,
    M_OPTIONS,
    M_POST,
    M_PUT,
    M_TRACE
  };

  HTTPMessage() {}
  virtual ~HTTPMessage() {}

  const std::string& version() const;
  void setVersion(const std::string& version);

  const HeaderList& headers() const;
  const std::string& getHeader(const std::string& key) const;
  bool hasHeader(const std::string& key) const;
  void addHeader(const std::string& key, const std::string& value);
  void setHeader(const std::string& key, const std::string& value);
  void clearHeaders();

  const Buffer& body() const;
  void addBody(const std::string& body);
  void addBody(const Buffer& buf);
  void addBody(void* data, size_t size);
  void appendBody(void* data, size_t size);
  void clearBody();

  std::unique_ptr<InputStream> getBodyInputStream() const;
  std::unique_ptr<OutputStream> getBodyOutputStream();

protected:
  std::string version_;
  static std::string kEmptyHeader;
  std::vector<std::pair<std::string, std::string>> headers_;
  Buffer body_;
};

}
}
#endif
