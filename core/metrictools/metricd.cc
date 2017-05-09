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
#include <metrictools/util/flagparser.h>
#include <metrictools/util/logging.h>
#include <metrictools/util/fileutil.h>
#include <metrictools/util/daemonize.h>
#include <metrictools/config_list.h>
#include <metrictools/config_parser.h>
#include <metrictools/collect.h>
#include <metrictools/storage/backend.h>

using namespace fnordmetric;

int sig_pipe[2] = {-1, -1};
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
  FlagParser flags;

  flags.defineFlag(
      "config",
      FlagParser::T_STRING,
      false,
      "c",
      NULL);

  flags.defineFlag(
      "listen_http",
      FlagParser::T_STRING,
      false,
      NULL,
      "localhost:8080");

  flags.defineFlag(
      "datadir",
      FlagParser::T_STRING,
      false,
      NULL,
      NULL);

  flags.defineFlag(
      "plugindir",
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
      "verbose",
      FlagParser::T_SWITCH,
      false,
      "v",
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
  bool verbose = flags.isSet("verbose");
  if (!flags.isSet("nolog_to_stderr") && !flags.isSet("daemonize")) {
    Logger::logToStderr("metricd");
  }

  if (flags.isSet("log_to_syslog")) {
    Logger::logToSyslog("metricd");
  }

  if (verbose) {
    Logger::get()->setMinimumLogLevel(strToLogLevel("DEBUG"));
  } else {
    Logger::get()->setMinimumLogLevel(
        strToLogLevel(flags.getString("loglevel")));
  }

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
        "Usage: $ metricd [OPTIONS]\n"
        "   -c, --config <file>       Load config file\n"
        "   --listen_http             Listen for HTTP connection on this address\n"
        "   --daemonize               Daemonize the server\n"
        "   --pidfile <file>          Write a PID file\n"
        "   --loglevel <level>        Minimum log level (default: INFO)\n"
        "   --[no]log_to_syslog       Do[n't] log to syslog\n"
        "   --[no]log_to_stderr       Do[n't] log to stderr\n"
        "   --datadir                 Override the default datadir (/usr/share/metrictools)\n"
        "   --plugindir               Override the default plugindir (/usr/share/metrictools/plugins)\n"
        "   -?, --help                Display this help text and exit\n"
        "   -V, --version             Display the version of this binary and exit\n"
        "\n"
        "Examples:\n"
        "   $ metricd -c /etc/metrictools/metricd.conf\n"
        "   $ metricd -c /etc/metrictools/metricd.conf --daemonize --log_to_syslog\n";

    return 0;
  }

  /* search for config */
  std::string config_path;
  if (flags.isSet("config")) {
    config_path = flags.getString("config");
  } else {
    std::vector<std::string> candidates = {
      FileUtil::joinPaths(getenv("HOME"), ".metricd.conf"),
      "/etc/metrictools/metricd.conf",
      "/etc/metricd.conf"
    };

    for (const auto& c : candidates) {
      if (FileUtil::exists(c)) {
        config_path = c;
        break;
      }
    }

    if (config_path.empty()) {
      std::cerr << "ERROR: no config file found (--config), tried:" << std::endl;
      for (const auto& c : candidates) {
        std::cerr << "ERROR:   - " << c << std::endl;
      }
      return 1;
    }
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
    std::unique_ptr<ConfigParser> config_parser;
    rc = ConfigParser::openFile(&config_parser, config_path);
    if (rc.isSuccess()) {
      rc = config_parser->parse(&config);
    }
  }

  /* override config options from flags */
  if (flags.isSet("datadir")) {
    config.setDatadir(flags.getString("datadir"));
  }

  if (flags.isSet("plugindir")) {
    config.setPlugindir(flags.getString("plugindir"));
  }

  if (rc.isSuccess() && flags.isSet("listen_http")) {
    std::string http_bind;
    uint16_t http_port;
    if (rc.isSuccess()) {
      auto parse_rc = parseListenAddr(
          flags.getString("listen_http"),
          &http_bind,
          &http_port);

      if (parse_rc) {
        std::unique_ptr<ListenHTTPTaskConfig> listen_http(new ListenHTTPTaskConfig());
        listen_http->bind = http_bind;
        listen_http->port = http_port;
        config.addListenerConfig(std::move(listen_http));
      } else {
        rc = ReturnCode::error("ERUNTIME", "invalid value for --listen_http");
      }
    }
  }

  /* open backend */
  if (rc.isSuccess() && config.getBackendURL().empty()) {
    rc = ReturnCode::error("ERUNTIME", "missing backend url");
  }

  std::unique_ptr<Backend> backend;
  if (rc.isSuccess()) {
    rc = Backend::openBackend(config.getBackendURL(), &backend);
  }

  /* open signal pipe and bind signals */
  if (rc.isSuccess()) {
    if (::pipe(sig_pipe) != 0) {
      rc = ReturnCode::error("EIO", "pipe() failed");
    }

    //signal(SIGTERM, shutdown);
    //signal(SIGINT, shutdown);
    //signal(SIGHUP, shutdown);
    signal(SIGPIPE, SIG_IGN);
  }

  /* start tasks  */
  std::unique_ptr<TaskRunner> task_runner(new TaskRunner());
  if (rc.isSuccess()) {
    rc = startIngestionTasks(
        backend.get(),
        &config,
        task_runner.get());
  }

  if (rc.isSuccess()) {
    rc = startListeners(
        backend.get(),
        &config,
        task_runner.get());
  }

  task_runner->start();

  /* wait for shutdown or reload */
  while (rc.isSuccess()) {
    char sig;
    auto read_rc = read(sig_pipe[0], &sig, 1);
    if (read_rc != 1) {
      rc = ReturnCode::error("EIO", "read() failed");
      break;
    }

    // shutdown
    if (sig == 'S') { 
      break;
    }
  }

  if (!rc.isSuccess()) {
    logFatal("ERROR: $0", rc.getMessage());
  }

  /* shutdown */
  logInfo("Exiting...");
  signal(SIGTERM, SIG_DFL);
  signal(SIGINT, SIG_DFL);
  signal(SIGHUP, SIG_DFL);

  if (sig_pipe[0] >= 0) {
    close(sig_pipe[0]);
  }

  if (sig_pipe[1] >= 0) {
    close(sig_pipe[1]);
  }

  if (task_runner) {
    task_runner->shutdown();
  }

  if (backend) {
    backend->shutdown();
  }

  /* unlock pidfile */
  if (pidfile_fd > 0) {
    unlink(pidfile_path.c_str());
    flock(pidfile_fd, LOCK_UN);
    close(pidfile_fd);
  }

  return rc.isSuccess() ? 0 : 1;
}

void shutdown(int) {
  char shutdown = 'S';
  int rc = write(sig_pipe[1], &shutdown, sizeof(shutdown));
  if (rc != 1) {
    exit(1);
  }
}

