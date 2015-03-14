/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
namespace fnord {
namespace thread {

template <typename T>
void Queue<T>::insert(const T& job) {
  std::unique_lock<std::mutex> lk(mutex_);
  queue_.emplace_back(job);
  lk.unlock();
  wakeup_.notify_one();
}

template <typename T>
T Queue<T>::pop() {
  std::unique_lock<std::mutex> lk(mutex_);

  while (queue_.size() == 0) {
    wakeup_.wait(lk);
  }

  auto job = queue_.front();
  queue_.pop_front();
  return job;
}

template <typename T>
Option<T> Queue<T>::poll() {
  std::unique_lock<std::mutex> lk(mutex_);

  if (queue_.size() == 0) {
    return None<T>();
  } else {
    auto job = Some(queue_.front());
    queue_.pop_front();
    return job;
  }
}

}
}
