/**
 * Copyright (c) 2016 zScale Technology GmbH <legal@zscale.io>
 * Authors:
 *   - Paul Asmuth <paul@zscale.io>
 *   - Laura Schlimmer <laura@zscale.io>
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
#ifndef _STX_HTTP_HTTPREQUESTSTREAM_H
#define _STX_HTTP_HTTPREQUESTSTREAM_H
#include <metricd/util/stdtypes.h>
#include <metricd/util/autoref.h>
#include <metricd/transport/http/httpresponse.h>
#include <metricd/transport/http/httpserverconnection.h>

namespace fnordmetric {
namespace http {

class HTTPRequestStream : public RefCounted {
public:

  HTTPRequestStream(const HTTPRequest& req, RefPtr<HTTPServerConnection> conn);

  /**
   * Retrieve the http request. The http request will not contain a request body
   * by default. Instead you must call one of the readBody(...) method to
   * retrieve the request body
   */
  const HTTPRequest& request() const;

  /**
   * Read the http request body in chunks. This method will call the provided
   * callback for each chunk and return once all body chunks have been read
   */
  void readBody(Function<void (const void* data, size_t size)> fn);

  /**
   * Read the http request body into the contained HTTPRequest
   */
  void readBody();

  /**
   * Discard the http request body
   */
  void discardBody();

protected:
  HTTPRequest req_;
  RefPtr<HTTPServerConnection> conn_;
};

}
}
#endif
