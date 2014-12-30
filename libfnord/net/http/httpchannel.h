/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_HTTPCHANNEL_H
#define _FNORD_HTTPCHANNEL_H
#include <map>
#include <vector>
#include <string>
#include "fnord/comm/lbgroup.h"
#include "fnord/net/http/httpconnectionpool.h"
#include "fnord/net/http/httprequest.h"
#include "fnord/net/http/httpresponsefuture.h"
#include "fnord/thread/taskscheduler.h"

namespace fnord {
namespace http {

class HTTPChannel {
public:
  HTTPChannel(
      comm::LBGroup* lb_group,
      fnord::thread::TaskScheduler* scheduler);

  std::unique_ptr<HTTPResponseFuture> executeRequest(const HTTPRequest& req);

protected:
  comm::LBGroup* lb_group_;
  HTTPConnectionPool conn_pool_;
};

}
}
#endif
