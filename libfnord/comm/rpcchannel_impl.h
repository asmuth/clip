/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "fnord/base/inspect.h"

namespace fnord {
namespace comm {

template <typename ServiceType>
LocalRPCChannel::LocalRPCChannel(
    ServiceType* service,
    thread::TaskScheduler* scheduler) :
    service_(service),
    scheduler_(scheduler) {
  ReflectionProxy proxy(this);
  fnord::reflect::MetaClass<ServiceType>::reflectMethods(&proxy);
}

template <typename MethodType>
void LocalRPCChannel::ReflectionProxy::method(MethodType* method) {
  auto service = base_->service_;
  auto runnable = [service, method] (AnyRPC* anyrpc) {
    auto rpc = dynamic_cast<
        RPC<
            typename MethodType::ReturnType,
            typename MethodType::ArgPackType>*>(anyrpc);

    if (rpc == nullptr) {
        RAISEF(
            kNoSuchMethodError,
            "invalid argument signature for method: $0",
            anyrpc->method());
    }

    rpc->ready(
        method->call((typename MethodType::ClassType*) service, rpc->args()));
  };

  auto sched = base_->scheduler_;
  base_->methods_.emplace(method->name(), [runnable, sched] (AnyRPC* anyrpc) {
    sched->run([runnable, anyrpc] () {
      runnable(anyrpc);
    });
  });
}

template <typename RPCCallType>
void LocalRPCChannel::ReflectionProxy::rpc(RPCCallType rpccall) {
  auto service = base_->service_;
  auto method = rpccall.method();
  auto runnable = [service, method] (AnyRPC* anyrpc) {
    auto rpc = dynamic_cast<
        RPC<
            typename RPCCallType::RPCReturnType,
            typename RPCCallType::RPCArgPackType>*>(anyrpc);

    if (rpc == nullptr) {
        RAISEF(
            kNoSuchMethodError,
            "invalid argument signature for method: $0",
            anyrpc->method());
    }

    method->call((typename RPCCallType::RPCServiceType*) service, rpc);
  };

  auto sched = base_->scheduler_;
  base_->methods_.emplace(method->name(), [runnable, sched] (AnyRPC* anyrpc) {
    sched->run([runnable, anyrpc] () {
      runnable(anyrpc);
    });
  });
}

template <class RPCType>
void LocalRPCChannel::call(RPCType* rpc) {
  const auto& method = methods_.find(rpc->method());

  if (method == methods_.end()) {
    RAISEF(kNoSuchMethodError, "no such method: $0", rpc->method());
  }

  method->second(rpc);
}

} // namespace comm
} // namsepace fnord

