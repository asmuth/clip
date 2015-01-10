/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_HTTPSTATS_H
#define _FNORDMETRIC_HTTPSTATS_H

#include "fnord/stats/counter.h"

namespace fnord {
namespace http {

struct HTTPClientStats {
  stats::Counter<uint64_t> open_connections;
  stats::Counter<uint64_t, uint64_t> status_codes;
  stats::Counter<uint64_t> current_requests;
  stats::Counter<uint64_t> total_requests;
  stats::Counter<uint64_t> sent_bytes;
  stats::Counter<uint64_t> received_bytes;
  stats::Counter<uint64_t> total_bytes;

  HTTPClientStats() :
      status_codes("http_status") {}
};

/*
struct HTTPServerStats {
  stats::Counter<uint64_t> 
  stats::Counter<uint64_t, uint64_t> http_status_codes;
};
*/

}
}
#endif
