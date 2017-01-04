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
#include <libtransport/http/v1/http_server.h>
#include <metricd/util/flagparser.h>
#include <metricd/util/logging.h>
#include <metricd/util/fileutil.h>
#include <metricd/transport/http/httpapi.h>
#include <metricd/transport/statsd/statsd.h>
#include <metricd/transport/statsd/statsd_server.h>
#include <metricd/config/config_list.h>
#include <metricd/config/config_parser.h>
#include <metricd/webui/webui.h>
#include <metricd/metric_service.h>

using namespace fnordmetric;

void shutdown(int);
ReturnCode daemonize();

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
      "datadir",
      FlagParser::T_STRING,
      false,
      "d",
      NULL);

  flags.defineFlag(
      "listen_http",
      FlagParser::T_STRING,
      false,
      NULL,
      "localhost:8175");

  flags.defineFlag(
      "listen_statsd",
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

  flags.defineFlag(
      "dev_assets",
      FlagParser::T_STRING,
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
        "Usage: $ metricd [OPTIONS]\n\n"
        "   -c, --config <path>       Path to config file\n"
        "   -d, --datadir <dir>       Where to store the data\n"
        "   --daemonize               Daemonize the server\n"
        "   --pidfile <file>          Write a PID file\n"
        "   --loglevel <level>        Minimum log level (default: INFO)\n"
        "   --[no]log_to_syslog       Do[n't] log to syslog\n"
        "   --[no]log_to_stderr       Do[n't] log to stderr\n"
        "   -?, --help                Display this help text and exit\n"
        "   -v, --version             Display the version of this binary and exit\n"
        "                                                       \n"
        "Examples:                                              \n"
        "   $ fmetricd --daemonize\n";

    return 0;
  }

  /* check flags */
  if (!flags.isSet("config")) {
    std::cerr << "ERROR: --config flag must be set" << std::endl;
    return 1;
  }

  if (!flags.isSet("datadir")) {
    std::cerr << "ERROR: --datadir flag must be set" << std::endl;
    return 1;
  }

  /* init libraries */
  //curl_global_init(CURL_GLOBAL_DEFAULT);


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

  /* start metric service */
  std::unique_ptr<MetricService> metric_service;
  if (rc.isSuccess()) {
    rc = MetricService::startService(
        flags.getString("datadir"),
        &config,
        &metric_service);
  }

  /* start statsd service */
  std::unique_ptr<statsd::StatsdServer> statsd_server;
  if (rc.isSuccess() && flags.isSet("listen_statsd")) {
    std::string statsd_bind;
    uint16_t statsd_port;
    auto parse_rc = parseListenAddr(
        flags.getString("listen_statsd"),
        &statsd_bind,
        &statsd_port);
    if (parse_rc) {
      statsd_server.reset(new statsd::StatsdServer(metric_service.get()));
      rc = statsd_server->listenAndStart(statsd_bind, statsd_port);
    } else {
      rc = ReturnCode::error("ERUNTIME", "invalid value for --listen_statsd");
    }
  }

  /* run http server */
  if (rc.isSuccess()) {
    std::string http_bind;
    uint16_t http_port;
    auto parse_rc = parseListenAddr(
        flags.getString("listen_http"),
        &http_bind,
        &http_port);

    if (parse_rc) {
      WebUI webui(flags.getString("dev_assets"));
      HTTPAPI http_api(metric_service.get());

      libtransport::http::HTTPServer server;
      server.setRequestHandler(
          std::bind(
              &HTTPAPI::handleHTTPRequest,
              &http_api,
              std::placeholders::_1,
              std::placeholders::_2));

      server.listen(http_bind, http_port);
      server.run();
    } else {
      rc = ReturnCode::error("ERUNTIME", "invalid value for --listen_http");
    }
  }

  if (!rc.isSuccess()) {
    logFatal("ERROR: $0", rc.getMessage());
  }

  /* shutdown */
  if (statsd_server) {
    statsd_server->shutdown();
  }

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

  /* shutdown libraries*/
  //curl_global_cleanup();

  return rc.isSuccess() ? 0 : 1;
}

void shutdown(int) {
  // kill
}

ReturnCode daemonize() {
#if defined(_WIN32)
#error "Application::daemonize() not yet implemented for windows"

#elif defined(__APPLE__) && defined(__MACH__)
  // FIXME: deprecated on OSX
  if (::daemon(true /*no chdir*/, true /*no close*/) < 0) {
    return ReturnCode::error("INIT_FAIL", "daemon() failed");
  }

#elif defined(__linux__) || defined(__linux) || defined(linux) || defined(__gnu_linux__)
  if (::daemon(true /*no chdir*/, true /*no close*/) < 0) {
    return ReturnCode::error("INIT_FAIL", "daemon() failed");
  }

#else
#error Unsupported OS
#endif
  return ReturnCode::success();
}

