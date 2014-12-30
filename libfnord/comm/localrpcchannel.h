/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_COMM_LOCALRPCCHANNEL_H
#define _FNORD_COMM_LOCALRPCCHANNEL_H
#include <functional>
#include <stdlib.h>
#include <string>
#include <unordered_map>
#include <vector>
#include "fnord/comm/rpcchannel.h"
#include "fnord/thread/taskscheduler.h"

namespace fnord {
namespace comm {

class LocalRPCChannel : public RPCChannel {
public:

  template <typename ServiceType>
  static std::unique_ptr<LocalRPCChannel> forService(
      ServiceType* service,
      thread::TaskScheduler* scheduler);

  template <typename ServiceType>
  LocalRPCChannel(ServiceType* service, thread::TaskScheduler* scheduler);

  template <class RPCType>
  void call(RPCType* rpc);

protected:
  class ReflectionProxy {
  public:
    ReflectionProxy(LocalRPCChannel* base) : base_(base) {}

    template <typename MethodType>
    void method(MethodType* method);

    template <typename RPCCallType>
    void rpc(RPCCallType rpccall);

  protected:
    LocalRPCChannel* base_;
  };

  void* service_;
  thread::TaskScheduler* scheduler_;
  std::unordered_map<std::string, std::function<void (AnyRPC* rpc)>> methods_;
};

} // namespace comm
} // namsepace fnord

#include "localrpcchannel_impl.h"
#endif
