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
namespace json {

template <typename RPCType>
void JSONRPCCodec::encodeRPCRequest(RPCType* rpc, Buffer* buffer) {
  JSONOutputStream json(
      (std::unique_ptr<OutputStream>) BufferOutputStream::fromBuffer(buffer));

  json.emplace_back(JSON_OBJECT_BEGIN);
  json.emplace_back(JSON_STRING, "jsonrpc");
  json.emplace_back(JSON_STRING, "2.0");
  json.emplace_back(JSON_STRING, "method");
  json.emplace_back(JSON_STRING, rpc->method());
  json.emplace_back(JSON_STRING, "id");
  json.emplace_back(JSON_STRING, "0"); // FIXPAUL
  json.emplace_back(JSON_STRING, "params");
  fnord::json::toJSON(rpc->args(), &json);
  json.emplace_back(JSON_OBJECT_END);

}

template <typename RPCType>
void JSONRPCCodec::decodeRPCResponse(RPCType* rpc, const Buffer& buffer) {
  auto res = parseJSON(buffer);

  auto err_iter = JSONUtil::objectLookup(res.begin(), res.end(), "error");
  if (err_iter != res.end()) {
    auto err_str_i = JSONUtil::objectLookup(err_iter, res.end(), "message");
    if (err_str_i == res.end()) {
      RAISE(kRPCError, "invalid JSONRPC response");
    } else {
      RAISE(kRPCError, err_str_i->data);
    }
  }

  auto res_iter = JSONUtil::objectLookup(res.begin(), res.end(), "result");

  ScopedPtr<typename RPCType::ResultType> decoded_res(
      new typename RPCType::ResultType(
          fromJSON<typename RPCType::ResultType>(res_iter, res.end())));

  rpc->success(std::move(decoded_res));
}


} // namespace json
} // namsepace fnord
