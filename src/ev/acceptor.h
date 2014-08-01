/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */

#ifndef _FNORDMETRIC_EV_ACCEPTOR_H
#define _FNORDMETRIC_EV_ACCEPTOR_H
#include <memory>
#include <vector>
#include "eventloop.h"

namespace fnordmetric {
namespace ev {

class Acceptor {
public:
  class CallbackInterface {
  public:
    virtual void onConnection(int fd) = 0;
  };

  Acceptor(EventLoop* ev_loop);
  void listen(int port, CallbackInterface* handler);

protected:
  class HandlerRef : public EventLoop::CallbackInterface {
  public:
    HandlerRef(int ssock, Acceptor::CallbackInterface* handler);
    ~HandlerRef();
    void onEvent(EventLoop* loop, int fd, EventLoop::kInterestType ev) override;
  protected:
    int ssock_;
    Acceptor::CallbackInterface* handler_;
  };

  EventLoop* ev_loop_;
  std::vector<std::unique_ptr<HandlerRef>> handlers_;
};


}
}
#endif
