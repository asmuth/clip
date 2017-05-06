/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#pragma once
#include <string>
#include <vector>
#include "return_code.h"

class FlagParser {
public:
  enum kFlagType {
    T_SWITCH,
    T_STRING,
    T_FLOAT,
    T_INTEGER
  };

  struct FlagValue {
    std::string shortopt;
    std::string longopt;
    std::string value;
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
      const char* default_value = NULL);

  /**
   * Returns true if the flag is set, i.e. if a get() call with the same flag
   * will return a value (user provided or default)
   *
   * @param longopt the longopt of the flag
   */
  bool isSet(const char* longopt) const;

  /**
   * Returns true if the flag is explictly set by the user and false otherwise
   *
   * @param longopt the longopt of the flag
   */
  bool isSetExplicit(const char* longopt) const;

  /**
   * Returns the string value of the flag or throws an exception if the value
   * is invalid.
   *
   * @param longopt the longopt of the flag
   */
  std::string getString(const char* longopt) const;

  /**
   * Returns the string values of the flag or throws an exception if the value
   * is invalid.
   *
   * @param longopt the longopt of the flag
   */
  std::vector<std::string> getStrings(const char* longopt) const;

  /**
   * Returns the integer value of the flag or throws an exception if the value
   * is invalid.
   *
   * @param longopt the longopt of the flag
   */
  int64_t getInt(const char* longopt) const;

  /**
   * Returns the float value of the flag or throws an exception if the value
   * is invalid.
   *
   * @param longopt the longopt of the flag
   */
  double getFloat(const char* longopt) const;

  /**
   * Parse an argv array. This may throw an exception.
   */
  ReturnCode parseArgv(int argc, const char** argv);

  /**
   * Parse an argv array. This may throw an exception.
   */
  ReturnCode parseArgv(const std::vector<std::string>& argv);

  /**
   * Get the argv array with all flag args removed
   */
  const std::vector<std::string>& getArgv() const;

  /**
   * Get the list of all individual flag values in correct order
   */
  const std::vector<FlagValue>& getFlagValues() const;

  void ignoreUnknownFlags();

protected:
  struct FlagState {
    kFlagType type;
    bool required;
    const char* longopt;
    const char* shortopt;
    const char* default_value;
    std::vector<std::string> values;
  };

  std::vector<std::string> argv_;
  std::vector<FlagState> flags_;
  std::vector<FlagValue> flag_values_;
  bool ignore_unknown_;
};

