/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "fnord-base/inspect.h"
#include "fnord/service/groups/groupsservice.h"

namespace fnord {
namespace groups_service {

GroupsService::GroupsService() {}

void GroupsService::addMember(
    const std::string& group,
    const std::string& member) {
  std::lock_guard<std::mutex> lock_holder(groups_mutex_);
  groups_[group].insert(member);
}

void GroupsService::addMembers(
    const std::string& group,
    const std::set<std::string>& members) {
  std::lock_guard<std::mutex> lock_holder(groups_mutex_);
  auto& g = groups_[group];

  for (const auto& member: members) {
    g.insert(member);
  }
}

std::set<std::string> GroupsService::getMembers(
    const std::string& group) const {
  std::lock_guard<std::mutex> lock_holder(groups_mutex_);
  const auto& iter = groups_.find(group);

  if (iter == groups_.end()) {
    return std::set<std::string>();
  } else {
    return iter->second;
  }
}

} // namespace groups_service
} // namsepace fnord
