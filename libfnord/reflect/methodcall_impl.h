/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <type_traits>
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
  fnord::reflect::MetaClass<ClassType>::reflectMethods(&lookup);
  return lookup.get();
}

template <typename ClassType, typename ReturnType, typename... ArgTypes>
MethodCallLookup<ClassType, ReturnType, ArgTypes...>::MethodCallLookup(
    ReturnType (ClassType::* subject)(ArgTypes...)) :
    subject_(subject) {}

template <typename ClassType, typename ReturnType, typename... ArgTypes>
template <typename MethodType, typename... ArgNameTypes>
void MethodCallLookup<ClassType, ReturnType, ArgTypes...>::method(
    const std::string& method_name,
    MethodType method_call,
    ArgNameTypes... arg_names) {
  if (method_call == subject_) {
    method_call_.reset(
        new MethodCall<ClassType, ReturnType, ArgTypes...>(
            method_name,
            method_call,
            arg_names...));
  }
}

template <typename ClassType, typename ReturnType, typename... ArgTypes>
MethodCall<ClassType, ReturnType, ArgTypes...>
MethodCallLookup<ClassType, ReturnType, ArgTypes...>::get() const {
  return *method_call_;
}

template <typename MethodType>
auto reflectMethod(MethodType method) -> decltype(reflectMethodImpl(method))
    const* {
  static const auto method_call = reflectMethodImpl(method);
  return &method_call;
}

}
}
