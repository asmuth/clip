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

#define __RUNTIME_EXCEPTION(T, E, ...) \
    fnordmetric::util::RuntimeException( \
        T, \
        #T, \
        __FILE__, \
        __LINE__, \
        __PRETTY_FUNCTION__, \
        E, \
        __VA_ARGS__)

#define RUNTIME_EXCEPTION(T, ...) \
    __RUNTIME_EXCEPTION(T, -1, __VA_ARGS__)

#define RUNTIME_EXCEPTION_ERRNO(T, ...) \
    __RUNTIME_EXCEPTION(T, errno, __VA_ARGS__)

namespace fnordmetric {
namespace util {

class RuntimeException : public std::exception {
public:

  RuntimeException(
      int type_id,
      const char* type_human,
      const char* file,
      int line,
      const char* func,
      int posix_errno,
      const char* message,
      ...);

  void debugPrint() const;

private:
  int type_id_;
  const char* type_human_;
  const char* file_;
  int line_;
  const char* func_;
  char message_[1024];
};

}
}
#endif
