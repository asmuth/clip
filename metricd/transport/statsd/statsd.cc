/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V. <paul@asmuth.com>
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <regex>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <metricd/util/logging.h>
#include <metricd/util/time.h>
#include <metricd/transport/statsd/statsd.h>
#include <metricd/sample.h>
#include <metricd/metric_service.h>

namespace fnordmetric {
namespace statsd {

StatsdEmitter::StatsdEmitter() : fd_(-1) {}

ReturnCode StatsdEmitter::connect(const std::string& addr) {
  std::regex addr_regex("([0-9a-zA-Z-_.]+):([0-9]+)");
  std::smatch m;
  if (!std::regex_match(addr, m, addr_regex)) {
    return ReturnCode::error("EARG", "invalid srcds_addr");
  }

  /* resolve hostname */
  auto hostname = m[1].str();
  struct hostent* h = gethostbyname(hostname.c_str());
  if (h == nullptr) {
    return ReturnCode::errorf(
        "EIO",
        "gethostbyname($0) failed",
        hostname);
  }

  memcpy(&remote_addr_.sin_addr, h->h_addr, h->h_length);
  remote_addr_.sin_family = AF_INET;
  remote_addr_.sin_port = htons(std::stoul(m[2]));

  /* open local socket and put into nonblock */
  fd_ = socket(AF_INET, SOCK_DGRAM, 0);
  if (fd_ == -1) {
    return ReturnCode::errorf("EIO", "socket() failed: $0", strerror(errno));
  }

  if (fcntl(fd_, F_SETFL, fcntl(fd_, F_GETFL, 0) | O_NONBLOCK) != 0) {
    return ReturnCode::errorf("EIO", "fcntl() failed: $0", strerror(errno));
  }

  return ReturnCode::success();
}

void StatsdEmitter::enqueueSample(
    const std::string& metric,
    const std::string& value,
    const std::map<std::string, std::string>& labels /* = {} */) {
  buf_ += metric;
  for (const auto& l : labels) {
    buf_ += "[";
    buf_ += l.first;
    buf_ += "=";
    buf_ += l.second;
    buf_ += "]";
  }
  buf_ += ":";
  buf_ += value;
  buf_ += "\n";
}

ReturnCode StatsdEmitter::emitSamples() {
  if (fd_ == -1) {
    return ReturnCode::error("EARG", "not connected");
  }

  if (buf_.empty()) {
    return ReturnCode::success();
  }

  // FIXME handle multi-packet buffers
  int rc = sendto(
      fd_,
      buf_.data(),
      buf_.size(),
      0,
      (const sockaddr*) &remote_addr_,
      sizeof(remote_addr_));

  buf_.clear();

  if (rc > 0) {
    return ReturnCode::success();
  } else {
    /* N.B. we set the socket to nonblocking so this might return EAGAIN.
       however we explicitly chose not to retry in this case */
    return ReturnCode::errorf("EIO", "sendto() failed: $0", strerror(errno));
  }
}

enum StatsdParseState {
  S_KEY,
  S_LABEL_OR_VALUE,
  S_LABEL,
  S_VALUE
};

bool parseStatsdSample(
    const char** ucur,
    const char* end,
    std::string* metric_id,
    std::string* series_id,
    std::string* value) {
  StatsdParseState state = S_KEY;
  const char* cur = *ucur;
  const char* mark = cur;

  for (; cur <= end; ++cur) {
    switch (state) {
      case S_KEY: {
        switch (*cur) {
          case '[':
          case ':':
          case 0:
            break;
          case '=':
            return false;
          default:
            continue;
        }

        *metric_id = std::string(mark, cur);
        state = *cur == '[' ? S_LABEL : S_VALUE;
        mark = cur + 1;
        break;
      }

      case S_LABEL_OR_VALUE: {
        switch (*cur) {
          case '[':
            state = S_LABEL;
            mark = cur + 1;
            continue;
          case ':':
            state = S_VALUE;
            mark = cur + 1;
            continue;
          default:
            return false;
        }
      }

      case S_LABEL: {
        switch (*cur) {
          case ']':
          case ':':
          case 0:
            break;
          default:
            continue;
        }

        *series_id = std::string(mark, cur - mark);
        state = S_LABEL_OR_VALUE;
        mark = cur + 1;
        break;
      }

      case S_VALUE: {
        char const* lend = mark;
        for (; lend < end && *lend != '\n' && *lend != '\r'; ++lend);
        *value = std::string(mark, lend);
        for (; lend < end && (*lend == '\n' || *lend == '\r'); ++lend);
        *ucur = lend;
        return true;
      }

    }
  }

  *ucur = end;
  return true;
}

} // namespace statsd
} // namespace fnordmetric

