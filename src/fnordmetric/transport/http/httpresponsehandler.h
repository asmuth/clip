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
#ifndef _libstx_HTTPRESPONSEHANDLER_H
#define _libstx_HTTPRESPONSEHANDLER_H
#include "fnordmetric/transport/http/wakeup.h"
#include <memory>

namespace fnordmetric {
namespace http {

class HTTPResponseHandler {
public:
  virtual ~HTTPResponseHandler() {}

  virtual void onError(const std::exception& e) = 0;

  virtual void onVersion(const std::string& version) = 0;
  virtual void onStatusCode(int status_code) = 0;
  virtual void onStatusName(const std::string& status) = 0;
  virtual void onHeader(const std::string& key, const std::string& value) = 0;
  virtual void onHeadersComplete() = 0;
  virtual void onBodyChunk(const char* data, size_t size) = 0;
  virtual void onResponseComplete() = 0;

};

}
}
#endif
