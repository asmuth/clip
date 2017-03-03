/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <limits.h>
#include <stdint.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <netinet/tcp.h>
#include <iostream>
#include "libtransport/http/v1/http_connection.h"
#include "libtransport/http/v1/http_parser.h"
#include "libtransport/http/v1/http_generator.h"

namespace libtransport {
namespace http {

HTTPConnection::HTTPConnection(
    int fd,
    uint64_t io_timeout,
    const std::string& prelude_bytes /* = "" */) :
    fd_(fd),
    io_timeout_(io_timeout),
    prelude_bytes_(prelude_bytes),
    closed_(false) {
  fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) | O_NONBLOCK);
  size_t nodelay = 1;
  setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &nodelay, sizeof(nodelay));
}

HTTPConnection::~HTTPConnection() {
  close();
}

bool HTTPConnection::sendRequest(const HTTPRequest& req) {
  if (!sendRequestHeaders(req)) {
    return false;
  }

  return sendRequestBodyChunk(
      (const char*) req.body().data(),
      req.body().size());
}

bool HTTPConnection::sendRequestHeaders(const HTTPRequest& req) {
  std::string write_buf;
  if (!HTTPGenerator::generateHeaders(req, &write_buf)) {
    return false;
  }

  return write((const char*) write_buf.data(), write_buf.size());
}

bool HTTPConnection::sendRequestBodyChunk(const char* data, size_t size) {
  return write(data, size);
}

bool HTTPConnection::recvRequest(HTTPRequest* request) {
  HTTPParser parser(HTTPParser::PARSE_HTTP_REQUEST);

  parser.onMethod([request] (HTTPMessage::kHTTPMethod method) {
    request->setMethod(method);
  });

  parser.onURI([request] (const char* data, size_t size) {
    request->setURI(std::string(data, size));
  });

  parser.onVersion([request] (const char* data, size_t size) {
    request->setVersion(std::string(data, size));
  });

  parser.onHeader([request] (
      const char* key,
      size_t key_size,
      const char* val,
      size_t val_size) {
    request->addHeader(
        std::string(key, key_size),
        std::string(val, val_size));
  });

  parser.onBodyChunk([request] (
      const char* data,
      size_t size) {
    request->appendBody(data, size);
  });


  if (!prelude_bytes_.empty()) {
    if (!parser.parse(prelude_bytes_.data(), prelude_bytes_.size())) {
      return false;
    }

    prelude_bytes_.clear();
  }

  std::string read_buf;
  read_buf.resize(4096);

  while (parser.state() != HTTPParser::S_DONE) {
    int read_rc = ::read(fd_, &read_buf[0], read_buf.size());
    switch (read_rc) {
      case 0:
        close();
        setError("connection unexpectedly closed");
        return false;
      case -1:
        if (errno == EAGAIN || errno == EINTR) {
          break;
        } else {
          close();
          setError(strerror(errno));
          return false;
        }
      default:
        if (!parser.parse(read_buf.data(), read_rc)) {
          return false;
        }
        break;
    }

    if (parser.state() == HTTPParser::S_DONE) {
      break;
    }

    struct pollfd p;
    p.fd = fd_;
    p.events = POLLIN;

    int poll_rc = poll(&p, 1, io_timeout_ / 1000);
    switch (poll_rc) {
      case 0:
        close();
        setError("operation timed out");
        return false;
      case -1:
        if (errno == EAGAIN || errno == EINTR) {
          break;
        } else {
          close();
          setError(strerror(errno));
          return false;
        }
    }
  }

  return true;
}

bool HTTPConnection::sendResponse(const HTTPResponse& res) {
  if (!sendResponseHeaders(res)) {
    return false;
  }

  return sendResponseBodyChunk(
      (const char*) res.body().data(),
      res.body().size());
}

