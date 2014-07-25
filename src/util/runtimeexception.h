/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_UTIL_RUNTIMEXCEPTION_H
#define _FNORDMETRIC_UTIL_RUNTIMEXCEPTION_H
#include <exception>

#define RUNTIME_EXCEPTION(...) fnordmetric::util::RuntimeException( \
    __FILE__, \
    __LINE__, \
    __PRETTY_FUNCTION__, \
    __VA_ARGS__)

namespace fnordmetric {
namespace util {

class RuntimeException : public std::exception {
public:

  RuntimeException(
      const char* file,
      int line,
      const char* func,
      const char* message,
      ...);

  void debugPrint() const;

private:
  const char* file_;
  int line_;
  const char* func_;
  char message_[1024];
};

}
}
#endif
