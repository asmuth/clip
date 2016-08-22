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

#ifndef _STX_HTTP_HTTPSSESTREAM_H
#define _STX_HTTP_HTTPSSESTREAM_H
#include "fnordmetric/transport/http/httpservice.h"
#include "fnordmetric/util/json/json.h"
#include <fnordmetric/util/inspect.h>

namespace fnordmetric {
namespace http {

class HTTPSSEStream : public RefCounted {
public:

  /**
   * Initialize the response from the provided request stream, write to the
   * provided response stream
   */
  HTTPSSEStream(
      RefPtr<http::HTTPRequestStream> req_stream,
      RefPtr<http::HTTPResponseStream> res_stream);

  /**
   * Initialize the response from the provided request, write to the provided
   * response stream
   */
  HTTPSSEStream(
      const http::HTTPRequest* req,
      RefPtr<http::HTTPResponseStream> res_stream);

  /**
   * Initialize the response from the provided response, write to the provided
   * response stream
   */
  HTTPSSEStream(
      const http::HTTPResponse* res,
      RefPtr<http::HTTPResponseStream> res_stream);

  void start();

  void sendEvent(
    const String& data,
    const Option<String>& event_type);

  void sendEvent(
    const Buffer& data,
    const Option<String>& event_type);

  void sendEvent(
    const void* event_data,
    size_t event_size,
    const Option<String>& event_type);

  const HTTPResponse response() const;
  void finish();

  /**
   * Returns true if the HTTP connection was closed
   */
  bool isClosed() const;

private:
  RefPtr<http::HTTPResponseStream> res_stream_;
  HTTPResponse res_;
};

}
}
#endif
