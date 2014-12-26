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

namespace fnord {
namespace comm {

template <typename ResultType, typename ArgPackType>
RPC<ResultType, ArgPackType>::RPC(const std::string& method) {}

template <typename ResultType, typename ArgPackType>
void RPC<ResultType, ArgPackType>::call(const ArgPackType& arguments) {
}

template <typename ResultType, typename ArgPackType>
void RPC<ResultType, ArgPackType>::wait() {
}

template <typename ResultType, typename ArgPackType>
const ResultType& RPC<ResultType, ArgPackType>::result() const {
  return result_;
}

template <class MethodCall>
RPC<typename MethodCall::ReturnType, typename MethodCall::ArgPackType> mkRPC(
    MethodCall method) {
  return RPC<
      typename MethodCall::ReturnType,
      typename MethodCall::ArgPackType>(method.name());
}

template <typename ClassType, typename ReturnType, typename... ArgTypes>
RPC<ReturnType, std::tuple<ArgTypes...>> mkRPC(
  ReturnType (ClassType::* method)(ArgTypes...)) {
  return mkRPC(fnord::reflect::reflectMethod(method));
}

} // namespace comm
} // namsepace fnord

