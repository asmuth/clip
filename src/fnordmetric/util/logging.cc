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
#include "logging.h"
#include "time.h"
#include <assert.h>
#include <string.h>
#include <iostream>
#ifdef HAVE_SYSLOG_H
#include <syslog.h>
#endif

const char* logLevelToStr(LogLevel log_level) {
  switch (log_level) {
    case LogLevel::kFatal: return "FATAL";
    case LogLevel::kEmergency: return "EMERGENCY";
    case LogLevel::kAlert: return "ALERT";
    case LogLevel::kCritical: return "CRITICAL";
    case LogLevel::kError: return "ERROR";
    case LogLevel::kWarning: return "WARNING";
    case LogLevel::kNotice: return "NOTICE";
    case LogLevel::kInfo: return "INFO";
    case LogLevel::kDebug: return "DEBUG";
    case LogLevel::kTrace: return "TRACE";
    default: return "CUSTOM"; // FIXPAUL
  }
}

LogLevel strToLogLevel(const std::string& log_level) {
  if (log_level == "FATAL") return LogLevel::kFatal;
  if (log_level == "EMERGENCY") return LogLevel::kEmergency;
  if (log_level == "ALERT") return LogLevel::kAlert;
  if (log_level == "CRITICAL") return LogLevel::kCritical;
  if (log_level == "ERROR") return LogLevel::kError;
  if (log_level == "WARNING") return LogLevel::kWarning;
  if (log_level == "NOTICE") return LogLevel::kNotice;
  if (log_level == "INFO") return LogLevel::kInfo;
  if (log_level == "DEBUG") return LogLevel::kDebug;
  if (log_level == "TRACE") return LogLevel::kTrace;
  assert(false); // illegal argument
}

Logger* Logger::get() {
  static Logger singleton;
  return &singleton;
}

Logger::Logger() :
    min_level_(LogLevel::kNotice),
    max_listener_index_(0) {
  for (int i = 0; i < LOGGER_MAX_LISTENERS; ++i) {
    listeners_[i] = nullptr;
  }
}

void Logger::log(
      LogLevel log_level,
      const std::string& message) {
  if (log_level < min_level_) {
    return;
  }

  const auto max_idx = max_listener_index_.load();
  for (int i = 0; i < max_idx; ++i) {
    auto listener = listeners_[i].load();

    if (listener != nullptr) {
      listener->log(log_level, message);
    }
  }
}

void Logger::addTarget(LogTarget* target) {
  auto listener_id = max_listener_index_.fetch_add(1);
  listeners_[listener_id] = target;
}

void Logger::setMinimumLogLevel(LogLevel min_level) {
  min_level_ = min_level;
}

// stderr
class StderrLogOutputStream : public LogTarget {
public:

  StderrLogOutputStream(
      const std::string& program_name);

  void log(
      LogLevel level,
      const std::string& message) override;

protected:
  std::string program_name_;
  std::mutex mutex_;
};

StderrLogOutputStream::StderrLogOutputStream(
    const std::string& program_name) :
    program_name_(program_name) {}

void StderrLogOutputStream::log(
    LogLevel level,
    const std::string& message) {
  const auto prefix = StringUtil::format(
      "$0 $1 [$2] ",
      WallClock::now().toString("%Y-%m-%d %H:%M:%S"),
      program_name_,
      logLevelToStr(level));

  std::string lines = prefix + message;
  StringUtil::replaceAll(&lines, "\n", "\n" + prefix);
  lines.append("\n");

  std::unique_lock<std::mutex> lk(mutex_);
  std::cerr << lines;
}

void Logger::logToStderr(
    const std::string& program_name,
    LogLevel min_log_level /* = LogLevel::kInfo */) {
  auto logger = new StderrLogOutputStream(program_name);
  Logger::get()->setMinimumLogLevel(min_log_level);
  Logger::get()->addTarget(logger);
}

// syslog
class SyslogTarget : public LogTarget {
public:

  SyslogTarget(const std::string& name);
  ~SyslogTarget();

  void log(
      LogLevel level,
      const std::string& message) override;

protected:
  std::unique_ptr<char> ident_;
};

SyslogTarget::SyslogTarget(const std::string& name)  {
#ifdef HAVE_SYSLOG_H
  setlogmask(LOG_UPTO(LOG_DEBUG));


  ident_ = std::unique_ptr<char>((char*) malloc(name.length() + 1));
  if (ident_.get()) {
    memcpy(ident_.get(), name.c_str(), name.length() + 1);
    openlog(ident_.get(), LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);
  } else {
    openlog(NULL, LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);
  }
#else
  RAISE(kRuntimeError, "compiled without syslog support");
#endif
}

SyslogTarget::~SyslogTarget()  {
#ifdef HAVE_SYSLOG_H
  closelog();
#endif
}

void SyslogTarget::log(
    LogLevel level,
    const std::string& message) {
#ifdef HAVE_SYSLOG_H
  syslog(LOG_NOTICE, "[%s] %s", logLevelToStr(level), message.c_str());
#endif
}

void Logger::logToSyslog(
    const std::string& name,
    LogLevel min_log_level /* = LogLevel::kInfo */) {
  auto logger = new SyslogTarget(name);
  Logger::get()->addTarget(logger);
}

