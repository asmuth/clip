/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_JSON_JSONRPCCODEC_H
#define _FNORD_JSON_JSONRPCCODEC_H
#include <functional>
#include <stdlib.h>
#include <string>
#include <unordered_map>
#include <vector>
#include "fnord-base/buffer.h"
#include "fnord/json/json.h"

namespace fnord {
namespace json {

class JSONRPCCodec {
public:

  template <typename RPCType>
  static void encodeRPCRequest(RPCType* rpc, Buffer* buffer);

  template <typename RPCType>
  static void decodeRPCResponse(RPCType* rpc, const Buffer& buffer);

};

} // namespace json
} // namsepace fnord

#include "JSONRPCCodec_impl.h"
#endif
