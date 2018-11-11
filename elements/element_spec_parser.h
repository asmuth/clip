/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V. <paul@asmuth.com>
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#pragma once
#include <atomic>
#include <memory>
#include <string>
#include "utils/return_code.h"
#include "element_spec.h"

namespace signaltk {

class SpecParser {
public:

  enum TokenType {
    T_STRING,
    T_ENDLINE,
    T_COLON,
    T_SEMICOLON,
    T_LCBRACE,
    T_RCBRACE
  };

  SpecParser(
      const char* input,
      size_t input_len);

  ReturnCode parse(PropertyList* plist);

  bool getToken(
      TokenType* type,
      std::string* buf) const;

  bool hasToken() const;

  bool consumeToken();

protected:

  bool parseDefinitions(PropertyList* plist);
  bool parsePropertyOrList(PropertyList* plist);
  bool parseProperty(const std::string& pname, PropertyList* plist);
  bool parsePropertyList(const std::string& pname, PropertyList* plist);

  bool getToken(
      TokenType* type,
      const char** buf,
      size_t* buf_len) const;

  bool expectAndConsumeToken(TokenType type);
  bool expectAndConsumeString(std::string* buf);

  std::string printToken(TokenType type);

  std::string printToken(
      TokenType type,
      const std::string& buf);

  std::string printToken(
      TokenType type,
      const char* buf,
      size_t buf_len);

  void setError(const std::string& error);

  const char* input_;
  const char* input_end_;

  /* internal read ahead buffer. may mutated by const methods, but the external
   * interface of those const methods must still be side effect free */
  mutable const char* input_cur_;
  mutable bool has_token_;
  mutable TokenType token_type_;
  mutable std::string token_buf_;

  bool has_error_;
  std::string error_msg_;
  size_t error_lineno_;
  size_t error_colno_;
};

} // namespace signaltk

