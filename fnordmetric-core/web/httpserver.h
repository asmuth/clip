/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_WEB_HTTPSERVER_H
#define _FNORDMETRIC_WEB_HTTPSERVER_H
#include <memory>
#include <vector>
#include "../ev/acceptor.h"

namespace fnordmetric {
namespace web {

class ThreadedHTTPServer : public ev::Acceptor::CallbackInterface {
public:
  void onConnection(int fd) override;
protected:
};

}
}
#endif
