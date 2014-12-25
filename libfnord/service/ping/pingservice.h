/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_PING_SERVICE_H
#define _FNORD_PING_SERVICE_H
#include <mutex>
#include <stdlib.h>
#include <set>
#include <string>
#include <unordered_map>
#include "fnord/reflect/reflect.h"

namespace fnord {
namespace ping_service {

class PingService {
public:
  PingService();
  std::string ping();
  std::string ping2(int i);
};

} // namespace ping_service

namespace reflect {
template <> template <class T>
void MetaClass<ping_service::PingService>::reflectMethods(T* t) {
  t->method("ping", &ping_service::PingService::ping);
  t->method("ping2", &ping_service::PingService::ping2, "i");
}
} // namespace reflect

} // namsepace fnord
#endif
