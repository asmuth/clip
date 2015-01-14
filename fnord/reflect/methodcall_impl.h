/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <memory>
#include <string.h>
#include <type_traits>
#include "fnord/base/exception.h"
#include "fnord/base/stringutil.h"
#include "fnord/reflect/metaclass.h"

namespace fnord {
namespace reflect {

template <typename ClassType, typename ReturnType, typename... ArgTypes>
template <typename... ArgNameTypes>
MethodCall<ClassType, ReturnType, ArgTypes...>::MethodCall(
    const std::string& name,
    ReturnType (ClassType::* fn)(ArgTypes...),
    ArgNameTypes... arg_names) :
    name_(name),
    fn_(fn),
    arg_names_(StringUtil::toStringV(arg_names...)) {
  static_assert(
      (sizeof...(ArgTypes) == sizeof...(ArgNameTypes)),
      "invalid argument name list");
}

template <typename ClassType, typename ReturnType, typename... ArgTypes>
const std::string& MethodCall<ClassType, ReturnType, ArgTypes...>::name()
    const {
  return name_;
}

template <typename ClassType, typename ReturnType, typename... ArgTypes>
ReturnType MethodCall<ClassType, ReturnType, ArgTypes...>::call(
    ClassType* klass,
    ArgTypes... args) const {
  return std::bind(fn_, klass, args...)();
}

template <typename ClassType, typename ReturnType, typename... ArgTypes>
ReturnType MethodCall<ClassType, ReturnType, ArgTypes...>::call(
    ClassType* klass,
    const ArgPackType& args) const {
  return call(klass, args, typename MkIndexSequenceFor<ArgTypes...>::type());
}

template <typename ClassType, typename ReturnType, typename... ArgTypes>
template <int... I>
ReturnType MethodCall<ClassType, ReturnType, ArgTypes...>::call(
    ClassType* klass,
    const ArgPackType& args,
    IndexSequence<I...>) const {
  return std::bind(fn_, klass, std::get<I>(args)...)();
}

template <typename ClassType, typename ReturnType, typename... ArgTypes>
template <class ArgListType>
ReturnType MethodCall<ClassType, ReturnType, ArgTypes...>::call(
    ClassType* klass,
    const ArgListType& args) const {
  return call(klass, args, typename MkIndexSequenceFor<ArgTypes...>::type());
}

template <typename ClassType, typename ReturnType, typename... ArgTypes>
template <class ArgListType, int... I>
ReturnType MethodCall<ClassType, ReturnType, ArgTypes...>::call(
    ClassType* klass,
    const ArgListType& args,
    IndexSequence<I...>) const {
  return std::bind(
      fn_,
      klass,
      args.template getArg<
          typename std::decay<
              typename std::tuple_element<I, ArgPackType>::type>::type>(
                  I,
                  arg_names_[I])...)();
}

template <typename ClassType, typename ReturnType, typename... ArgTypes>
MethodCall<ClassType, ReturnType, ArgTypes...> reflectMethodImpl(
    ReturnType (ClassType::* method)(ArgTypes...)) {
  MethodCallLookup<ClassType, ReturnType, ArgTypes...> lookup(method);
  fnord::reflect::MetaClass<ClassType>::reflect(&lookup);
  return lookup.get();
}

template <typename ClassType, typename ReturnType, typename... ArgTypes>
MethodCallLookup<ClassType, ReturnType, ArgTypes...>::MethodCallLookup(
    ReturnType (ClassType::* subject)(ArgTypes...)) :
    subject_(subject) {}

template <typename ClassType, typename ReturnType, typename... ArgTypes>
template <typename T1, typename... ArgNameTypes>
void MethodCallLookup<ClassType, ReturnType, ArgTypes...>::tryMethod(
    T1 m1,
    T1 m2,
    const std::string& method_name,
    ArgNameTypes... arg_names) {
  const char* m1_raw = static_cast<const char*>(static_cast<const void*>(&m1));
  const char* m2_raw = static_cast<const char*>(static_cast<const void*>(&m2));
  if (sizeof(m1) != sizeof(m2)) {
    return;
  }

  if (memcmp(m1_raw, m2_raw, sizeof(m1) != 0)) {
    return;
  }

  method_call_.reset(
      new MethodCall<ClassType, ReturnType, ArgTypes...>(
          method_name,
          m1,
          arg_names...));
}

template <typename ClassType, typename ReturnType, typename... ArgTypes>
template <typename T1, typename T2, typename... ArgNameTypes>
void MethodCallLookup<ClassType, ReturnType, ArgTypes...>::tryMethod(
    T1 m1,
    T2 m2,
    const std::string& method_name,
    ArgNameTypes... arg_names) {
}

template <typename ClassType, typename ReturnType, typename... ArgTypes>
template <typename MethodType, typename... ArgNameTypes>
void MethodCallLookup<ClassType, ReturnType, ArgTypes...>::method(
    const std::string& method_name,
    MethodType method_call,
    ArgNameTypes... arg_names) {
  tryMethod(method_call, subject_, method_name, arg_names...);
}

template <typename ClassType, typename ReturnType, typename... ArgTypes>
template <typename MethodType, typename... ArgNameTypes>
void MethodCallLookup<ClassType, ReturnType, ArgTypes...>::rpc(
    const std::string& method_name,
    MethodType method_call,
    ArgNameTypes... arg_names) {
  tryMethod(method_call, subject_, method_name, "rpc");
}

template <typename ClassType, typename ReturnType, typename... ArgTypes>
MethodCall<ClassType, ReturnType, ArgTypes...>
MethodCallLookup<ClassType, ReturnType, ArgTypes...>::get() const {
  if (method_call_ == nullptr) {
    RAISE(kReflectionError, "reflection failed");
  }

  return *method_call_;
}

template <typename MethodType>
auto reflectMethod(MethodType method) -> decltype(reflectMethodImpl(method))
    const * {
  static std::recursive_mutex cache_lock;
  static std::vector<
      std::pair<
          MethodType,
          std::unique_ptr<decltype(reflectMethodImpl(method))>>> cache;

  std::lock_guard<std::recursive_mutex> l(cache_lock);

  for (const auto& pair : cache) {
    if (pair.first == method) {
      return pair.second.get();
    }
  }

  cache.emplace_back(std::make_pair(
      method,
      std::unique_ptr<decltype(reflectMethodImpl(method))>(
          new decltype(reflectMethodImpl(method))(reflectMethodImpl(method)))));

  return cache.back().second.get();
}

template <class ClassType>
template <class TargetType>
void MetaClass<ClassType>::reflectMethods(TargetType* target) {
  MethodCallProxy<TargetType> proxy(target);
  reflect(&proxy);
}

template <class TargetType>
MethodCallProxy<TargetType>::MethodCallProxy(
    TargetType* target) :
    target_(target) {}

template <class TargetType>
template <typename MethodType, typename... ArgNameTypes>
void MethodCallProxy<TargetType>::method(
      const std::string& method_name,
      MethodType method_call,
      ArgNameTypes... arg_names) {
  target_->method(reflectMethod(method_call));
}

template <class TargetType>
template <typename MethodType, typename... ArgNameTypes>
void MethodCallProxy<TargetType>::rpc(
      const std::string& method_name,
      MethodType method_call,
      ArgNameTypes... arg_names) {
  target_->rpc(RPCCall<
      decltype(reflectMethod(method_call))>(
          reflectMethod(method_call),
          arg_names...));
}

template <typename MethodCallType>
template <typename... ArgNameTypes>
RPCCall<MethodCallType>::RPCCall(
    MethodCallType method,
    ArgNameTypes... arg_names) :
    method_(method) {
  static_assert(
      std::tuple_size<RPCArgPackType>() == sizeof...(ArgNameTypes),
      "invalid argument name list");
  arg_names_ = StringUtil::toStringV(arg_names...);
}

template <typename MethodCallType>
MethodCallType RPCCall<MethodCallType>::method() const {
  return method_;
}

template <typename MethodCallType>
template <typename ArgListType>
typename RPCCall<MethodCallType>::RPCArgPackType
    RPCCall<MethodCallType>::getArgs(
        const ArgListType& args) const {
  return getArgs(
      args,
      typename IndexSequenceFor<
          std::tuple_size<RPCArgPackType>::value>::IndexSequenceType());
}

template <typename MethodCallType>
template <typename ArgListType, int... I>
typename RPCCall<MethodCallType>::RPCArgPackType
    RPCCall<MethodCallType>::getArgs(
        const ArgListType& args,
        IndexSequence<I...>) const {
  return std::make_tuple(
      args.template getArg<
          typename std::decay<
              typename std::tuple_element<I, RPCArgPackType>::type>::type>(
                  I,
                  arg_names_[I])...);
}

}
}
