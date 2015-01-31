/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "fnord-rpc/RPC.h"
#include "fnord-json/jsonrpcrequest.h"
#include "fnord-json/jsonrpcresponse.h"

namespace fnord {
namespace json {

template <class ServiceType>
void JSONRPC::registerService(ServiceType* service) {
  JSONRPC::ReflectionTarget<ServiceType> target(this, service);
  reflect::MetaClass<ServiceType>::reflectMethods(&target);
}

template <class MethodType>
void JSONRPC::registerMethod(
    const std::string& method_name,
    MethodType* method_call,
    typename MethodType::ClassType* service) {
  registerMethod(method_name, [method_call, service] (
      JSONRPCRequest* req,
      JSONRPCResponse* res) {
    res->successAndReturn(method_call->call(service, *req));
  });
}

template <class ClassType>
JSONRPC::ReflectionTarget<ClassType>::ReflectionTarget(
    JSONRPC* self,
    ClassType* service) :
    self_(self),
    service_(service) {}

template <typename ClassType>
template <typename MethodType>
void JSONRPC::ReflectionTarget<ClassType>::method(MethodType* method_call) {
  self_->registerMethod(
      method_call->name(),
      method_call,
      service_);
}

template <typename ClassType>
template <typename RPCCallType>
void JSONRPC::ReflectionTarget<ClassType>::rpc(RPCCallType rpc_call) {
  auto service = service_;
  auto mname = rpc_call.method()->name();

  self_->registerMethod(mname, [rpc_call, service] (
      JSONRPCRequest* req,
      JSONRPCResponse* res) {
      RPC<
        typename RPCCallType::RPCReturnType,
        typename RPCCallType::RPCArgPackType> rpc(
        rpc_call.method()->name(),
        rpc_call.getArgs(*req));

    /*
    rpc->onReady([rpc, res] () {
      res->successAndReturn(rpc->result());
      delete rpc;
    });
    */

    rpc_call.method()->call(service, &rpc);
    rpc.wait();
    res->successAndReturn(rpc.result());
  });
}

}
}
