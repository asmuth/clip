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

namespace fnordmetric {
namespace ev {

Acceptor::Acceptor() : running_(true) {}

void Acceptor::listen(int port) {
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

  while (running_) {
    int fd = accept(ssock, NULL, NULL);

    if (!running_) {
      close(fd);
      break;
    }

    if (fd == -1) {
      perror("accept failed");
      continue;
    }

  }
}

}
}
