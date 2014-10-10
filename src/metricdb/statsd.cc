/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnordmetric/metricdb/statsd.h>
#include <fnordmetric/util/runtimeexception.h>

namespace fnordmetric {
namespace metricdb {

enum StatsdParseState {
  S_KEY,
  S_LABEL_OR_VALUE,
  S_LABEL,
  S_VALUE
};

void parseStatsdSample(
    const std::string& src,
    Sample<std::string>* dst) {
  StatsdParseState state = S_KEY;

  char const* cur = src.c_str();
  char const* end = cur + src.size();
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

        dst->key.setKey(std::string(mark, cur));
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
            return;
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

        dst->key.addLabel(
            std::string(mark, split),
            std::string(split + 1, cur));

        state = S_LABEL_OR_VALUE;
        mark = cur + 1;
        break;
      }

      case S_VALUE: {
        dst->value = std::string(mark, end);
        return;
      }

    }
  }

}

}
}
