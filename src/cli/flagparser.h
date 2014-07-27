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
namespace cli {

class FlagParser {
public:
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
      kFlagType type,
      bool required,
      const char* shortopt,
      const char* longopt = NULL,
      const char* default_value = NULL,
      const char* description = NULL,
      const char* placeholder = NULL);

  /**
   * Parse an argv array. This may throw an exception.
   */
  void parseArgv(const std::vector<std::string>& argv);

  /**
   * Get the argv array with all flag args removed
   */
  const std::vector<std::string>& getArgv() const;

protected:
  struct FlagState {
    kFlagType type;
    bool required;
    const char* shortopt;
    const char* longopt;
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
