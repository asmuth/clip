/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_EV_ACCEPTOR_H
#define _FNORDMETRIC_EV_ACCEPTOR_H
#include <stdlib.h>

namespace fnordmetric {
namespace ev {

class Acceptor {
public:
  Acceptor();

  void listen(int port);

protected:
  volatile bool running_;
};


}
}
#endif
