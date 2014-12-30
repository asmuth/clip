/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "fnord/net/http/httpchannel.h"

namespace fnord {
namespace http {

HTTPChannel::HTTPChannel(
    comm::LBGroup* lb_group,
    fnord::thread::TaskScheduler* scheduler) :
    conn_pool_(scheduler),
    lb_group_(lb_group) {}

std::unique_ptr<HTTPResponseFuture> HTTPChannel::executeRequest(
    const HTTPRequest& req) {
  return conn_pool_.executeRequest(req, lb_group_->getServerForNextRequest());
}

}
}