bool HTTPConnection::sendResponseHeaders(const HTTPResponse& res) {
  std::string write_buf;
  if (!HTTPGenerator::generateHeaders(res, &write_buf)) {
    return false;
  }

  return write((const char*) write_buf.data(), write_buf.size());
}

bool HTTPConnection::sendResponseBodyChunk(
    const char* data,
    size_t size) {
  return write(data, size);
}

bool HTTPConnection::recvResponse(HTTPResponse* response) {
  HTTPParser parser(HTTPParser::PARSE_HTTP_RESPONSE);

  parser.onVersion([response] (const char* data, size_t size) {
    response->setVersion(std::string(data, size));
  });

  parser.onStatusCode([response] (int code) {
    response->setStatusCode(code);
  });

  parser.onStatusName([response] (const char* data, size_t size) {
    response->setStatusName(std::string(data, size));
  });

  parser.onHeader([response] (
      const char* key,
      size_t key_size,
      const char* val,
      size_t val_size) {
    response->addHeader(
        std::string(key, key_size),
        std::string(val, val_size));
  });

  parser.onBodyChunk([response] (
      const char* data,
      size_t size) {
    response->appendBody(data, size);
  });


  if (!prelude_bytes_.empty()) {
    if (!parser.parse(prelude_bytes_.data(), prelude_bytes_.size())) {
      return false;
    }

    prelude_bytes_.clear();
  }

  std::string read_buf;
  read_buf.resize(4096);

  while (parser.state() != HTTPParser::S_DONE) {
    int read_rc = ::read(fd_, &read_buf[0], read_buf.size());
    switch (read_rc) {
      case 0:
        if (!parser.eof()) {
          close();
          return false;
        }

        if (parser.state() == HTTPParser::S_DONE) {
          break;
        } else {
          close();
          setError("connection unexpectedly closed");
          return false;
        }

      case -1:
        if (errno == EAGAIN || errno == EINTR) {
          break;
        } else {
          close();
          setError(strerror(errno));
          return false;
        }

      default:
        if (!parser.parse(read_buf.data(), read_rc)) {
          return false;
        }
        break;

    }

    if (parser.state() == HTTPParser::S_DONE) {
      break;
    }

    struct pollfd p;
    p.fd = fd_;
    p.events = POLLIN;

    int poll_rc = poll(&p, 1, io_timeout_ / 1000);
    switch (poll_rc) {
      case 0:
        close();
        setError("operation timed out");
        return false;
      case -1:
        if (errno == EAGAIN || errno == EINTR) {
          break;
        } else {
          close();
          setError(strerror(errno));
          return false;
        }
    }
  }

  return true;
}

bool HTTPConnection::write(const char* data, size_t size) {
  size_t pos = 0;
  while (pos < size) {
    int write_rc = ::write(fd_, data + pos, size - pos);
    switch (write_rc) {
      case 0:
        close();
        setError("connection unexpectedly closed");
        return false;
      case -1:
        if (errno == EAGAIN || errno == EINTR) {
          break;
        } else {
          close();
          setError(strerror(errno));
          return false;
        }
      default:
        pos += write_rc;
        break;
    }

    if (pos == size) {
      break;
    }

    struct pollfd p;
    p.fd = fd_;
    p.events = POLLOUT;
    int poll_rc = poll(&p, 1, io_timeout_ / 1000);
    switch (poll_rc) {
      case 0:
        close();
        setError("operation timed out");
        return false;
      case -1:
        if (errno == EAGAIN || errno == EINTR) {
          break;
        } else {
          close();
          setError(strerror(errno));
          return false;
        }
    }
  }

  return true;
}

bool HTTPConnection::isClosed() const {
  return closed_;
}

void HTTPConnection::close() {
  if (closed_) {
    return;
  }

  ::close(fd_);
  closed_ = true;
}

std::string HTTPConnection::getError() const {
  return error_;
}

void HTTPConnection::setError(const std::string& error) {
  error_ = error;
}

} // namespace http
} // namespace libtransport

