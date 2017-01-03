/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V.
 *   Copyright (c) 2016 Laura Schlimmer, FnordCorp B.V.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _STX_HTTP_HTTPSSESTREAM_H
#define _STX_HTTP_HTTPSSESTREAM_H
#include "metricd/transport/http/httpservice.h"
#include <metricd/util/inspect.h>
#include <metricd/util/option.h>

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
