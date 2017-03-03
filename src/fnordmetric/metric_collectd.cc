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
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <regex>
#include <iostream>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/file.h>
#include <fnordmetric/util/flagparser.h>
#include <fnordmetric/util/logging.h>
#include <fnordmetric/util/fileutil.h>
#include <fnordmetric/util/daemonize.h>
#include <fnordmetric/config_list.h>
#include <fnordmetric/config_parser.h>
#include <fnordmetric/aggregation_service.h>
#include <fnordmetric/ingest.h>
#include <fnordmetric/listen_udp.h>
#include <fnordmetric/listen_http.h>

using namespace fnordmetric;

void shutdown(int);

bool parseListenAddr(
    const std::string& addr,
    std::string* host,
    uint16_t* port) {
  std::smatch m;
  std::regex listen_regex("([0-9a-zA-Z-_.]+):([0-9]+)");
  if (std::regex_match(addr, m, listen_regex)) {
    *host = m[1];
    *port = std::stoul(m[2]);
    return true;
  } else {
    return false;
  }
}

int main(int argc, const char** argv) {
  //signal(SIGTERM, shutdown);
  //signal(SIGINT, shutdown);
  //signal(SIGHUP, shutdown);
  signal(SIGPIPE, SIG_IGN);

  FlagParser flags;

  flags.defineFlag(
      "config",
      FlagParser::T_STRING,
      false,
      "c",
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

  flags.defineFlag(
      "loglevel",
      FlagParser::T_STRING,
      false,
      NULL,
      "INFO");

  flags.defineFlag(
      "daemonize",
      FlagParser::T_SWITCH,
      false,
      NULL,
      NULL);

  flags.defineFlag(
      "pidfile",
      FlagParser::T_STRING,
      false,
      NULL,
      NULL);

  flags.defineFlag(
      "log_to_syslog",
      FlagParser::T_SWITCH,
      false,
      NULL,
      NULL);

  flags.defineFlag(
      "nolog_to_stderr",
      FlagParser::T_SWITCH,
      false,
      NULL,
      NULL);

  /* parse flags */
  {
    auto rc = flags.parseArgv(argc, argv);
    if (!rc.isSuccess()) {
      std::cerr << "ERROR: " << rc.getMessage() << std::endl;
      return 1;
    }
  }

  /* setup logging */
  if (!flags.isSet("nolog_to_stderr") && !flags.isSet("daemonize")) {
    Logger::logToStderr("metricd");
  }

  if (flags.isSet("log_to_syslog")) {
    Logger::logToSyslog("metricd");
  }

  Logger::get()->setMinimumLogLevel(
      strToLogLevel(flags.getString("loglevel")));

  /* print help */
  if (flags.isSet("help") || flags.isSet("version")) {
    std::cerr <<
        StringUtil::format(
            "metricd $0\n"
            "Part of the FnordMetric project (http://fnordmetric.io)\n"
            "Copyright (c) 2016, Paul Asmuth et al. All rights reserved.\n\n",
            FNORDMETRIC_VERSION);
  }

  if (flags.isSet("version")) {
    return 0;
  }

  if (flags.isSet("help")) {
    std::cerr <<
        "Usage: $ metric-collectd [OPTIONS]\n"
        "   -c, --config <file>       Load config file\n"
        "   --daemonize               Daemonize the server\n"
        "   --pidfile <file>          Write a PID file\n"
        "   --loglevel <level>        Minimum log level (default: INFO)\n"
        "   --[no]log_to_syslog       Do[n't] log to syslog\n"
        "   --[no]log_to_stderr       Do[n't] log to stderr\n"
        "   -?, --help                Display this help text and exit\n"
        "   -v, --version             Display the version of this binary and exit\n"
        "\n"
        "Examples:\n"
        "   $ metric-collectd -c metrics.conf\n"
        "   $ metric-collectd -c metrics.conf --daemonize --log_to_syslog\n";

    return 0;
  }

  /* check flags */
  if (!flags.isSet("config")) {
    std::cerr << "ERROR: --config flag must be set" << std::endl;
    return 1;
  }

  /* daemonize */
  auto rc = ReturnCode::success();
  if (rc.isSuccess() && flags.isSet("daemonize")) {
    rc = daemonize();
  }

  /* write pidfile */
  int pidfile_fd = -1;
  auto pidfile_path = flags.getString("pidfile");
  if (rc.isSuccess() && !pidfile_path.empty()) {
    pidfile_fd = open(
        pidfile_path.c_str(),
        O_WRONLY | O_CREAT,
        0666);

    if (pidfile_fd < 0) {
      rc = ReturnCode::errorf(
          "IO_ERROR",
          "writing pidfile failed: $0",
          std::strerror(errno));
    }

    if (rc.isSuccess() && flock(pidfile_fd, LOCK_EX | LOCK_NB) != 0) {
      close(pidfile_fd);
      pidfile_fd = -1;
      rc = ReturnCode::error("IO_ERROR", "locking pidfile failed");
    }

    if (rc.isSuccess() && ftruncate(pidfile_fd, 0) != 0) {
      close(pidfile_fd);
      pidfile_fd = -1;
      rc = ReturnCode::error("IO_ERROR", "writing pidfile failed");
    }

    auto pid = StringUtil::toString(getpid());
    if (rc.isSuccess() &&
        write(pidfile_fd, pid.data(), pid.size()) != pid.size()) {
      close(pidfile_fd);
      pidfile_fd = -1;
      rc = ReturnCode::error("IO_ERROR", "writing pidfile failed");
    }
  }

  /* load config */
  ConfigList config;
  if (rc.isSuccess()) {
    auto config_file = FileUtil::read(flags.getString("config"));
    ConfigParser config_parser(
        (const char*) config_file.data(),
        config_file.size());

    rc = config_parser.parse(&config);
  }

  /* start aggregation service */
  std::unique_ptr<AggregationService> aggr_service;
  if (rc.isSuccess()) {
    rc = AggregationService::startService(&aggr_service);
  }

  if (rc.isSuccess()) {
    rc = aggr_service->applyConfig(&config);
  }

  ///* start sensor scheduler */
  //SensorScheduler sensor_sched(config.getSensorThreads());
  //if (rc.isSuccess()) {
  //  for (const auto& s : config.getSensorConfigs()) {
  //    std::unique_ptr<SensorTask> sensor_task;
  //    rc = mkSensorTask(aggr_service.get(), s.second.get(), &sensor_task);
  //    if (!rc.isSuccess()) {
  //      break;
  //    }

  //    sensor_sched.addTask(std::move(sensor_task));
  //  }
  //}

  //if (rc.isSuccess()) {
  //  rc = sensor_sched.start();
  //}

  ///* start statsd service */
  //std::unique_ptr<statsd::StatsdServer> statsd_server;
  //if (rc.isSuccess() && flags.isSet("listen_statsd")) {
  //  std::string statsd_bind;
  //  uint16_t statsd_port;
  //  auto parse_rc = parseListenAddr(
  //      flags.getString("listen_statsd"),
  //      &statsd_bind,
  //      &statsd_port);
  //  if (parse_rc) {
  //    statsd_server.reset(new statsd::StatsdServer(aggr_service.get()));
  //    rc = statsd_server->listenAndStart(statsd_bind, statsd_port);
  //  } else {
  //    rc = ReturnCode::error("ERUNTIME", "invalid value for --listen_statsd");
  //  }
  //}

  ///* run http server */
  //if (rc.isSuccess()) {
  //  std::string http_bind;
  //  uint16_t http_port;
  //  auto parse_rc = parseListenAddr(
  //      flags.getString("listen_http"),
  //      &http_bind,
  //      &http_port);

  //  if (parse_rc) {
  //    //HTTPServer server(aggr_service.get(), flags.getString("dev_assets"));
  //    //server.listen(http_bind, http_port);
  //    //server.run();
  //  } else {
  //    rc = ReturnCode::error("ERUNTIME", "invalid value for --listen_http");
  //  }
  //}

  if (!rc.isSuccess()) {
    logFatal("ERROR: $0", rc.getMessage());
  }

  /* shutdown */
  logInfo("Exiting...");
  signal(SIGTERM, SIG_IGN);
  signal(SIGINT, SIG_IGN);
  signal(SIGHUP, SIG_IGN);
  //service.reset(nullptr);

  /* unlock pidfile */
  if (pidfile_fd > 0) {
    unlink(pidfile_path.c_str());
    flock(pidfile_fd, LOCK_UN);
    close(pidfile_fd);
  }

  return rc.isSuccess() ? 0 : 1;
}

void shutdown(int) {
  // kill
}

