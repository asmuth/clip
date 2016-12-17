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

static const uint64_t kResponseTimeout_us = 1000000;

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
    return ReturnCode::error("EIO", "gethostbyname($0) failed", hostname);
  }

  memcpy(&remote_addr_.sin_addr, h->h_addr, h->h_length);
  remote_addr_.sin_family = AF_INET;
  remote_addr_.sin_port = htons(std::stoul(m[2]));

  /* open local socket and put into nonblock */
  fd_ = socket(AF_INET, SOCK_DGRAM, 0);
  if (fd_ == -1) {
    return ReturnCode::error("EIO", "socket() failed: $0", strerror(errno));
  }

  if (fcntl(fd_, F_SETFL, fcntl(fd_, F_GETFL, 0) | O_NONBLOCK) != 0) {
    return ReturnCode::error("EIO", "fcntl() failed: $0", strerror(errno));
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

  if (rc < -1) {
    /* N.B. we set the socket to nonblocking so this might return EAGAIN.
       however we explicitly chose not to retry in this case */
    return ReturnCode::error("EIO", "sendto() failed: $0", strerror(errno));
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
      return ReturnCode::error("EIO", "poll() failed: $0", strerror(errno));
  }

  char resp_packet[0xFFFF];
  ssize_t resp_packet_len = recv(
      fd_,
      resp_packet,
      sizeof(resp_packet),
      0);

  if (resp_packet_len < 0) {
    return ReturnCode::error("EIO", "recv() failed: $0", strerror(errno));
  }

  return parseInfoResponsePacket(resp_packet, resp_packet_len, info);
}

} // namespace sensor_valve_srcds
} // namespace fnordmetric

