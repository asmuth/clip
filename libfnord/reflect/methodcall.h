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
#include "fnord/json/json.h"
#include "fnord/reflect/indexsequence.h"

namespace fnord {
namespace reflect {

class AnyMethodCall {
public:
  virtual ~AnyMethodCall() {}
};

template <typename _ClassType, typename _ReturnType, typename... ArgTypes>
class MethodCall : public AnyMethodCall {
public:
  typedef std::tuple<ArgTypes...> ArgPackType;
  typedef _ClassType ClassType;
  typedef _ReturnType ReturnType;

  template <typename... ArgNameTypes>
  MethodCall(ReturnType (ClassType::* fn)(ArgTypes...), ArgNameTypes... names);

  ReturnType call(ClassType* klass, ArgTypes... args) const;
  ReturnType call(ClassType* klass, const ArgPackType& args) const;

  template <class ArgListType>
  ReturnType call(ClassType* klass, const ArgListType& args) const;

protected:

  template <int... I>
  ReturnType call(
      ClassType* klass,
      const ArgPackType& args,
      IndexSequence<I...>) const;

  template <class ArgListType, int... I>
  ReturnType call(
      ClassType* klass,
      const ArgListType& args,
      IndexSequence<I...>) const;

  ReturnType (ClassType::* fn_)(ArgTypes...);
  std::vector<std::string> arg_names_;
};

}
}

#include "methodcall_impl.h"
#endif
