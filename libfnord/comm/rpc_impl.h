/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "fnord/reflect/reflect.h"
#include "fnord/comm/localrpcchannel.h"
#include "fnord/comm/rpcchannel.h"
#include "fnord/json/jsonrpchttpchannel.h"

namespace fnord {
namespace comm {

template <typename ResultType, typename ArgPackType>
RPC<ResultType, ArgPackType>::RPC(
  const std::string& method,
  const ArgPackType& args) :
  AnyRPC(method),
  args_(args) {}

template <typename ResultType, typename ArgPackType>
void RPC<ResultType, ArgPackType>::call(RPCChannel* chan) {
  auto local_chan = dynamic_cast<LocalRPCChannel*>(chan);
  if (local_chan) {
    return local_chan->call(this);
  }

  auto jsonrpchttp_chan = dynamic_cast<json::JSONRPCHTTPChannel*>(chan);
  if (jsonrpchttp_chan) {
    return jsonrpchttp_chan->call(this);
  }

  RAISE(kRPCError, "invalid RPC channel");
}

template <typename ResultType, typename ArgPackType>
void RPC<ResultType, ArgPackType>::ready(const ResultType& result) {
  result_ = result;
  AnyRPC::ready();
}

template <typename ResultType, typename ArgPackType>
const ResultType& RPC<ResultType, ArgPackType>::result() const {
  return result_;
}

template <typename ResultType, typename ArgPackType>
const ArgPackType&  RPC<ResultType, ArgPackType>::args() const {
  return args_;
}

template <class ReturnType, typename... ArgTypes>
RPC<ReturnType, std::tuple<ArgTypes...>> mkRPC(
    const std::string& method,
    ArgTypes... args) {
  return RPC<ReturnType, std::tuple<ArgTypes...>>(
      method,
      std::make_tuple(args...));
}

template <class MethodCall>
RPC<typename MethodCall::ReturnType, typename MethodCall::ArgPackType> mkRPC(
    const MethodCall* method,
    typename MethodCall::ArgPackType args) {
  return RPC<
      typename MethodCall::ReturnType,
      typename MethodCall::ArgPackType>(method->name(), args);
}

template <typename ClassType, typename ReturnType, typename... ArgTypes>
RPC<ReturnType, std::tuple<ArgTypes...>> mkRPC(
  ReturnType (ClassType::* method)(ArgTypes...),
  ArgTypes... args) {
  return mkRPC(
      fnord::reflect::reflectMethod(method),
      std::tuple<typename std::decay<ArgTypes>::type...>(args...));
}

} // namespace comm
} // namsepace fnord

