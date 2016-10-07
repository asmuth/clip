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
#ifndef _STX_HTTP_HTTPRESPONSESTREAM_H
#define _STX_HTTP_HTTPRESPONSESTREAM_H
#include <fnordmetric/util/stdtypes.h>
#include <fnordmetric/util/autoref.h>
#include <fnordmetric/transport/http/httpresponse.h>
#include <fnordmetric/transport/http/httpserverconnection.h>

namespace fnordmetric {
namespace http {

class HTTPResponseStream : public RefCounted {
public:
  static const size_t kMaxWriteBufferSize = 4 * 1024 * 1024;

  HTTPResponseStream(RefPtr<HTTPServerConnection> conn);

  /**
   * Write the provided http response (including headers and body) and then
   * immediately finish the response. After calling this method you must not
   * call any of startResponse, writeBodyChunk or finishResponse
   */
  void writeResponse(HTTPResponse res);

  /**
   * Start writing the HTTP response (i.e. write the headers). After calling
   * this method you may call writeBodyChunk zero or more times and then MUST
   * call finishResponse
   */
  void startResponse(const HTTPResponse& resp);

  /**
   * Write a HTTP response body chunk. Only legal if the HTTP response was 
   * previously started by calling "startResponse". You may call this method
   * zero or more times before calling finishResponse
   */
  void writeBodyChunk(const Buffer& buf);
  void writeBodyChunk(const void* data, size_t size);

  /**
   * Finish the http response. Must be called iff the HTTP response was started
   * by calling "startResponse"
   */
  void finishResponse();

  /**
   * Register a callback that will be called after all pending body chunks
   * have been written to the client.
   */
  void onBodyWritten(Function<void ()> callback);

  /**
   * Wait for the reader to consume the outstanding write buffer (i.e. block
   * until the write buffer size has fallen below a threshold.) This is an
   * advisory facility and it is not required to call this method correctness.
   * Should be used to apply "backpressure" to streaming writer to make sure
   * we don't fill up write buffers faster than the reader is reading.
   */
  void waitForReader();

  /**
   * Return the number of outstanding bytes in the write buffer
   */
  size_t bufferSize();

  /**
   * Returns true if the HTTP response was started (i.e. we started writing the
   * status line and some headers
   */
  bool isOutputStarted() const;

  /**
   * Returns true if the HTTP connection was closed
   */
  bool isClosed() const;

protected:

  void onCallbackCompleted();
  void onCallbackError();
  void onStateChanged(std::unique_lock<std::mutex>* lk);

  mutable std::mutex mutex_;
  mutable std::condition_variable cv_;
  RefPtr<HTTPServerConnection> conn_;
  bool callback_running_;
  bool headers_written_;
  bool response_finished_;
  Buffer buf_;
  Function<void ()> on_body_written_;
  bool error_;
};

}
}
#endif
