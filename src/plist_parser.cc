/**
 * This file is part of the "plotfx" project
 *   Copyright (c) 2018 Paul Asmuth
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
#include "plist_parser.h"
#include "stringutil.h"

#include <fmt/format.h>

namespace clip {

PropertyListParser::PropertyListParser(
    const char* input,
    size_t input_len) :
    input_(input),
    input_cur_(input_),
    input_end_(input_ + input_len),
    has_token_(false),
    has_error_(false) {}

const std::string& PropertyListParser::get_error() const {
  return error_msg_;
}

bool PropertyListParser::parse(ExprStorage* plist) {
  TokenType ttype;
  std::string tbuf;
  for (auto tail = plist; getToken(&ttype, &tbuf); tail = expr_get_next_storage(&**tail)) {
    if (!parsePropertyOrMap(tail)) {
      return false;
    }
  }

  return true;
}

bool PropertyListParser::parsePropertyOrMap(ExprStorage* plist) {
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
          parsePropertyMap(pname, plist) &&
          expectAndConsumeToken(T_RCBRACE);

    default:
      setError(
          fmt::format(
              "unexpected token '{}'; expected COLON OR LCBRACE",
              printToken(ttype, tbuf)));
      return false;
  }

  return true;
}

bool PropertyListParser::parseProperty(const std::string& pname, ExprStorage* plist) {
  auto args = expr_create_value_literal(pname);

  if (!parsePropertyValues(expr_get_next_storage(args.get()))) {
    return false;
  }

  *plist = expr_create_list(std::move(args));
  return true;
}

bool PropertyListParser::parsePropertyValues(ExprStorage* plist) {
  auto tail = plist;

  TokenType ttype;
  std::string tbuf;
  while (getToken(&ttype, &tbuf) && ttype != T_SEMICOLON) {
    switch (ttype) {
      case T_STRING_QUOTED:
      case T_STRING: {
        if (!parsePropertyValueOrEnum(tail)) {
          return false;
        }

        tail = expr_get_next_storage(tail->get());
        break;
      }

      case T_SEMICOLON:
        return true;

      case T_RPAREN:
        return true;

      case T_COMMA:
        consumeToken();
        continue;

      default:
        setError(
            fmt::format(
                "unexpected token '{}'; expected SEMICOLON OR COMMA",
                printToken(ttype, tbuf)));
        return false;
    }
  }

  return true;
}

bool PropertyListParser::parsePropertyValueOrEnum(ExprStorage* prop) {
  if (!parsePropertyValue(prop)) {
    return false;
  }

  if (!expr_is_value_literal(prop->get())) {
    return true;
  }

  TokenType ttype;
  std::string tbuf;
  if (!getToken(&ttype, &tbuf) || ttype != T_LPAREN) {
    return true;
  }

  expectAndConsumeToken(T_LPAREN);

  if (!parsePropertyValues(expr_get_next_storage(prop->get()))) {
    return false;
  }

  expectAndConsumeToken(T_RPAREN);

  *prop = expr_create_list(std::move(*prop));
  return true;
}

bool PropertyListParser::parsePropertyValue(ExprStorage* prop) {
  TokenType ttype;
  std::string tbuf;
  if (!getToken(&ttype, &tbuf)) {
    return false;
  }

  switch (ttype) {
    case T_STRING_QUOTED:
      *prop = expr_create_value(tbuf);
      consumeToken();
      break;
    case T_STRING:
      *prop = expr_create_value_literal(tbuf);
      consumeToken();
      break;
    default:
      setError(
          fmt::format(
              "unexpected token '{}'; expected STRING",
              printToken(ttype, tbuf)));
      return false;
  }

  return true;
}

bool PropertyListParser::parsePropertyMap(const std::string& pname, ExprStorage* plist) {
  auto args = expr_create_value_literal(pname);
  auto tail = expr_get_next_storage(args.get());

  TokenType ttype;
  std::string tbuf;
  while (getToken(&ttype, &tbuf) && ttype != T_RCBRACE) {
    if (!parsePropertyOrMap(tail)) {
      return false;
    }

    tail = expr_get_next_storage(tail->get());
  }

  *plist = expr_create_list(std::move(args));
  return true;
}

bool PropertyListParser::getToken(
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

bool PropertyListParser::getToken(
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
  //while (input_cur_ < input_end_ && *input_cur_ == '#') {
  //  while (input_cur_ < input_end_) {
  //    if (*input_cur_++ == '\n') {
  //      break;
  //    }
  //  }

  //  while (input_cur_ < input_end_) {
  //    if (*input_cur_ == ' ' ||
  //        *input_cur_ == '\t' ||
  //        *input_cur_ == '\r') {
  //      ++input_cur_;
  //    } else {
  //      break;
  //    }
  //  }
  //}

  if (input_cur_ >= input_end_) {
    return false;
  }

  /* single character tokens */
  switch (*input_cur_) {

    case ',': {
      token_type_ = T_COMMA;
      input_cur_++;
      goto return_token;
    }

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

    case '(': {
      token_type_ = T_LPAREN;
      input_cur_++;
      goto return_token;
    }

    case ')': {
      token_type_ = T_RPAREN;
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
  token_type_ = quote_char ? T_STRING_QUOTED : T_STRING;

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
        *input_cur_ != ' ' &&
        *input_cur_ != '\t' &&
        *input_cur_ != '\n' &&
        *input_cur_ != '\r' &&
        *input_cur_ != ',' &&
        *input_cur_ != ':' &&
        *input_cur_ != ';' &&
        *input_cur_ != '(' &&
        *input_cur_ != ')' &&
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
  *tbuf = token_buf_.c_str();
  *tbuf_len = token_buf_.size();
  return true;
}

bool PropertyListParser::consumeToken() {
  has_token_ = false;
  token_buf_.clear();
  return true;
}

bool PropertyListParser::expectAndConsumeToken(TokenType desired_type) {
  TokenType actual_type;
  const char* tbuf = nullptr;
  size_t tbuf_len = 0;

  if (!getToken(&actual_type, &tbuf, &tbuf_len)) {
    return false;
  }

  if (actual_type != desired_type) {
    setError(
        fmt::format(
            "unexpected token; expected: {}, got: {}",
            printToken(desired_type),
            printToken(actual_type, tbuf, tbuf_len)));

    return false;
  }

  consumeToken();
  return true;
}

bool PropertyListParser::expectAndConsumeString(std::string* buf) {
  TokenType ttype;
  if (!getToken(&ttype, buf)) {
    return false;
  }

  if (ttype != T_STRING && ttype != T_STRING_QUOTED) {
    setError(
        fmt::format(
            "unexpected token; expected: STRING, got: {}",
            printToken(ttype, *buf)));

    return false;
  }

  consumeToken();
  return true;
}

std::string PropertyListParser::printToken(TokenType type) {
  return printToken(type, nullptr, 0);
}

std::string PropertyListParser::printToken(
    TokenType type,
    const std::string& buf) {
  return printToken(type, buf.c_str(), buf.size());
}

std::string PropertyListParser::printToken(
    TokenType type,
    const char* buf,
    size_t buf_len) {
  std::string out;
  switch (type) {
    case T_STRING: out = "STRING"; break;
    case T_STRING_QUOTED: out = "STRING"; break;
    case T_COLON: out = "COLON"; break;
    case T_COMMA: out = "COMMA"; break;
    case T_SEMICOLON: out = "SEMICOLON"; break;
    case T_LPAREN: out = "LPAREN"; break;
    case T_RPAREN: out = "RPAREN"; break;
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

void PropertyListParser::setError(const std::string& error) {
  has_error_ = true;
  error_msg_ = error;
  error_lineno_ = 0;
  error_colno_ = 0;
}

} // namespace clip
