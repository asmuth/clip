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
#include <metricd/transport/statsd/statsd_server.h>
#include <metricd/sample.h>
#include <metricd/metric_service.h>

namespace fnordmetric {
namespace statsd {

StatsdServer::StatsdServer(
    MetricService* metric_service) :
    ssock_(-1),
    metric_service_(metric_service) {}

StatsdServer::~StatsdServer() {
  if (ssock_ >= 0) {
    close(ssock_);
  }
}

ReturnCode StatsdServer::listen(const std::string& bind_addr, int port) {
  logInfo("Starting statsd server on $0:$1", bind_addr, port);

  ssock_ = socket(AF_INET, SOCK_DGRAM, 0);
  if (ssock_ == 0) {
    return ReturnCode::error("EIO", strerror(errno));
  }

  int opt = 1;
  if (setsockopt(ssock_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
    return ReturnCode::error("EIO", strerror(errno));
  }

  struct sockaddr_in addr;
  memset((char *) &addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port = htons(port);
  if (bind(ssock_, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
    close(ssock_);
    ssock_ = -1;
    return ReturnCode::error("EIO", strerror(errno));
  }

  return ReturnCode::success();
}

ReturnCode StatsdServer::listenAndStart(const std::string& addr, int port) {
  auto rc = listen(addr, port);
  if (!rc.isSuccess()) {
    return rc;
  }

  return start();
}

ReturnCode StatsdServer::start() {
  running_ = true;
  thread_ =  std::thread([this] {
    while (running_.load(std::memory_order_acquire)) {
      struct sockaddr_in other_addr;
      socklen_t other_addr_len = sizeof(other_addr);

      char buf[65535];
      auto buf_len = recvfrom(
          ssock_,
          buf,
          sizeof(buf),
          0,
          (struct sockaddr *) &other_addr,
          &other_addr_len);

      if (buf_len < 0) {
        logError("statsd receive failed: $0", strerror(errno));
        continue;
      }

      handlePacket(buf, buf_len);
    }
  });

  return ReturnCode::success();
}

void StatsdServer::shutdown() {
  running_.store(false, std::memory_order_release);
  close(ssock_);
  if (thread_.joinable()) {
    thread_.join();
  }
  ssock_ = -1;
}

void StatsdServer::handlePacket(const char* pkt, size_t pkt_len) {
  std::string key;
  std::string value;
  LabelSet labels;

  char const* cur = pkt;
  char const* end = pkt + pkt_len;

  while (cur < end) {
    if (!parseStatsdSample(&cur, end, &key, &value, &labels)) {
      logWarning("received invalid statsd packet");
      return;
    }

    logDebug(
        "received statsd sample; metric_id=$0 value=$1",
        key,
        value);

    double float_value;
    try {
      float_value = std::stod(value);
    } catch (std::exception& e) {
      continue;
    }

    auto now = WallClock::unixMicros();
    LabelledSample sample(
        Sample(now, float_value),
        labels);

    auto rc = metric_service_->insertSample(key, sample);
    if (!rc.isSuccess()) {
      logWarning(
          "statsd insert failed: $0 [$1=$2]",
          rc.getMessage(),
          key,
          value);
    }

    labels.clear();
  }
}

} // namespace statsd
} // namespace fnordmetric

