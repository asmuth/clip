/**
 * Copyright (c) 2016 zScale Technology GmbH <legal@zscale.io>
 * Authors:
 *   - Paul Asmuth <paul@zscale.io>
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Affero General Public License ("the license") as
 * published by the Free Software Foundation, either version 3 of the License,
 * or any later version.
 *
 * In accordance with Section 7(e) of the license, the licensing of the Program
 * under the license does not imply a trademark license. Therefore any rights,
 * title and interest in our trademarks remain entirely with us.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the license for more details.
 *
 * You can be released from the requirements of the license by purchasing a
 * commercial license. Buying such a license is mandatory as soon as you develop
 * commercial activities involving this program without disclosing the source
 * code of your own applications
 */
#include <fnordmetric/util/exception.h>
#include <fnordmetric/util/inspect.h>
#include <fnordmetric/util/logging.h>
#include <fnordmetric/util/stats/statsd.h>

namespace statsd {

StatsdServer::StatsdServer(
    TaskScheduler* server_scheduler,
    TaskScheduler* work_scheduler) :
    udp_server_(server_scheduler, work_scheduler) {

  udp_server_.onMessage([this] (const Buffer& msg) {
    this->messageReceived(msg);
  });
}

void StatsdServer::listen(int port) {
  udp_server_.listen(port);
}

void StatsdServer::onSample(std::function<void (
    const std::string&,
    double,
    const std::vector<std::pair<std::string, std::string>>&)> callback) {
  callback_ = callback;
}

enum StatsdParseState {
  S_KEY,
  S_LABEL_OR_VALUE,
  S_LABEL,
  S_VALUE
};

void StatsdServer::messageReceived(const Buffer& msg) {
  std::string key;
  std::string value;
  std::vector<std::pair<std::string, std::string>> labels;

  auto msg_str = msg.toString();
  char const* begin = msg_str.c_str();
  char const* end = begin + msg_str.size();

  while (begin < end) {
    begin = parseStatsdSample(begin, end, &key, &value, &labels);

    double float_value;
    try {
      float_value = std::stod(value);
    } catch (std::exception& e) {
      return;
    }

    if (callback_) {
      callback_(key, float_value, labels);
    }

    labels.clear();
  }
}

char const* StatsdServer::parseStatsdSample(
    char const* begin,
    char const* end,
    std::string* key,
    std::string* value,
    std::vector<std::pair<std::string, std::string>>* labels) {
  StatsdParseState state = S_KEY;
  char const* cur = begin;
  char const* mark = cur;

  for (; cur <= end; ++cur) {
    switch (state) {
      case S_KEY: {
        switch (*cur) {
          case '[':
          case ':':
          case 0:
            break;
          case '=':
            RAISE(kParseError, "invalid equal sign in metric key");
          default:
            continue;
        }

        *key = std::string(mark, cur);
        state = *cur == '[' ? S_LABEL : S_VALUE;
        mark = cur + 1;
        break;
      }

      case S_LABEL_OR_VALUE: {
        switch (*cur) {
          case '[':
            state = S_LABEL;
            mark = cur + 1;
            continue;
          case ':':
            state = S_VALUE;
            mark = cur + 1;
            continue;
          default:
            RAISE(kParseError, "unexpected...");
            return end;
        }
      }

      case S_LABEL: {
        switch (*cur) {
          case ']':
          case ':':
          case 0:
            break;
          default:
            continue;
        }

        char const* split;
        for (split = mark; split < cur && *split != '='; ++split);
        if (split + 1 >= cur) {
          RAISE(
              kParseError,
              "invalid label (format is k=v): '%s'",
              std::string(mark, cur).c_str());
        }

        labels->emplace_back(
            std::string(mark, split),
            std::string(split + 1, cur));

        state = S_LABEL_OR_VALUE;
        mark = cur + 1;
        break;
      }

      case S_VALUE: {
        char const* lend = mark;
        for (; lend < end && *lend != '\n' && *lend != '\r'; ++lend);
        *value = std::string(mark, lend);
        for (; lend < end && (*lend == '\n' || *lend == '\r'); ++lend);
        return lend;
      }

    }
  }

  return end;
}

}
