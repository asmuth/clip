/**
 * This file is part of the "plotfx" project
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V. <paul@asmuth.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 * 
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * 
 * * Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#pragma once
#include <atomic>
#include <memory>
#include <string>
#include "plist.h"

namespace plist {

class PropertyListParser {
public:

  PropertyListParser(
      const char* input,
      size_t input_len);

  bool parse(PropertyList* plist);

  const std::string& get_error() const;

protected:

  enum TokenType {
    T_STRING,
    T_STRING_QUOTED,
    T_COMMA,
    T_COLON,
    T_SEMICOLON,
    T_LPAREN,
    T_RPAREN,
    T_LCBRACE,
    T_RCBRACE
  };

  bool getToken(
      TokenType* type,
      std::string* buf) const;

  bool hasToken() const;

  bool consumeToken();

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

} // namespace plist

