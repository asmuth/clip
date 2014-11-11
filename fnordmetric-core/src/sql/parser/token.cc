/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnordmetric/sql/parser/token.h>
#include <fnordmetric/util/runtimeexception.h>
#include <fnordmetric/util/inspect.h>
#include <string.h>

namespace fnordmetric {
namespace query {

Token::Token(kTokenType token_type) :
    type_(token_type) {}

Token::Token(
    kTokenType token_type,
    const char* data,
    size_t len) :
    type_(token_type) {
  if (data != nullptr) {
    str_ = std::string(data, len);
  }
}

Token::Token(
    kTokenType token_type,
    const std::string& str) :
    type_(token_type),
    str_(str) {}

Token::Token(const Token& copy) :
    type_(copy.type_),
    str_(copy.str_) {}

void Token::debugPrint() const {
  if (str_.size() == 0) {
    printf("%s\n", getTypeName(type_));
  } else {
    printf("%s(%s)\n", getTypeName(type_), str_.c_str());
  }
}

const char* Token::getTypeName(kTokenType type) {
  switch (type) {
    case T_SELECT:
      return "T_SELECT";

    case T_FROM:
      return "T_FROM";

    case T_WHERE:
      return "T_WHERE";

    case T_GROUP:
      return "T_GROUP";

    case T_BY:
      return "T_BY";

    case T_HAVING:
      return "T_HAVING";

    case T_AS:
      return "T_AS";

    case T_ASC:
      return "T_ASC";

    case T_DESC:
      return "T_DESC";

    case T_COMMA:
      return "T_COMMA";

    case T_DOT:
      return "T_DOT";

    case T_SEMICOLON:
      return "T_SEMICOLON";

    case T_LPAREN:
      return "T_LPAREN";

    case T_RPAREN:
      return "T_RPAREN";

    case T_EQUAL:
      return "T_EQUAL";

    case T_PLUS:
      return "T_PLUS";

    case T_MINUS:
      return "T_MINUS";

    case T_ASTERISK:
      return "T_ASTERISK";

    case T_NOT:
      return "T_NOT";

    case T_NUMERIC:
      return "T_NUMERIC";

    case T_IDENTIFIER:
      return "T_IDENTIFIER";

    default:
      return "T_UNKNOWN_TOKEN";
  }
}

Token::kTokenType Token::getType() const {
  return type_;
}

bool Token::operator==(const std::string& string) const {
  if (str_.size() != string.size()) {
    return false;
  }

  return strncasecmp(str_.c_str(), string.c_str(), string.size()) == 0;
}

bool Token::operator==(kTokenType type) const {
  return type_ == type;
}

bool Token::operator==(const Token& other) const {
  if (type_ != other.type_) {
    return false;
  }

  return str_ == other.str_;
}

int64_t Token::getInteger() const {
  return std::stoi(str_); // FIXPAUL catch errors
}

const std::string Token::getString() const {
  return str_;
}

} // namespace query
} // namespace fnordmetric

template <>
std::string fnord::util::inspect<
    fnordmetric::query::Token::kTokenType>(
    const fnordmetric::query::Token::kTokenType& value) {
  return fnordmetric::query::Token::getTypeName(value);
}

template <>
std::string fnord::util::inspect<
    fnordmetric::query::Token>(
    const fnordmetric::query::Token& value) {
  return fnordmetric::query::Token::getTypeName(value.getType());
}
