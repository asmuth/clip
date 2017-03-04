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
#include <fnordmetric/util/logging.h>
#include <fnordmetric/util/time.h>
#include <fnordmetric/statsd.h>
#include <fnordmetric/listen_udp.h>
#include <fnordmetric/sample.h>
#include <fnordmetric/aggregation_service.h>

namespace fnordmetric {

UDPIngestionTaskConfig::UDPIngestionTaskConfig() :
    bind("0.0.0.0"),
    port(8125),
    format(IngestionSampleFormat::STATSD) {}

ReturnCode UDPListener::start(
    AggregationService* aggregation_service,
    const IngestionTaskConfig* config,
    std::unique_ptr<IngestionTask>* task) {
  auto c = dynamic_cast<const UDPIngestionTaskConfig*>(config);
  if (!c) {
    return ReturnCode::error("ERUNTIME", "invalid ingestion task config");
  }

  if (c->port == 0) {
    return ReturnCode::error("ERUNTIME", "missing port");
  }

  auto self = new UDPListener(aggregation_service, c->format);
  task->reset(self);

  auto rc = self->listen(c->bind, c->port);
  if (!rc.isSuccess()) {
    return rc;
  }

  return ReturnCode::success();
}

UDPListener::UDPListener(
    AggregationService* aggr_service,
    IngestionSampleFormat format) :
    aggr_service_(aggr_service),
    format_(format),
    ssock_(-1) {}

UDPListener::~UDPListener() {
  if (ssock_ >= 0) {
    close(ssock_);
  }
}

ReturnCode UDPListener::listen(const std::string& bind_addr, int port) {
  logInfo("Starting UDP server on $0:$1", bind_addr, port);

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

ReturnCode UDPListener::start() {
  running_ = true;

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

    AggregationService::BatchInsertOptions opts;
    opts.format = format_;
    auto rc = aggr_service_->insertSamplesBatch(buf, buf_len, &opts);
    if (!rc.isSuccess()) {
      logWarning("error while inserting samples: $0", rc.getMessage());
    }
  }

  return ReturnCode::success();
}

void UDPListener::shutdown() {
  running_.store(false, std::memory_order_release);
  close(ssock_);
  ssock_ = -1;
}

} // namespace fnordmetric

