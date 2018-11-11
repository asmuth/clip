/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V. <paul@asmuth.com>
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <regex>
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
  TokenType ttype;
  std::string tbuf;

  /* a file consists of a list of top-level definitions */
  while (getToken(&ttype, &tbuf)) {
    std::string pname;
    switch (ttype) {
      case T_ENDLINE:
        consumeToken();
        continue;
      case T_STRING:
        pname = tbuf;
        consumeToken();
        break;
      default:
        setError(
            StringUtil::format(
                "unexpected token '$0'; expected STRING or ENDLINE",
                printToken(ttype, tbuf)));
        break;
    }

    if (has_error_) {
      break;
    }

    if (!getToken(&ttype, &tbuf)) {
      setError("unexpected end of file; expected COLON or LCBRACE");
      break;
    }

    switch (ttype) {
      case T_LCBRACE:
        consumeToken();
        break;
      case T_COLON:
        consumeToken();
        break;
    }
  }

  if (has_error_) {
    return ReturnCode::error(
        "EPARSE",
        StringUtil::format(
            "<$0:$1> $2",
            error_lineno_,
            error_colno_,
            error_msg_));
  } else {
    return ReturnCode::success();
  }
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

    case '\n': {
      token_type_ = T_ENDLINE;
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

void SpecParser::consumeToken() {
  has_token_ = false;
  token_buf_.clear();
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
    case T_ENDLINE: out = "ENDLINE"; break;
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
