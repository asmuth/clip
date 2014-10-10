/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnordmetric/metricdb/metrickey.h>
#include <fnordmetric/util/runtimeexception.h>

namespace fnordmetric {
namespace metricdb {

const std::string& MetricKey::key() const {
  return key_;
}

void MetricKey::setKey(const std::string& key) {
  key_ = key;
}

const std::vector<std::pair<std::string, std::string>>& MetricKey::labels()
    const {
  return labels_;
}

static enum StatsdParseState {
  S_KEY,
  S_LABEL,
  S_VALUE
};

void MetricKey::parseStatsdFormat(
    const std::string& src,
    MetricKey* key,
    std::string* value) {
  StatsdParseState state = S_KEY;

  char const* cur = src.c_str();
  char const* end = cur + src.size();
  char const* mark = cur;

  for (; cur < end; ++cur) {
    switch (state) {

      /* key */
      case S_KEY:
        switch (*cur) {
          case '[':
          case ':':
            key->setKey(std::string(mark, cur));
            state = *cur == '[' ? S_LABEL : S_VALUE;
            continue;
          case '=':
            //RAISE(util::RuntimeException, "invalid equal sign in metric key");
            continue;
          default:
            continue;
        }
    }
  }
}

}
}
