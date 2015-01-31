/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_CLI_FLAGPARSER_H
#define _FNORD_CLI_FLAGPARSER_H
#include <string>
#include <vector>
#include <fnord-base/exception.h>
#include <fnord-base/io/outputstream.h>

namespace fnord {
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
   * Returns the integer value of the flag or throws an exception if the value
   * is invalid.
   *
   * @param longopt the longopt of the flag
   */
  int64_t getInt(const char* longopt) const;

  /**
   * Parse an argv array. This may throw an exception.
   */
  void parseArgv(int argc, const char** argv);

  /**
   * Parse an argv array. This may throw an exception.
   */
  void parseArgv(const std::vector<std::string>& argv);

  /**
   * Get the argv array with all flag args removed
   */
  const std::vector<std::string>& getArgv() const;

  void printUsage(fnord::OutputStream* target) const;

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
