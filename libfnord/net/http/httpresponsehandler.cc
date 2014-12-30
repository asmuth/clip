/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "fnord/base/inspect.h"
#include "fnord/net/http/httpresponse.h"
#include "fnord/net/http/httpresponsehandler.h"

namespace fnord {
namespace http {

DefaultHTTPResponseHandler::DefaultHTTPResponseHandler(
    fnord::http::HTTPResponse* response,
    fnord::thread::Wakeup* wakeup) :
    response_(response),
    wakeup_(wakeup) {}

void DefaultHTTPResponseHandler::onError(const std::exception& e) {

}

void DefaultHTTPResponseHandler::onVersion(const std::string& version) {
  response_->setVersion(version);
}

void DefaultHTTPResponseHandler::onStatusCode(int status_code) {
  response_->setStatusCode(status_code);
}

void DefaultHTTPResponseHandler::onStatusName(const std::string& status) {
  response_->setStatusName(status);
}

void DefaultHTTPResponseHandler::onHeader(
    const std::string& key,
    const std::string& value) {
  response_->addHeader(key, value);
}

void DefaultHTTPResponseHandler::onHeadersComplete() {}

void DefaultHTTPResponseHandler::onBodyChunk(const char* data, size_t size) {
  response_->appendBody((char *) data, size);
}

void DefaultHTTPResponseHandler::onResponseComplete() {
  wakeup_->wakeup();
}

}
}
