/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnord/base/inspect.h>
#include <fnord/net/http/httpclientconnection.h>

namespace fnord {
namespace http {

HTTPClientConnection::HTTPClientConnection(
    std::unique_ptr<net::TCPConnection> conn,
    thread::TaskScheduler* scheduler) :
    conn_(std::move(conn)),
    scheduler_(scheduler),
    state_(S_CONN_INIT),
    parser_(HTTPParser::PARSE_HTTP_RESPONSE) {}

void HTTPClientConnection::executeRequest(const HTTPRequest& req) {
}

}
}
