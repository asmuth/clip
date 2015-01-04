/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_COMM_RPC_H
#define _FNORD_COMM_RPC_H
#include <functional>
#include <memory>
#include <mutex>
#include <stdlib.h>
#include <string>
#include <unordered_map>
#include <vector>
#include "fnord/base/status.h"
#include "fnord/base/stdtypes.h"
#include "fnord/thread/future.h"
#include "fnord/thread/wakeup.h"

namespace fnord {
namespace comm {
class RPCChannel;

class AnyRPC : public RefCounted {
public:
  virtual ~AnyRPC();
  AnyRPC(const std::string& method);

  void wait();
  void onReady(Function<void()> callback);
  void raiseIfError() const;

  const std::string& method() const;

  void ready() noexcept;
  void error(const std::exception& e);
  void error(const Status& status);

protected:
  Status status_;
  String method_;
  bool is_ready_;
  bool autodelete_;
  std::mutex mutex_;
  Wakeup ready_wakeup_;
};

template <typename _ResultType, typename _ArgPackType>
class RPC : public AnyRPC {
public:
  typedef _ResultType ResultType;
  typedef _ArgPackType ArgPackType;

  RPC(const std::string& method, const ArgPackType& arguments);
  RPC(const RPC<ResultType, ArgPackType>& other);

  void call(RPCChannel* channel);
  void ready(const ResultType& result) noexcept;

  void onSuccess(Function<void(const RPC<ResultType, ArgPackType>& rpc)> fn);
  void onError(Function<void(const Status& status)> fn);

  const ArgPackType& args() const;
  const ResultType& result() const;

protected:

  ArgPackType args_;
  ResultType result_;
};

template <class ReturnType, typename... ArgTypes>
AutoRef<RPC<ReturnType, std::tuple<ArgTypes...>>> mkRPC(
    const std::string& method,
    ArgTypes... args);

template <class MethodCall>
AutoRef<
    RPC<
        typename MethodCall::ReturnType,
        typename MethodCall::ArgPackType>> mkRPC(
    const MethodCall* method,
    typename MethodCall::ArgPackType args);

template <typename ClassType, typename ReturnType, typename... ArgTypes>
AutoRef<RPC<ReturnType, std::tuple<ArgTypes...>>> mkRPC(
  ReturnType (ClassType::* method)(ArgTypes...),
  ArgTypes... args);

} // namespace comm
} // namsepace fnord


#include "rpc_impl.h"
#endif
