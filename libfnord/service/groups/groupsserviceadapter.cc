/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "fnord/json/jsonrpcrequest.h"
#include "fnord/json/jsonrpcresponse.h"
#include "fnord/service/groups/groupsservice.h"
#include "fnord/service/groups/groupsserviceadapter.h"

namespace fnord {
namespace groups_service {

void GroupsServiceAdapter::registerJSONRPC(
    GroupsService* groups_service,
    json::JSONRPC* rpc) {

  rpc->registerMethod("GroupsService.addMember", [groups_service] (
      json::JSONRPCRequest* req,
      json::JSONRPCResponse* res) {
    auto p_group = req->body().get("/params/group");
    auto p_member = req->body().get("/params/member");
    groups_service->addMember(p_group, p_member);
    res->successAndReturn(true);
  });

  rpc->registerMethod("GroupsService.addMembers", [groups_service] (
      json::JSONRPCRequest* req,
      json::JSONRPCResponse* res) {
    auto p_group = req->body().get("/params/group");

    std::set<std::string> p_members;
    req->body().forEach("/params/members", [&] (
        const json::JSONPointer& path) -> bool {
      p_members.insert(req->body().get(path));
      return true;
    });

    groups_service->addMembers(p_group, p_members);
    res->successAndReturn(true);
  });

  rpc->registerMethod("GroupsService.getMembers", [groups_service] (
      json::JSONRPCRequest* req,
      json::JSONRPCResponse* res) {
    auto p_group = req->body().get("/params/group");
    res->successAndReturn(groups_service->getMembers(p_group));
  });

}

} // namespace groups_service
} // namsepace fnord

