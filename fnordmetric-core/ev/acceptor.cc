/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include "acceptor.h"
#include "eventloop.h"

namespace fnordmetric {
namespace ev {

Acceptor::Acceptor(EventLoop* ev_loop) : ev_loop_(ev_loop) {}

void Acceptor::listen(int port, std::function<void (int)> handler) {
  struct sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  server_addr.sin_port = htons(port);

  int ssock = socket(AF_INET, SOCK_STREAM, 0);
  if (ssock == -1) {
    fprintf(stderr, "create socket failed!\n");
    return;
  }

  int opt = 1;
  if (setsockopt(ssock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
    perror("setsockopt(SO_REUSEADDR)");
    return;
  }

#ifdef SOL_TCP
  if (setsockopt(ssock, SOL_TCP, TCP_QUICKACK, &opt, sizeof(opt)) < 0) {
    perror("setsockopt(TCP_QUICKACK)");
    return;
  }

  if (setsockopt(ssock, SOL_TCP, TCP_DEFER_ACCEPT, &opt, sizeof(opt)) < 0) {
    perror("setsockopt(TCP_DEFER_ACCEPT)");
    return;
  }
#endif

  if (bind(
      ssock,
      (struct sockaddr *)
      &server_addr, 
      sizeof(server_addr)) == -1) {
    perror("bind failed");
    return;
  }

  if (::listen(ssock, 1024) == -1) {
    perror("listen failed");
    return;
  }

  int flags = fcntl(ssock, F_GETFL, 0);
  flags = flags | O_NONBLOCK;

  if (fcntl(ssock, F_SETFL, flags) != 0) {
    fprintf(stderr, "fnctl() failed\n");
  }

  auto handler_ptr = new HandlerRef(ssock, handler);
  ev_loop_->watch(ssock, EventLoop::EV_READABLE, handler_ptr);
  handlers_.emplace_back(handler_ptr);
}

Acceptor::HandlerRef::HandlerRef(
    int ssock,
    std::function<void (int)> handler) :
    ssock_(ssock),
    handler_(handler) {}

void Acceptor::HandlerRef::onEvent(
    EventLoop* loop,
    int fd,
    EventLoop::kInterestType ev) {
  int conn_fd = accept(ssock_, NULL, NULL);

  if (conn_fd == -1) {
    perror("accept failed");
    return;
  }

  handler_(conn_fd);
  loop->watch(fd, EventLoop::EV_READABLE, this);
}

}
}
