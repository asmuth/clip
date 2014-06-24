/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "eventloop.h"

namespace fnordmetric {
namespace ev {

EventLoop::EventLoop() : max_fd_(1), running_(true) {
  auto callbacks_size = sizeof(CallbackInterface*) * FD_SETSIZE;
  callbacks_ = static_cast<CallbackInterface**>(malloc(callbacks_size));
  assert(callbacks_ != nullptr);
  memset(callbacks_, 0, callbacks_size);

  FD_ZERO(&op_read_);
  FD_ZERO(&op_write_);
}

void EventLoop::watch(
    int fd,
    kInterestType interest,
    CallbackInterface* callback) {
  if (fd >= FD_SETSIZE) {
    fprintf(stderr,
        "fd is too big: %i, max is %i\n", fd, FD_SETSIZE - 1);
    return;
  }

  if (fd > max_fd_) {
    max_fd_ = fd;
  }

  callbacks_[fd] = callback;

  switch (interest) {
    case EV_READABLE:
      FD_SET(fd, &op_read_);
      break;
    case EV_WRITEABLE:
      FD_SET(fd, &op_write_);
      break;
  }
}

void EventLoop::unwatch(int fd, int flags) {
  callbacks_[fd] = nullptr;
  FD_CLR(fd, &op_read_);
  FD_CLR(fd, &op_write_);
}

int EventLoop::poll() {
  fd_set op_read, op_write;

  memcpy(&op_read, &op_read_, sizeof(fd_set));
  memcpy(&op_write, &op_write_, sizeof(fd_set));

  int res = select(max_fd_ + 1, &op_read, &op_write, NULL, NULL);

  if (res == 0) {
    return 0;
  }

  if (res == -1) {
    perror("error while selecting");
    return -1;
  }

  int num_events = 0;
  for (int fd = 0; fd <= max_fd_; fd++) {
    if (FD_ISSET(fd, &op_read)) {
      FD_CLR(fd, &op_read_);
      callbacks_[fd]->onEvent(this, fd, EV_READABLE);
      num_events++;
    }

    else if (FD_ISSET(fd, &op_write)) {
      FD_CLR(fd, &op_write_);
      callbacks_[fd]->onEvent(this, fd, EV_READABLE);
      num_events++;
    }
  }

  return num_events;
}

void EventLoop::loop() {
  while (running_) {
    poll();
  }
}


}
}
