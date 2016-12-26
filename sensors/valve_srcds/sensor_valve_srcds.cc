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
#include <map>
#include "metricd/util/flagparser.h"
#include "metricd/util/stringutil.h"
#include "metricd/transport/statsd/statsd.h"
#include "sensors/valve_srcds/srcds_client.h"

int main(int argc, const char** argv) {
  FlagParser flags;

  flags.defineFlag(
      "srcds_addr",
      FlagParser::T_STRING,
      false,
      NULL,
      NULL);

  flags.defineFlag(
      "srcds_poll_interval",
      FlagParser::T_FLOAT,
      false,
      NULL,
      "1");

  flags.defineFlag(
      "metric_prefix",
      FlagParser::T_STRING,
      false,
      NULL,
      NULL);

  flags.defineFlag(
      "metric_label",
      FlagParser::T_STRING,
      false,
      NULL,
      NULL);

  flags.defineFlag(
      "send_statsd",
      FlagParser::T_STRING,
      false,
      NULL,
      NULL);

  flags.defineFlag(
      "verbose",
      FlagParser::T_SWITCH,
      false,
      "v",
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
        "   --srcds_addr <addr>         Address of the srcds server (e.g. localhost:27015)\n"
        "   --srcds_poll_interval <n>   Poll interval (default 1s)\n"
        "   --metric_prefix <str>       Prefix all metric names with a string (e.g. 'gameserver.csgo.')\n"
        "   --metric_label <k>=<v>      Add a label to all metrics\n"
        "   --send_statsd <addr>        Send measurements via statsd/udp\n"
        "   --send_jsonudp <addr>       Send measurements via json/udp\n"
        "   --send_jsontcp <addr>       Send measurements via json/tcp\n"
        "   -?, --help                  Display this help text and exit\n"
        "   -v, --version               Display the version of this binary and exit";

    return 0;
  }

  /* check arguments */
  bool verbose = flags.isSet("verbose");

  if (!flags.isSet("srcds_addr")) {
    std::cerr << "ERROR: --srcds_addr flag must be set\n";
    return 1;
  }

  std::string metric_prefix = flags.getString("metric_prefix");
  std::map<std::string, std::string> metric_labels;
  for (const auto& lbl : flags.getStrings("metric_label")) {
    auto s = lbl.find("=");
    if (s == std::string::npos) {
      continue;
    }

    metric_labels[lbl.substr(0, s)] = lbl.substr(s + 1);
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

  /* setup statsd client if enabled */
  std::vector<std::unique_ptr<fnordmetric::statsd::StatsdEmitter>> statsd_trgts;
  for (const auto& statsd_addr : flags.getStrings("send_statsd")) {
    std::unique_ptr<fnordmetric::statsd::StatsdEmitter> statsd_emitter(
        new fnordmetric::statsd::StatsdEmitter());

    auto rc = statsd_emitter->connect(statsd_addr);
    if (!rc.isSuccess()) {
      std::cerr <<
          "WARNING: statsd emittter error:" << rc.getMessage() << std::endl;
      continue;
    }

    statsd_trgts.emplace_back(std::move(statsd_emitter));
  }

  /* periodically poll for measurements */
  for (;; usleep(flags.getFloat("srcds_poll_interval") * 1000000)) {
    fnordmetric::sensor_valve_srcds::SRCDSInfo info;
    auto rc = srcds_client.getInfo(&info);
    if (!rc.isSuccess()) {
      std::cerr <<
          "WARNING: SRCDS info request failed:" << rc.getMessage() << std::endl;
      continue;
    }

    std::vector<std::pair<std::string, std::string>> measurements;
    info.toList(&measurements);
    for (auto& m : measurements) {
      m.first.insert(0, metric_prefix);
    }

    /* if statsd output is enabled, deliver measurements via statsd */
    for (const auto& statsd_emitter : statsd_trgts) {
      for (const auto& m : measurements) {
        statsd_emitter->enqueueSample(m.first, m.second, metric_labels);
      }

      auto statsd_rc = statsd_emitter->emitSamples();
      if (!statsd_rc.isSuccess()) {
        std::cerr <<
            "WARNING: statsd send failed:" << rc.getMessage() << std::endl;
      }
    }

    /* print measurements to stdout if verbose*/
    if (verbose) {
      for (const auto& m : measurements) {
        std::cout << m.first << "=" << m.second << std::endl;
      }
    }
  }

  return 0;
}

