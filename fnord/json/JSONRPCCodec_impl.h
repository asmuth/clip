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
  buffer->append("fnord");
}

template <typename RPCType>
void JSONRPCCodec::decodeRPCResponse(RPCType* rpc, const Buffer& buffer) {
  fnord::iputs("decode: $0", buffer.toString());
}


} // namespace json
} // namsepace fnord
