/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Paul Asmuth, Laura Schlimmer, FnordCorp B.V.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#pragma once
#include <metrictools/util/return_code.h>
#include <metrictools/cli/command.h>

namespace fnordmetric {

class LogCommand : public Command {
public:

  ReturnCode execute(
      CLIContext* ctx,
      const std::vector<std::string>& argv) override;

  const std::string& getName() const override;
  const std::string& getDescription() const override;

  void printHelp() const override;

};

} // namespace fnordmetric

