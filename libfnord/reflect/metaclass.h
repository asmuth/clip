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
#include "fnord/reflect/methodcall.h"

namespace fnord {
namespace reflect {

class AnyMetaClass {
public:
  virtual ~AnyMetaClass() {}
  const AnyMethodCall* method(const std::string& name) const;

protected:
  std::unordered_map<std::string, std::unique_ptr<AnyMethodCall>> methods_;
};

template <class T>
class MetaClass : public AnyMetaClass {
public:
  static MetaClass<T>* get();

  template <typename RetType, typename... ArgTypes>
  void registerMethod(
      RetType (T::* fn)(ArgTypes...),
      const std::string& name);

protected:
  MetaClass();
};


}
}

#endif
