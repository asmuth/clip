/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_UTIL_RUNTIMEXCEPTION_H
#define _FNORDMETRIC_UTIL_RUNTIMEXCEPTION_H
#include <errno.h>
#include <exception>
#include <string>

#define RAISE_EXCEPTION(E) \
  { \
    throw E; \
  }

#define RAISE(E, M, ...) \
  { \
    RAISE_EXCEPTION(E((M), __VA_ARGS__)); \
  }

#define __RUNTIME_EXCEPTION(N, T, E, ...) \
    fnordmetric::util::RuntimeException( \
        __VA_ARGS__)

#define RUNTIME_EXCEPTION(N, T, ...) \
    __RUNTIME_EXCEPTION(N, T, -1, __VA_ARGS__)

#define RUNTIME_EXCEPTION_ERRNO(N, T, ...) \
    __RUNTIME_EXCEPTION(N, T, errno, __VA_ARGS__)

namespace fnordmetric {
namespace util {

class RuntimeException : public std::exception {
public:

/*
      const void* namespace_id,
      int type_id,
      const char* type_human,
      const char* file,
      int line,
      const char* func,
      int posix_errno,
*/
  RuntimeException(const char* message, ...);
  void debugPrint() const;
  std::string getMessage() const;

private:
  const void* namespace_id_;
  const int type_id_;
  const char* type_human_;
  const char* file_;
  const int line_;
  const char* func_;
  char message_[1024];
};

}
}
#endif
