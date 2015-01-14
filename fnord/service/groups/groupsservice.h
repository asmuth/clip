/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_GROUP_SERVICE_H
#define _FNORD_GROUP_SERVICE_H
#include <mutex>
#include <stdlib.h>
#include <set>
#include <string>
#include <unordered_map>

namespace fnord {
namespace groups_service {

class GroupsService {
public:

  GroupsService();

  void addMember(const std::string& group, const std::string& member);

  void addMembers(
      const std::string& group,
      const std::set<std::string>& members);

  std::set<std::string> getMembers(const std::string& group) const;

protected:
  std::unordered_map<std::string, std::set<std::string>> groups_;
  mutable std::mutex groups_mutex_;
};

} // namespace groups_service
} // namsepace fnord
#endif
