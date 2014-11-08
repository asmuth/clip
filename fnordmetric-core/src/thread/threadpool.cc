/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <thread>
#include <memory>
#include <fnordmetric/thread/threadpool.h>
#include <fnordmetric/util/runtimeexception.h>

using fnord::util::ExceptionHandler;

namespace fnord {
namespace thread {

ThreadPool::ThreadPool(
    std::unique_ptr<ExceptionHandler> error_handler) :
    error_handler_(std::move(error_handler)) {}

void ThreadPool::run(std::shared_ptr<Task> task) {
  new std::thread([this, task] () {
    try {
      task->run();
    } catch (const std::exception& e) {
      this->error_handler_->onException(e);
    }
  });
}

}
}
