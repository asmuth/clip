/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V. <paul@asmuth.com>
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/fcntl.h>
#include <poll.h>
#include <string.h>
#include <regex>
#include <iostream>
#include "sensors/valve_srcds/srcds_client.h"

namespace fnordmetric {
namespace sensor_valve_srcds {

static const unsigned char kInfoRequestPacket[] = {
  0xff, 0xff, 0xff, 0xff, 0x54, 0x53, 0x6f, 0x75, 0x72, 0x63, 0x65, 0x20, 0x45,
  0x6e, 0x67, 0x69, 0x6e, 0x65, 0x20, 0x51, 0x75, 0x65, 0x72, 0x79, 0x00
};

static const unsigned char kInfoResponsePacketHeader[] = {
  0xff, 0xff, 0xff, 0xff, 0x49
};

static const uint64_t kResponseTimeout_us = 1000000;

SRCDSInfo::SRCDSInfo() :
    protocol_version(0),
    server_type(0),
    server_os(0),
    appid(0),
    player_count(0),
    player_count_max(0),
    player_count_bots(0),
    password_protected(false),
    vac_enabled(false) {}

void SRCDSInfo::toList(
    std::vector<std::pair<std::string, std::string>>* list) const {
  list->emplace_back("protocol_version", std::to_string(protocol_version));
  list->emplace_back("server_name", server_name);
  list->emplace_back("server_folder", server_folder);
  list->emplace_back("server_os", std::string(1, server_os));
  list->emplace_back("server_type", std::string(1, server_type));
  list->emplace_back("appid", std::to_string(appid));
  list->emplace_back("game", game);
  list->emplace_back("map", map);
  list->emplace_back("player_count", std::to_string(player_count));
  list->emplace_back("player_count_bots", std::to_string(player_count_bots));
  list->emplace_back("player_count_human", std::to_string(player_count - player_count_bots));
  list->emplace_back("player_count_max", std::to_string(player_count_max));
  list->emplace_back("password_protected", password_protected ? "true" : "false");
  list->emplace_back("vac_enbled", vac_enabled ? "true" : "false");
}

SRCDSClient::SRCDSClient() : fd_(-1) {}

ReturnCode SRCDSClient::connect(const std::string& addr) {
  std::regex addr_regex("([0-9a-zA-Z-_.]+):([0-9]+)");
  std::smatch m;
  if (!std::regex_match(addr, m, addr_regex)) {
    return ReturnCode::error("EARG", "invalid srcds_addr");
  }

  /* resolve hostname */
  auto hostname = m[1].str();
  struct hostent* h = gethostbyname(hostname.c_str());
  if (h == nullptr) {
    return ReturnCode::errorf("EIO", "gethostbyname($0) failed", hostname);
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

ReturnCode SRCDSClient::getInfo(SRCDSInfo* info) {
  if (fd_ == -1) {
    return ReturnCode::error("EARG", "not connected");
  }

  int rc = sendto(
      fd_,
      kInfoRequestPacket,
      sizeof(kInfoRequestPacket),
      0,
      (const sockaddr*) &remote_addr_,
      sizeof(remote_addr_));

  if (rc < 0) {
    /* N.B. we set the socket to nonblocking so this might return EAGAIN.
       however we explicitly chose not to retry in this case */
    return ReturnCode::errorf("EIO", "sendto() failed: $0", strerror(errno));
  }

  struct pollfd p;
  p.fd = fd_;
  p.events = POLLIN;
  int poll_rc = poll(&p, 1, kResponseTimeout_us / 1000);
  switch (poll_rc) {
    case 0:
      return ReturnCode::error("EIO", "SRCDS info operation timed out");
    case -1:
      /* N.B. we set the socket to nonblocking so this might return EAGAIN.
         however we explicitly chose not to retry in this case */
      return ReturnCode::errorf("EIO", "poll() failed: $0", strerror(errno));
  }

  char resp_packet[0xFFFF];
  ssize_t resp_packet_len = recv(
      fd_,
      resp_packet,
      sizeof(resp_packet),
      0);

  if (resp_packet_len < 0) {
    return ReturnCode::errorf("EIO", "recv() failed: $0", strerror(errno));
  }

  if (parseInfoResponsePacket(resp_packet, resp_packet_len, info)) {
    return ReturnCode::success();
  } else {
    return ReturnCode::error("EIO", "received invalid response packet");
  }
}

bool SRCDSClient::parseInfoResponsePacket(
    const char* pkt,
    size_t pkt_len,
    SRCDSInfo* info) const {
  auto pkt_cur = pkt;
  auto pkt_end = pkt + pkt_len;

  /* read header */
  if (pkt_len < sizeof(kInfoResponsePacketHeader)) {
    return false;
  }

  if (memcmp(
        pkt_cur,
        kInfoResponsePacketHeader,
        sizeof(kInfoResponsePacketHeader)) != 0) {
    return false;
  }

  pkt_cur += sizeof(kInfoResponsePacketHeader);

  /* read protocol version */
  if (pkt_cur >= pkt_end) {
    return false;
  }

  info->protocol_version = *((const uint8_t*) pkt_cur++);

  /* read server map */
  for (; pkt_cur < pkt_end && *pkt_cur != 0; ++pkt_cur) {
    info->server_name += *pkt_cur;
  }

  if (pkt_cur < pkt_end) {
    ++pkt_cur;
  } else {
    return false;
  }

  /* read server map */
  for (; pkt_cur < pkt_end && *pkt_cur != 0; ++pkt_cur) {
    info->map += *pkt_cur;
  }

  if (pkt_cur < pkt_end) {
    ++pkt_cur;
  } else {
    return false;
  }

  /* read server folder */
  for (; pkt_cur < pkt_end && *pkt_cur != 0; ++pkt_cur) {
    info->server_folder += *pkt_cur;
  }

  if (pkt_cur < pkt_end) {
    ++pkt_cur;
  } else {
    return false;
  }

  /* read server game */
  for (; pkt_cur < pkt_end && *pkt_cur != 0; ++pkt_cur) {
    info->game += *pkt_cur;
  }

  if (pkt_cur < pkt_end) {
    ++pkt_cur;
  } else {
    return false;
  }

  /* read app id */
  if (pkt_cur >= pkt_end - 2) {
    return false;
  }

  info->appid = *((const uint8_t*) pkt_cur++);
  info->appid += *((const uint8_t*) pkt_cur++) << 8;

  /* read players count */
  if (pkt_cur >= pkt_end) {
    return false;
  }

  info->player_count = *((const uint8_t*) pkt_cur++);

  /* read players max count */
  if (pkt_cur >= pkt_end) {
    return false;
  }

  info->player_count_max = *((const uint8_t*) pkt_cur++);

  /* read players bots count */
  if (pkt_cur >= pkt_end) {
    return false;
  }

  info->player_count_bots = *((const uint8_t*) pkt_cur++);

  /* read server type */
  if (pkt_cur >= pkt_end) {
    return false;
  }

  info->server_type = *pkt_cur++;

  /* read server os */
  if (pkt_cur >= pkt_end) {
    return false;
  }

  info->server_os = *pkt_cur++;

  /* read server password */
  if (pkt_cur >= pkt_end) {
    return false;
  }

  info->password_protected = (*pkt_cur++) == 0;

  /* read server vac */
  if (pkt_cur >= pkt_end) {
    return false;
  }

  info->vac_enabled = (*pkt_cur++) > 0;

  return true;
}

} // namespace sensor_valve_srcds
} // namespace fnordmetric

