/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V. <paul@asmuth.com>
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>
#include <poll.h>
#include <fcntl.h>
#include <metrictools/collect.h>
#include <metrictools/collect_proc.h>
#include <metrictools/util/time.h>
#include <metrictools/util/logging.h>
#include <metrictools/storage/ops/insert_op.h>
#include <metrictools/storage/backend.h>

namespace fnordmetric {

CollectProcTaskConfig::CollectProcTaskConfig() :
    interval(10 * kMicrosPerSecond),
    format(MeasurementCoding::STATSD) {}

ReturnCode CollectProcTask::start(
    Backend* storage_backend,
    const ConfigList* config,
    const IngestionTaskConfig* task_config,
    std::unique_ptr<IngestionTask>* task) {
  auto c = dynamic_cast<const CollectProcTaskConfig*>(task_config);
  if (!c) {
    return ReturnCode::error("ERUNTIME", "invalid ingestion task config");
  }

  if (c->command.empty()) {
    return ReturnCode::error("ERUNTIME", "missing command");
  }

  task->reset(
      new CollectProcTask(
          storage_backend,
          config,
          c->interval,
          c->command,
          c->format));

  return ReturnCode::success();
}

CollectProcTask::CollectProcTask(
    Backend* storage_backend,
    const ConfigList* config,
    uint64_t interval,
    const std::string& cmd_path,
    MeasurementCoding format) :
    PeriodicIngestionTask(interval),
    storage_backend_(storage_backend),
    config_(config),
    cmd_path_(cmd_path),
    format_(format) {}

struct PipeRef {
  PipeRef() : fds{-1, -1} {}

  ~PipeRef() {
    if (fds[0] >= 0) close(fds[0]);
    if (fds[1] >= 0) close(fds[1]);
  }

  std::array<int, 2> fds;
};

ReturnCode CollectProcTask::invoke() {
  std::string stdout_buf;
  {
    auto rc = runProcess(&stdout_buf);
    if (!rc.isSuccess()) {
      return rc;
    }
  }

  std::vector<Measurement> samples;
  {
    auto rc = parseMeasurements(format_, stdout_buf, &samples);
    if (!rc.isSuccess()) {
      return rc;
    }
  }

  return storeMeasurements(config_, storage_backend_, samples);
}

ReturnCode CollectProcTask::runProcess(std::string* stdout_buf) {
  logDebug("Running: $0", cmd_path_);

  PipeRef stdout_pipe;
  PipeRef stderr_pipe;
  pid_t pid;
  {
    auto rc = spawnProcess(&stdout_pipe, &stderr_pipe, &pid);
    if (!rc.isSuccess()) {
      return rc;
    }
  }

  {
    auto rc = waitProcess(&stdout_pipe, &stderr_pipe, stdout_buf, &pid);
    if (!rc.isSuccess()) {
      return rc;
    }
  }

  int retcode = 1;
  if (::waitpid(pid, &retcode, 0) == -1) {
    ::kill(pid, SIGKILL);
    return ReturnCode::error("EIO", "waitpid() failed, killing child");
  }

  if (retcode != 0) {
    return ReturnCode::errorf(
        "EIO",
        "process returned with non-zero exit code: $0",
        retcode);
  }

  return ReturnCode::success();
}

ReturnCode CollectProcTask::spawnProcess(
    PipeRef* stdout_pipe,
    PipeRef* stderr_pipe,
    pid_t* pid) {
  if (::pipe(stdout_pipe->fds.data()) < 0) {
    return ReturnCode::error("EIO", "pipe() failed");
  }

  if (::pipe(stderr_pipe->fds.data()) < 0) {
    return ReturnCode::error("EIO", "pipe() failed");
  }

  ::fcntl(
      stdout_pipe->fds[0],
      F_SETFL,
      ::fcntl(stdout_pipe->fds[0], F_GETFL, 0) | O_NONBLOCK);

  ::fcntl(
      stderr_pipe->fds[0],
      F_SETFL,
      ::fcntl(stderr_pipe->fds[0], F_GETFL, 0) | O_NONBLOCK);

  auto rc = ::fork();
  switch (rc) {
    case 0:
      break;
    case -1:
      return ReturnCode::error("EIO", "fork() failed");
    default:
      *pid = rc;
      return ReturnCode::success();
  }

  char* argv[1];
  argv[0] = nullptr;

  char* envp[1];
  envp[0] = nullptr;

  ::dup2(stdout_pipe->fds[1], STDOUT_FILENO);
  ::close(stdout_pipe->fds[1]);
  ::dup2(stderr_pipe->fds[1], STDERR_FILENO);
  ::close(stderr_pipe->fds[1]);

  if (::execve(cmd_path_.c_str(), argv, envp) == -1) {
    const char* err = strerror(errno);
    auto rc = ::write(STDERR_FILENO, err, strlen(err));
    (void) rc; // disable unused result warning
    exit(1);
  }

  abort(); // should not be reached
}

ReturnCode CollectProcTask::waitProcess(
    PipeRef* stdout_pipe,
    PipeRef* stderr_pipe,
    std::string* stdout_buf,
    pid_t* pid) {
  ::close(stdout_pipe->fds[1]);
  stdout_pipe->fds[1] = -1;
  ::close(stderr_pipe->fds[1]);
  stderr_pipe->fds[1] = -1;

  constexpr const size_t pollset_len = 2;
  struct pollfd pollset[pollset_len];
  pollset[0].fd = stdout_pipe->fds[0];
  pollset[0].events = POLLIN;
  pollset[1].fd = stderr_pipe->fds[0];
  pollset[1].events = POLLIN;

  std::string stderr_buf;
  for (bool hup = false; !hup; ) {
    auto poll_rc = ::poll(pollset, pollset_len, -1);
    if (poll_rc == -1) {
      ::kill(*pid, SIGKILL);
      return ReturnCode::error("EIO", "poll() failed, killing child");
    }

    for (size_t i = 0; i < pollset_len; ++i) {
      if (pollset[i].revents & POLLHUP) {
        hup = true;
        continue;
      }

      for (bool eof = false; !eof;) {
        char buf[4096];
        auto read_rc = ::read(pollset[i].fd, &buf, sizeof(buf));
        switch (read_rc) {
          case -1:
            if (errno == EAGAIN) {
              eof = true;
              continue;
            } else {
              return ReturnCode::errorf("EIO", "read() error: $0", strerror(errno));
            }
          case 0:
            eof = true;
            continue;
          default:
            break;
        }

        if (pollset[i].fd == stdout_pipe->fds[0]) {
          *stdout_buf += std::string(buf, read_rc);
        }

        if (pollset[i].fd == stderr_pipe->fds[0]) {
          stderr_buf += std::string(buf, read_rc);
        }
      }
    }
  }

  return ReturnCode::success();
}

} // namespace fnordmetric

