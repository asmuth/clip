/**
 * Copyright (c) 2016 DeepCortex GmbH <legal@eventql.io>
 * Authors:
 *   - Paul Asmuth <paul@eventql.io>
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
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include "metricd/util/flagparser.h"
#include "metricd/util/stringutil.h"
#include "sensors/valve_srcds/srcds_client.h"

ReturnCode daemonize();

int main(int argc, const char** argv) {
  FlagParser flags;

  flags.defineFlag(
      "srcds_addr",
      FlagParser::T_STRING,
      false,
      NULL,
      NULL);

  flags.defineFlag(
      "help",
      FlagParser::T_SWITCH,
      false,
      "?",
      NULL);

  flags.defineFlag(
      "version",
      FlagParser::T_SWITCH,
      false,
      "V",
      NULL);

  /* parse flags */
  {
    auto rc = flags.parseArgv(argc, argv);
    if (!rc.isSuccess()) {
      std::cerr << rc.getMessage() << std::endl;
      return 1;
    }
  }

  /* print help */
  if (flags.isSet("help") || flags.isSet("version")) {
    std::cerr <<
        StringUtil::format(
            "sensor_valve_srcds $0\n"
            "Part of the FnordMetric project (http://fnordmetric.io)\n"
            "Copyright (c) 2016, Paul Asmuth, Laura Schlimmer et al. All rights reserved.\n\n",
            FNORDMETRIC_VERSION);
  }

  if (flags.isSet("version")) {
    return 0;
  }

  if (flags.isSet("help")) {
    std::cerr <<
        "Usage: $ sensor_valve_srcds [OPTIONS]\n\n"
        "   --srcds_addr <addr>       Address of the srcds server (e.g. localhost:27015)\n"
        "   -?, --help                Display this help text and exit\n"
        "   -v, --version             Display the version of this binary and exit";

    return 0;
  }

  /* check arguments */
  if (!flags.isSet("srcds_addr")) {
    std::cerr << "ERROR: --srcds_addr flag must be set\n";
    return 1;
  }

  /* setup srcds client */
  fnordmetric::sensor_valve_srcds::SRCDSClient srcds_client;
  {
    auto rc = srcds_client.connect(flags.getString("srcds_addr"));
    if (!rc.isSuccess()) {
      std::cerr << "ERROR: " << rc.getMessage() << std::endl;
      return 1;
    }
  }

  /* periodically poll for server infos */
  for (;; usleep(1000000)) {
    fnordmetric::sensor_valve_srcds::SRCDSInfo info;
    auto rc = srcds_client.getInfo(&info);
    if (!rc.isSuccess()) {
      std::cerr <<
          "WARNING: SRCDS info request failed:" << rc.getMessage() << std::endl;
      continue;
    }

  }

  return 0;
}

