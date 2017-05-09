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
  SRCDSInfo();
  void toList(std::vector<std::pair<std::string, std::string>>* list) const;

  uint8_t protocol_version;
  std::string server_name;
  std::string server_folder;
  char server_type;
  char server_os;
  std::string map;
  std::string game;
  uint16_t appid;
  uint8_t player_count;
  uint8_t player_count_max;
  uint8_t player_count_bots;
  bool password_protected;
  bool vac_enabled;
};

class SRCDSClient {
public:

  SRCDSClient();
  SRCDSClient(const SRCDSClient& o) = delete;
  SRCDSClient& operator=(const SRCDSClient& o) = delete;

  ReturnCode connect(const std::string& addr);
  ReturnCode getInfo(SRCDSInfo* info);

protected:

  bool parseInfoResponsePacket(
      const char* pkt,
      size_t pkt_len,
      SRCDSInfo* info) const;

  int fd_;
  sockaddr_in remote_addr_;
};

} // namespace sensor_valve_srcds
} // namespace fnordmetric

