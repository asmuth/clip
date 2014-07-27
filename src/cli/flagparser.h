/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_FLAGPARSER_H
#define _FNORDMETRIC_FLAGPARSER_H
#include <string>
#include <vector>
#include <fnordmetric/util/runtimeexception.h>

namespace fnordmetric {
namespace util {
class OutputStream;
}
namespace cli {

class FlagParser {
public:
  struct FlagError : public fnordmetric::util::RuntimeException {
    template <typename... T>
    FlagError(
        const char* message, T... args) :
        RuntimeException(message, args...) {}
  };

  enum kFlagType {
    T_SWITCH,
    T_STRING,
    T_FLOAT,
    T_INTEGER
  };

  FlagParser();

  /**
   * Define a flag
   */
  void defineFlag(
      const char* longopt,
      kFlagType type,
      bool required,
      const char* shortopt = NULL,
      const char* default_value = NULL,
      const char* description = NULL,
      const char* placeholder = NULL);

  /**
   * Returns true if the flag is set and false otherwise
   *
   * @param longopt the longopt of the flag
   */
  bool isSet(const char* longopt) const;

  /**
   * Returns the string value of the flag or throws an exception if the value
   * is invalid.
   *
   * @param longopt the longopt of the flag
   */
  std::string getString(const char* longopt) const;

  /**
   * Parse an argv array. This may throw an exception.
   */
  void parseArgv(const std::vector<std::string>& argv);

  /**
   * Get the argv array with all flag args removed
   */
  const std::vector<std::string>& getArgv() const;

  void printUsage(util::OutputStream* target) const;

protected:
  struct FlagState {
    kFlagType type;
    bool required;
    const char* longopt;
    const char* shortopt;
    const char* default_value;
    const char* description;
    const char* placeholder;
    std::vector<std::string> values;
  };

  std::vector<std::string> argv_;
  std::vector<FlagState> flags_;
};

}
}
#endif
