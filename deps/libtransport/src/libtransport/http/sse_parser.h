/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#pragma once
#include <string>
#include <functional>

namespace libtransport {
namespace http {

struct SSEEvent {
  std::string data;
  std::string name;
};

class SSEParser {
public:

  void setEventCallback(std::function<void (const SSEEvent& ev)> fn);

  bool parse(const char* data, size_t size);

protected:

  void parseEvent(const char* data, size_t size);

  std::string buf_;
  std::function<void (const SSEEvent& ev)> on_event_;
};

} // namespace http
} // namespace libtransport

