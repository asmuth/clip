/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_STATS_STATSHTTPSERVLET_H
#define _FNORD_STATS_STATSHTTPSERVLET_H
#include "fnord/base/autoref.h"
#include "fnord/net/http/httpservice.h"
#include "fnord/stats/statsrepository.h"

namespace fnord {
namespace stats {

class StatsHTTPServlet : public fnord::http::HTTPService {
public:

  StatsHTTPServlet();
  StatsHTTPServlet(StatsRepository* stats_repo);

  void handleHTTPRequest(
      fnord::http::HTTPRequest* req,
      fnord::http::HTTPResponse* res);

protected:
  StatsRepository* stats_repo_;
};

}
}
#endif
