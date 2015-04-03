/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_RPCCLIENT_H
#define _FNORD_RPCCLIENT_H
#include <functional>
#include <stdlib.h>
#include <string>
#include <unordered_map>
#include <vector>
#include "fnord-base/autoref.h"
#include "fnord-base/thread/taskscheduler.h"
#include "fnord-base/uri.h"
#include "fnord-rpc/RPC.h"
#include "fnord-http/httpconnectionpool.h"

namespace fnord {

class RPCClient {
public:
  virtual ~RPCClient() {}

  virtual void call(const URI& uri, RefPtr<AnyRPC> rpc) = 0;

};

class HTTPRPCClient : public RPCClient {
public:
  HTTPRPCClient(TaskScheduler* sched);
  void call(const URI& uri, RefPtr<AnyRPC> rpc) override;
protected:
  fnord::http::HTTPConnectionPool http_pool_;
};

} // namespace fnord
#endif
