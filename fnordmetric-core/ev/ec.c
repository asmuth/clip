// This file is part of the "fyrehose" project
//   (c) 2011-2013 Paul Asmuth <paul@paulasmuth.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/select.h>

#include "ev.h"

ev_loop_t* ev_init(ev_loop_t* loop) {
  loop->setsize = FD_SETSIZE;
  loop->max_fd  = 1;

  loop->events = malloc(sizeof(ev_event_t) * loop->setsize);
  memset(loop->events, 0, sizeof(ev_event_t) * loop->setsize);

  loop->fired = malloc(sizeof(ev_event_t *) * loop->setsize);

  FD_ZERO(&loop->op_read);
  FD_ZERO(&loop->op_write);

  return loop;
}

void ev_free(ev_loop_t* loop) {
  if (loop->events) free(loop->events);
  if (loop->fired) free(loop->fired);
}

void ev_watch(ev_loop_t* loop, int fd, int flags, void* userdata) {
  if (fd >= loop->setsize) {
    printf("fd is too big: %i, max is %i\n", fd, loop->setsize - 1);
    return;
  }

  if (fd > loop->max_fd)
    loop->max_fd = fd;

  loop->events[fd].userdata = userdata;

  if (flags & EV_READABLE) FD_SET(fd, &loop->op_read);
  if (flags & EV_WRITEABLE) FD_SET(fd, &loop->op_write);
}

void ev_unwatch(ev_loop_t* loop, int fd) {
  loop->events[fd].fired = 0;
  loop->events[fd].userdata = NULL;
  FD_CLR(fd, &loop->op_read);
  FD_CLR(fd, &loop->op_write);
}

int ev_poll(ev_loop_t* loop) {
  fd_set op_read, op_write;
  int res, fd, num_events = 0;

  memcpy(&op_read, &loop->op_read, sizeof(fd_set));
  memcpy(&op_write, &loop->op_write, sizeof(fd_set));

  res = select(loop->max_fd + 1, &op_read, &op_write, NULL, NULL);

  if (res == 0) {
    printf("timeout while selecting\n");
    return -1;
  }

  if (res == -1) {
    perror("error while selecting");
    return -1;
  }

  for (fd = 0; fd <= loop->max_fd; fd++) {
    loop->events[fd].fired = 0;

    if (FD_ISSET(fd, &op_read)) {
      loop->events[fd].fired |= EV_READABLE;
      FD_CLR(fd, &loop->op_read);
    }

    if (FD_ISSET(fd, &op_write)) {
      loop->events[fd].fired |= EV_WRITEABLE;
      FD_CLR(fd, &loop->op_write);
    }

    if (loop->events[fd].fired) {
      loop->fired[num_events] = &loop->events[fd];
      num_events++;
    }
  }

  return num_events;
}
