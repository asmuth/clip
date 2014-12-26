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

class AnyRPC {
public:
  virtual ~AnyRPC() {}
  AnyRPC(const std::string& method) : method_(method) {}
  const std::string& method() const { return method_; };
protected:
  std::string method_;
};

template <typename _ResultType, typename ArgPackType>
class RPC : public AnyRPC {
public:
  typedef _ResultType ResultType;

  RPC(const std::string& method, const ArgPackType& arguments);

  void call(RPCChannel* channel);
  void ready(const ResultType& result);
  void wait();

  const ArgPackType& args() const;
  const ResultType& result() const;

protected:
  ArgPackType args_;
  ResultType result_;
};

template <class ReturnType, typename... ArgTypes>
RPC<ReturnType, std::tuple<ArgTypes...>> mkRPC(
    const std::string& method,
    ArgTypes... args);

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
