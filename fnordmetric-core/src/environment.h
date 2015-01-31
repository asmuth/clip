/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_ENVIRONMENT_H
#define _FNORDMETRIC_ENVIRONMENT_H
#include <memory>
#include <fnordmetric/cli/flagparser.h>
#include <fnordmetric/util/logger.h>
#include "config.h"

namespace fnordmetric {

class Environment {
public:
  Environment();

  inline cli::FlagParser* flags() {
    return &flag_parser_;
  }

  inline fnord::util::Logger* logger() {
    return logger_.get();
  }

  inline bool verbose() {
    return verbose_;
  }

  void setVerbose(bool verbose);

protected:
  bool verbose_;
  cli::FlagParser flag_parser_;
  std::unique_ptr<fnord::util::Logger> logger_;
};

Environment* env();

}
#endif
