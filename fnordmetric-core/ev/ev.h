// This file is part of the "fyrehose" project
//   (c) 2011-2013 Paul Asmuth <paul@paulasmuth.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

#ifndef EV_H
#define EV_H

#define EV_READABLE 1
#define EV_WRITEABLE 2

typedef struct {
  int   watch;
  int   fired;
  void* userdata;
} ev_event_t;

typedef struct {
  fd_set       op_read;
  fd_set       op_write;
  ev_event_t*  events;
  ev_event_t** fired;
  int          max_fd;
  int          setsize;
} ev_loop_t;

ev_loop_t* ev_init(ev_loop_t* loop);
void ev_watch(ev_loop_t* loop, int fd, int flags, void* userdata);
void ev_unwatch(ev_loop_t* loop, int fd);
int  ev_poll(ev_loop_t* loop);
void ev_free(ev_loop_t* loop);

#endif
