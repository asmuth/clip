/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_REFLECT_METHODCALL_H
#define _FNORD_REFLECT_METHODCALL_H
#include <vector>
#include <string>
#include <functional>
#include "fnord/reflect/indexsequence.h"

namespace fnord {
namespace reflect {

class AnyMethodCall {
public:
  AnyMethodCall(const std::string& name);
  virtual ~AnyMethodCall() {}
protected:
  std::string name_;
};

template <typename ClassType, typename ReturnType, typename... ArgTypes>
class MethodCall : public AnyMethodCall {
public:
  typedef std::tuple<ArgTypes...> ArgPackType;

  MethodCall(
      ReturnType (ClassType::* fn)(ArgTypes...),
      const std::string& name);

  ReturnType call(ClassType* klass, ArgTypes... args) const;
  ReturnType call(ClassType* klass, const ArgPackType& args) const;

protected:

  template <int... I>
  ReturnType call(
      ClassType* klass,
      const ArgPackType& args,
      IndexSequence<I...>) const;

  ReturnType (ClassType::* fn_)(ArgTypes...);
};


}
}

#endif
