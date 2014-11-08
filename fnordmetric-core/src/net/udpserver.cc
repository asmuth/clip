/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fcntl.h>
#include <fnordmetric/net/udpserver.h>
#include <fnordmetric/util/runtimeexception.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>

using fnord::util::Buffer;

namespace fnord {
namespace net {

UDPServer::UDPServer(
    thread::TaskScheduler* server_scheduler,
    thread::TaskScheduler* callback_scheduler) :
    server_scheduler_(server_scheduler),
    callback_scheduler_(callback_scheduler) {}

void UDPServer::onMessage(
    std::function<void (const Buffer&)> callback) {
  callback_ = callback; // FIXPAUL lock or doc
}

void UDPServer::listen(int port) {
  ssock_ = socket(AF_INET, SOCK_DGRAM, 0);
  if (ssock_ == 0) {
    RAISE(kIOError, "create socket() failed");
  }

  struct sockaddr_in addr;
  memset((char *) &addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port = htons(port);

  if (bind(ssock_, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
    RAISE_ERRNO(kIOError, "bind() failed");
  }

  int flags = fcntl(ssock_, F_GETFL, 0);
  flags = flags | O_NONBLOCK;

  if (fcntl(ssock_, F_SETFL, flags) != 0) {
    RAISE_ERRNO(kIOError, "fnctl(%i) failed", ssock_);
  }

  server_scheduler_->runOnReadable(
      thread::Task::create(std::bind(&UDPServer::messageReceived, this)),
      ssock_);
}

void UDPServer::messageReceived() {
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

  server_scheduler_->runOnReadable(
      thread::Task::create(std::bind(&UDPServer::messageReceived, this)),
      ssock_);

  if (buf_len < 0) {
    RAISE_ERRNO(kIOError, "read(%i) failed", ssock_);
  }

  if (callback_) {
    Buffer msg(buf, buf_len);
    callback_scheduler_->run(thread::Task::create([msg, this] () {
      this->callback_(msg);
    }));
  }
}

}
}

