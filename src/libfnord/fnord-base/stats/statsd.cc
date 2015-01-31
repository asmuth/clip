/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnord-base/exception.h>
#include <fnord-base/inspect.h>
#include <fnord-base/logging.h>
#include <fnord-base/stats/statsd.h>

namespace fnord {
namespace statsd {

StatsdServer::StatsdServer(
    fnord::TaskScheduler* server_scheduler,
    fnord::TaskScheduler* work_scheduler) :
    udp_server_(server_scheduler, work_scheduler) {

  udp_server_.onMessage([this] (const fnord::Buffer& msg) {
    this->messageReceived(msg);
  });
}

void StatsdServer::listen(int port) {
  logNotice("fnord.statsd.server", "Starting StatsD server on port $0", port);
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

void StatsdServer::messageReceived(const fnord::Buffer& msg) {
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
}
