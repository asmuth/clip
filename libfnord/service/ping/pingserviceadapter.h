/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_KEYVALLUE_SERVICE_ADAPTER_H
#define _FNORD_KEYVALLUE_SERVICE_ADAPTER_H
#include <stdlib.h>
#include "fnord/json/jsonrpc.h"

namespace fnord {
namespace ping_service {
class PingService;

class PingServiceAdapter {
public:

  static void registerJSONRPC(
      PingService* ping_service,
      json::JSONRPC* rpc);

};

} // namespace ping_service
} // namsepace fnord
#endif
