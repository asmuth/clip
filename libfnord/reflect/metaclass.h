/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_REFLECT_METACLASS_H
#define _FNORD_REFLECT_METACLASS_H
#include <unordered_map>

namespace fnord {
namespace reflect {

template <class ClassType>
class MetaClass {
public:
  template <class TargetType>
  static void reflectMethods(TargetType* target);

  template <class TargetType>
  static void reflectProperties(TargetType* target);

  template <class TargetType>
  static void reflect(TargetType* target);

  template <class TargetType>
  static void serialize(const ClassType& instance, TargetType* target);

  template <class TargetType>
  static ClassType unserialize(TargetType* target);

};

template <typename TargetType>
struct DummyTarget {};

template<typename T, typename X = void>
struct is_reflected : std::false_type {};

template<typename T>
struct is_reflected<
    T,
    decltype(T::reflect((fnord::reflect::DummyTarget<T>*) nullptr))> :
        std::true_type {};

template <class ClassType>
template <class TargetType>
void MetaClass<ClassType>::reflect(TargetType* target) {
  ClassType::reflect(target);
}


}
}

#endif
