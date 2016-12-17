/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V. <paul@asmuth.com>
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#pragma once
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include "metricd/util/return_code.h"

namespace fnordmetric {
namespace sensor_valve_srcds {

struct SRCDSInfo {
  uint64_t num_players;
};

class SRCDSClient {
public:

  SRCDSClient();
  SRCDSClient(const SRCDSClient& o) = delete;
  SRCDSClient& operator=(const SRCDSClient& o) = delete;

  ReturnCode connect(const std::string& addr);
  ReturnCode getInfo(SRCDSInfo* info);

protected:
  int fd_;
  sockaddr_in remote_addr_;
  uint16_t remote_port_;
};

} // namespace sensor_valve_srcds
} // namespace fnordmetric

