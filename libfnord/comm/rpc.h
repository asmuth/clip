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
#include <stdlib.h>
#include <string>
#include <unordered_map>
#include <vector>

namespace fnord {
namespace comm {
class RPCChannel;

template <typename _ResultType, typename ArgPackType>
class RPC {
public:
  typedef _ResultType ResultType;

  RPC(const std::string& method, const ArgPackType& arguments);

  void wait();

  const ResultType& result() const;

protected:
  std::string method_;
  ArgPackType args_;
  ResultType result_;
};

template <class MethodCall>
RPC<typename MethodCall::ReturnType, typename MethodCall::ArgPackType> mkRPC(
    const MethodCall* method,
    typename MethodCall::ArgPackType args);

template <typename ClassType, typename ReturnType, typename... ArgTypes>
RPC<ReturnType, std::tuple<ArgTypes...>> mkRPC(
  ReturnType (ClassType::* method)(ArgTypes...),
  ArgTypes... args);

} // namespace comm
} // namsepace fnord


#include "rpc_impl.h"
#endif
