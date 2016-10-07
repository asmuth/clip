/**
 * Copyright (c) 2016 zScale Technology GmbH <legal@zscale.io>
 * Authors:
 *   - Paul Asmuth <paul@zscale.io>
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Affero General Public License ("the license") as
 * published by the Free Software Foundation, either version 3 of the License,
 * or any later version.
 *
 * In accordance with Section 7(e) of the license, the licensing of the Program
 * under the license does not imply a trademark license. Therefore any rights,
 * title and interest in our trademarks remain entirely with us.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the license for more details.
 *
 * You can be released from the requirements of the license by purchasing a
 * commercial license. Buying such a license is mandatory as soon as you develop
 * commercial activities involving this program without disclosing the source
 * code of your own applications
 */
#ifndef _libstx_WEB_HTTPMESSAGE_H
#define _libstx_WEB_HTTPMESSAGE_H
#include <vector>
#include <string>
#include <utility>
#include <metricd/util/buffer.h>
#include <metricd/util/inputstream.h>
#include <metricd/util/outputstream.h>

namespace fnordmetric {
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
  void addBody(const void* data, size_t size);
  void appendBody(const void* data, size_t size);
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
