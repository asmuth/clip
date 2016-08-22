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
#ifndef _libstx_HTTPHANDLER_H
#define _libstx_HTTPHANDLER_H
#include <memory>

namespace fnordmetric {
namespace http {
class HTTPServerConnection;
class HTTPRequest;

class HTTPHandler {
public:
  virtual ~HTTPHandler() {}
  virtual void handleHTTPRequest() = 0;
};

class HTTPHandlerFactory {
public:
  virtual ~HTTPHandlerFactory() {}
  virtual std::unique_ptr<HTTPHandler> getHandler(
      HTTPServerConnection* conn,
      HTTPRequest* req) = 0;
};

}
}
#endif
