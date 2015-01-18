/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_FEEDS_REMOTEFEEDWRITER_H
#define _FNORD_FEEDS_REMOTEFEEDWRITER_H
#include "fnord/base/stdtypes.h"
#include "fnord/base/option.h"
#include "fnord-rpc/RPC.h"
#include "fnord-rpc/RPCClient.h"
#include "fnord-feeds/FeedEntry.h"

namespace fnord {
namespace feeds {

class RemoteFeedWriter {
public:
  static const int kDefaultBatchSize = 1024;
  static const int kDefaultBufferSize = 8192;

  RemoteFeedWriter(
      RPCClient* rpc_client,
      int batch_size = kDefaultBatchSize,
      int buffer_size = kDefaultBufferSize);

  void appendEntry(const String& entry_data);

protected:
  fnord::RPCClient* rpc_client_;
  int batch_size_;
  int buffer_size_;
};

}
}
#endif
