/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_EV_ACCEPTOR_H
#define _FNORDMETRIC_EV_ACCEPTOR_H
#include <functional>

namespace fnordmetric {
namespace ev {

class Acceptor {
public:
  Acceptor();

  void listen(int port);

  void setHandler(std::function<void (int)> handler);

protected:
  std::function<void (int)> handler_;
  volatile bool running_;
};


}
}
#endif
