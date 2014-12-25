/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
namespace fnord {
namespace reflect {

template <typename ClassType, typename ReturnType, typename... ArgTypes>
MethodCall<ClassType, ReturnType, ArgTypes...>::MethodCall(
    ReturnType (ClassType::* fn)(ArgTypes...)) :
    fn_(fn) {}

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
      args.template getArg<typename std::tuple_element<I, ArgPackType>::type>(
          I,
          "fnord")...)();
}

}
}
