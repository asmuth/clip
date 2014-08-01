/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */

#ifndef FNORDMETRIC_EV_EVENTLOOP_H
#define FNORDMETRIC_EV_EVENTLOOP_H
#include <sys/select.h>

namespace fnordmetric {
namespace ev {

class EventLoop {
public:
  enum kInterestType {
    EV_READABLE = 1,
    EV_WRITEABLE = 2
  };

  class CallbackInterface {
  public:
    virtual void onEvent(
        EventLoop* loop,
        int fd,
        kInterestType interest) = 0;
  };

  EventLoop();
  void watch(int fd, kInterestType interest, CallbackInterface* callback);
  void unwatch(int fd, int flags);
  int poll();
  void loop();

protected:
  fd_set op_read_;
  fd_set op_write_;
  int max_fd_;
  CallbackInterface** callbacks_;
  volatile bool running_;
};

}
}
#endif
