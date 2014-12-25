/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_REFLECT_SERVICEMETA_H
#define _FNORD_REFLECT_SERVICEMETA_H

namespace fnord {
namespace reflect {

class MetaClass {
public:
  const MethodCall& method(const std::string& name) const;
  MethodCall& registerMethod(const std::string& name);

protected:
  std::unordered_map<std::string, MethodCall> methods_;
};


}
}

#endif
