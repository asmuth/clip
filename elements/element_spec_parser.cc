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
#include <regex>
#include <iostream>
#include "element_spec_parser.h"

namespace signaltk {

SpecParser::SpecParser(
    const char* input,
    size_t input_len) :
    input_(input),
    input_cur_(input_),
    input_end_(input_ + input_len),
    has_token_(false),
    has_error_(false) {}

ReturnCode SpecParser::parse(PropertyList* plist) {
  if (!parseDefinitions(plist)) {
    return ReturnCode::error(
        "EPARSE",
        StringUtil::format(
            "<$0:$1> $2",
            error_lineno_,
            error_colno_,
            error_msg_));
  }

  return ReturnCode::success();
}

bool SpecParser::parseDefinitions(PropertyList* plist) {
  TokenType ttype;
  std::string tbuf;
  while (getToken(&ttype, &tbuf)) {
    if (!parsePropertyOrList(plist)) {
      return false;
    }
  }

  return true;
}

bool SpecParser::parsePropertyOrList(PropertyList* plist) {
  std::string pname;
  if (!expectAndConsumeString(&pname)) {
    return false;
  }

  TokenType ttype;
  std::string tbuf;
  if (!getToken(&ttype, &tbuf)) {
    setError("unexpected end of file; expected COLON or LCBRACE");
    return false;
  }

  switch (ttype) {
    case T_COLON:
      return
          consumeToken() &&
          parseProperty(pname, plist) &&
          expectAndConsumeToken(T_SEMICOLON);

    case T_LCBRACE:
      return
          consumeToken() &&
          parsePropertyList(pname, plist) &&
          expectAndConsumeToken(T_RCBRACE);

    default:
      setError(
          StringUtil::format(
              "unexpected token '$0'; expected COLON OR LCBRACE",
              printToken(ttype, tbuf)));
      return false;
  }
}

bool SpecParser::parseProperty(const std::string& pname, PropertyList* plist) {
  std::string pvalue;
  if (!expectAndConsumeString(&pvalue)) {
    return false;
  }

  plist->properties.emplace_back(pname, pvalue);
  return true;
}

bool SpecParser::parsePropertyList(const std::string& pname, PropertyList* plist) {
  auto pvalue = std::make_unique<PropertyList>();

  TokenType ttype;
  std::string tbuf;
  while (getToken(&ttype, &tbuf) && ttype != T_RCBRACE) {
    if (!parsePropertyOrList(pvalue.get())) {
      return false;
    }
  }

  plist->children.emplace_back(pname, std::move(pvalue));
  return true;
}

bool SpecParser::getToken(
    TokenType* ttype,
    std::string* tbuf) const {
  const char* tbuf_cstr = nullptr;
  size_t tbuf_len = 0;

  tbuf->clear();

  bool ret = getToken(ttype, &tbuf_cstr, &tbuf_len);
  if (tbuf_cstr) {
    tbuf->append(tbuf_cstr, tbuf_len);
  }

  return ret;
}

bool SpecParser::getToken(
    TokenType* ttype,
    const char** tbuf,
    size_t* tbuf_len) const {
  char quote_char = 0;

  if (has_token_) {
    goto return_token;
  }

  /* skip whitespace */
  while (input_cur_ < input_end_) {
    if (*input_cur_ == ' ' ||
        *input_cur_ == '\n' ||
        *input_cur_ == '\t' ||
        *input_cur_ == '\r') {
      ++input_cur_;
    } else {
      break;
    }
  }

  /* skip comments */
  while (input_cur_ < input_end_ && *input_cur_ == '#') {
    while (input_cur_ < input_end_) {
      if (*input_cur_++ == '\n') {
        break;
      }
    }

    while (input_cur_ < input_end_) {
      if (*input_cur_ == ' ' ||
          *input_cur_ == '\t' ||
          *input_cur_ == '\r') {
        ++input_cur_;
      } else {
        break;
      }
    }
  }

  if (input_cur_ >= input_end_) {
    return false;
  }

  /* single character tokens */
  switch (*input_cur_) {

    case ':': {
      token_type_ = T_COLON;
      input_cur_++;
      goto return_token;
    }

    case ';': {
      token_type_ = T_SEMICOLON;
      input_cur_++;
      goto return_token;
    }

    case '{': {
      token_type_ = T_LCBRACE;
      input_cur_++;
      goto return_token;
    }

    case '}': {
      token_type_ = T_RCBRACE;
      input_cur_++;
      goto return_token;
    }

    /* quoted strings */
    case '"':
    case '\'':
      quote_char = *input_cur_;
      input_cur_++;
      break;

    /* unquoted strings */
    default:
      break;
  }

  /* [un]quoted strings */
  token_type_ = T_STRING;

  if (quote_char) {
    bool escaped = false;
    bool eof = false;
    for (; !eof && input_cur_ < input_end_; input_cur_++) {
      auto chr = *input_cur_;
      switch (chr) {

        case '"':
        case '\'':
          if (escaped || quote_char != chr) {
            token_buf_ += chr;
            break;
          } else {
            eof = true;
            continue;
          }

        case '\\':
          if (escaped) {
            token_buf_ += "\\";
            break;
          } else {
            escaped = true;
            continue;
          }

        default:
          token_buf_ += chr;
          break;

      }

      escaped = false;
    }

    quote_char = 0;
    goto return_token;
  } else {
    while (
        *input_cur_ != '#' &&
        *input_cur_ != ' ' &&
        *input_cur_ != '\t' &&
        *input_cur_ != '\n' &&
        *input_cur_ != '\r' &&
        *input_cur_ != ',' &&
        *input_cur_ != ':' &&
        *input_cur_ != ';' &&
        *input_cur_ != '{' &&
        *input_cur_ != '}' &&
        *input_cur_ != '"' &&
        *input_cur_ != '\'' &&
        input_cur_ < input_end_) {
      token_buf_ += *input_cur_;
      input_cur_++;
    }

    goto return_token;
  }

return_token:
  has_token_ = true;
  *ttype = token_type_;
  *tbuf = token_buf_.data();
  *tbuf_len = token_buf_.size();
  return true;
}

bool SpecParser::consumeToken() {
  has_token_ = false;
  token_buf_.clear();
  return true;
}

bool SpecParser::expectAndConsumeToken(TokenType desired_type) {
  TokenType actual_type;
  const char* tbuf = nullptr;
  size_t tbuf_len = 0;

  if (!getToken(&actual_type, &tbuf, &tbuf_len)) {
    return false;
  }

  if (actual_type != desired_type) {
    setError(
        StringUtil::format(
            "unexpected token; expected: $0, got: $1",
            printToken(desired_type),
            printToken(actual_type, tbuf, tbuf_len)));

    return false;
  }

  consumeToken();
  return true;
}

bool SpecParser::expectAndConsumeString(std::string* buf) {
  TokenType ttype;
  if (!getToken(&ttype, buf)) {
    return false;
  }

  if (ttype != T_STRING) {
    setError(
        StringUtil::format(
            "unexpected token; expected: STRING, got: $0",
            printToken(ttype, *buf)));

    return false;
  }

  consumeToken();
  return true;
}

std::string SpecParser::printToken(TokenType type) {
  return printToken(type, nullptr, 0);
}

std::string SpecParser::printToken(
    TokenType type,
    const std::string& buf) {
  return printToken(type, buf.data(), buf.size());
}

std::string SpecParser::printToken(
    TokenType type,
    const char* buf,
    size_t buf_len) {
  std::string out;
  switch (type) {
    case T_STRING: out = "STRING"; break;
    case T_COLON: out = "COLON"; break;
    case T_SEMICOLON: out = "SEMICOLON"; break;
    case T_LCBRACE: out = "LCBRACE"; break;
    case T_RCBRACE: out = "RCBRACE"; break;
  }

  if (buf && buf_len > 0) {
    out += "<";
    out += std::string(buf, buf_len);
    out += ">";
  }

  return out;
}

void SpecParser::setError(const std::string& error) {
  has_error_ = true;
  error_msg_ = error;
  error_lineno_ = 0;
  error_colno_ = 0;
}

} // namespace signaltk
