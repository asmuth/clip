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
#include <string.h>
#include "libtransport/http/sse_parser.h"

namespace libtransport {
namespace http {

void SSEParser::setEventCallback(std::function<void (const SSEEvent& ev)> fn) {
  on_event_ = fn;
}

bool SSEParser::parse(const char* data, size_t size) {
  buf_.append(data, size);

  size_t begin = 0;
  size_t end = buf_.size();
  size_t cur = 0;
  while (begin < end) {
    size_t state = 0;
    for (; state < 2 && cur < end; ++cur) {
      switch (buf_[cur]) {

        case '\n':
          ++state;
          break;

        case '\r':
          ++state;
          if (cur + 1 < end && buf_[cur + 1] == '\n') {
            ++cur;
          }
          break;

        default:
          state = 0;
          break;

      }
    }

    if (state == 2) {
      parseEvent(buf_.data() + begin, cur - begin);
      begin = cur;
    } else {
      break;
    }
  }

  if (begin > 0) {
    auto new_size = buf_.size() - begin;
    memmove(&buf_[0], (char*) buf_.data() + begin, new_size);
    buf_.resize(new_size);
  }

  return true;
}

static bool beginsWith(const std::string& str, const std::string& prefix) {
  if (str.length() < prefix.length()) {
    return false;
  }

  return str.compare(
      0,
      prefix.length(),
      prefix) == 0;
}

void SSEParser::parseEvent(const char* data, size_t size) {
  static const std::string kDataFieldPrefix = "data: ";
  static const std::string kNameFieldPrefix = "event: ";

  SSEEvent event;

  auto begin = 0;
  for (size_t cur = 0; cur < size; ++cur) {
    switch (data[cur]) {

      case '\r':
      case '\n':
        if (cur > begin) {
          std::string line(data + begin, cur - begin);

          if (beginsWith(line, kDataFieldPrefix)) {
            event.data = line.substr(kDataFieldPrefix.size());
          }

          else if (beginsWith(line, kNameFieldPrefix)) {
            event.name = line.substr(kNameFieldPrefix.size());
          }
        }

        if (cur + 1 < size && data[cur] == '\r' && data[cur + 1] == '\n') {
          ++cur;
        }

        begin = cur + 1;
        break;

    }
  }

  if (!event.data.empty() && on_event_) {
    on_event_(event);
  }
}

} // namespace http
} // namespace libtransport

