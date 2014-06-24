/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_EV_ACCEPTOR_H
#define _FNORDMETRIC_EV_ACCEPTOR_H
#include <functional>
#include <memory>
#include <vector>
#include "eventloop.h"

namespace fnordmetric {
namespace ev {

class Acceptor {
public:
  Acceptor(EventLoop* ev_loop);
  void listen(int port, std::function<void (int)> handler);

protected:
  class HandlerRef : public EventLoop::CallbackInterface {
  public:
    HandlerRef(int ssock, std::function<void (int)> handler);
    void onEvent(EventLoop* loop, int fd, EventLoop::kInterestType ev) override;
  protected:
    int ssock_;
    std::function<void (int)> handler_;
  };

  EventLoop* ev_loop_;
  std::vector<std::unique_ptr<HandlerRef>> handlers_;
};


}
}
#endif
