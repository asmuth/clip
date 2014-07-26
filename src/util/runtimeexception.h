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
    throw (E).setSource(__FILE__, __LINE__, __PRETTY_FUNCTION__); while(0) {}

#define RAISE(E, M, ...) \
    RAISE_EXCEPTION(E((M), __VA_ARGS__).setTypeName(#E)); while(0) {}

#define RAISE_ERRNO(E, M, ...) \
    { \
      int e = errno; \
      RAISE_EXCEPTION(E((M), __VA_ARGS__).setTypeName(#E).setErrno(e)); \
    }

namespace fnordmetric {
namespace util {

class RuntimeException : public std::exception {
public:
  RuntimeException(const char* message, ...);
  RuntimeException(const RuntimeException& other);
  RuntimeException& operator=(const RuntimeException& other) = delete;

  void debugPrint() const;
  std::string getMessage() const;

  RuntimeException setSource(const char* file, int line, const char* func);
  RuntimeException setTypeName(const char* type_name);
  RuntimeException setErrno(int posix_errno);

private:
  const char* type_name_;
  const char* file_;
  int line_;
  const char* func_;
  char message_[1024];
};

}
}
#endif
