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

template <typename ClassType>
template <typename ReturnType, typename... ArgTypes, typename... ArgNameTypes>
MethodCall<ClassType, ReturnType, ArgTypes...>
MetaClass<ClassType>::reflectMethod(
    ReturnType (ClassType::* method_fn)(ArgTypes...),
    ArgNameTypes... arg_names) {
  return MethodCall<ClassType, ReturnType, ArgTypes...>(
      method_fn,
      arg_names...);
}

}
}
