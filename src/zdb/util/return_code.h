/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#pragma once
#include <memory>
#include <mutex>
#include <stdlib.h>
#include <string>
#include <stdarg.h>
#include "stringutil.h"
#include "option.h"

class ReturnCode {
public:

  inline static ReturnCode success() {
    return ReturnCode(true);
  }

  inline static ReturnCode ok() {
    return ReturnCode::success();
  }

  inline static ReturnCode error(const std::string& message) {
    ReturnCode rc(false);
    rc.message_ = message;
    return rc;
  }

  inline static ReturnCode err(const std::string& message) {
    return ReturnCode::error(message);
  }

  inline static ReturnCode errorp(
      const char* message,
      ...) {
    ReturnCode rc(false);

    va_list args;
    va_start(args, message);
    int bufsize = 1024;//vsnprintf(nullptr, 0, message, args);
    rc.message_.resize(bufsize + 1);
    bufsize = vsnprintf(&rc.message_[0], rc.message_.size(), message, args);
    rc.message_.resize(bufsize);
    va_end(args);

    return rc;
  }

  template <typename... T>
  inline static ReturnCode errorf(
      const std::string& message,
      T... args) {
    return ReturnCode::error(StringUtil::format(message, args...));
  }

  operator bool() const {
    return isSuccess();
  }

  inline bool isError() const {
    return !success_;
  }

  inline bool isErr() const {
    return isError();
  }

  inline bool isSuccess() const {
    return success_;
  }

  inline bool isOk() const {
    return isSuccess();
  }

  inline const std::string& getMessage() const {
    return message_;
  }

  void check() const {
    if (!success_) {
      throw std::runtime_error(message_);
    }
  }

protected:
  ReturnCode(bool success) : success_(success) {}
  bool success_;
  std::string message_;
};

template <typename T>
class Result {
public:

  inline static Result success(const T& t) {
    return Result(Some(t));
  }

  inline static Result success(T&& t) {
    return Result(std::move(Some(std::move(t))));
  }

  inline static Result ok(const T& t) {
    return Result::success(t);
  }

  inline static Result ok(T&& t) {
    return Result::success(std::move(t));
  }

  inline static Result error(const std::string& message) {
    Result rc(None<T>());
    rc.message_ = message;
    return rc;
  }

  inline static Result err(const std::string& message) {
    return Result::error(message);
  }

  inline static Result errorp(
      const char* message,
      ...) {
    Result rc(None<T>());

    va_list args;
    va_start(args, message);
    int bufsize = 1024;//vsnprintf(nullptr, 0, message, args);
    rc.message_.resize(bufsize + 1);
    bufsize = vsnprintf(&rc.message_[0], rc.message_.size(), message, args);
    rc.message_.resize(bufsize);
    va_end(args);

    return rc;
  }

  template <typename... T1>
  inline static Result errorf(
      const std::string& message,
      T1... args) {
    return Result::error(StringUtil::format(message, args...));
  }

  operator bool() const {
    return isSuccess();
  }

  inline bool isError() const {
    return !isSuccess();
  }

  inline bool isErr() const {
    return isError();
  }

  inline bool isSuccess() const {
    return option_.isSome();
  }

  inline bool isOk() const {
    return isSuccess();
  }

  inline T& get() const {
    return option_.get();
  }

  inline const std::string& getMessage() const {
    return message_;
  }

  inline ReturnCode getError() const {
    return ReturnCode::error(message_);
  }

  const T& check() const {
    if (isError()) {
      throw std::runtime_error(message_);
    }

    return option_.get();
  }

protected:
  Result(Option<T>&& option) : option_(std::move(option)) {}
  Option<T> option_;
  std::string message_;
};

template <typename T>
Result<T> Ok(const T& value) {
  return Result<T>::ok(value);
}

template <typename T>
Result<T> Ok(T&& value) {
  return Result<T>::success(std::move(value));
}

