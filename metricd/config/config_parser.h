/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V. <paul@asmuth.com>
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#pragma once
#include <atomic>
#include <memory>
#include <string>
#include "metricd/config/config_list.h"
#include "metricd/util/return_code.h"

namespace fnordmetric {

class ConfigParser {
public:

  ConfigParser(
      const char* input,
      size_t input_len);

  ReturnCode parse(ConfigList* config);

protected:
  const char* input_;
  size_t input_len_;
};

} // namespace fnordmetric

