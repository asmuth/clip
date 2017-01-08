/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V. <paul@asmuth.com>
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "metricd/config/config_parser.h"

namespace fnordmetric {

ConfigParser::ConfigParser(
    const char* input,
    size_t input_len) :
    input_(input),
    input_cur_(input_),
    input_end_(input_ + input_len),
    has_token_(false),
    has_error_(false) {}

ReturnCode ConfigParser::parse(ConfigList* config) {
  TokenType ttype;
  std::string tbuf;

  /* a file consists of a list of top-level definitions */
  while (getToken(&ttype, &tbuf)) {

    /* parse the "metric" definition */
    if (ttype == T_STRING && tbuf == "metric") {
      consumeToken();
      if (parseMetricDefinition(config)) {
        continue;
      } else {
        break;
      }
    }

    /* parse the "metric" definition */
    if (ttype == T_STRING && tbuf == "unit") {
      consumeToken();
      if (parseUnitDefinition(config)) {
        continue;
      } else {
        break;
      }
    }

    if (ttype == T_ENDLINE) {
      consumeToken();
      continue;
    }

    setError(
        StringUtil::format(
            "invalid token; got: $0, expected one of: metric",
            printToken(ttype, tbuf)));

    break;
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

bool ConfigParser::parseMetricDefinition(ConfigList* config) {
  std::string metric_name;
  if (!expectAndConsumeString(&metric_name)) {
    return false;
  }

  if (!expectAndConsumeToken(T_LCBRACE)) {
    return false;
  }

  MetricConfig metric_config;
  metric_config.is_valid = true;

  TokenType ttype;
  std::string tbuf;
  while (getToken(&ttype, &tbuf)) {
    if (ttype == T_RCBRACE) {
      break;
    }

    if (ttype == T_ENDLINE) {
      consumeToken();
      continue;
    }

    /* parse the "kind" stanza */
    if (ttype == T_STRING && tbuf == "kind") {
      consumeToken();
      if (!parseMetricDefinitionKindStanza(&metric_config)) {
        return false;
      }
      continue;
    }

    /* parse the "granularity" stanza */
    if (ttype == T_STRING && tbuf == "granularity") {
      consumeToken();
      if (!parseMetricDefinitionGranularityStanza(&metric_config)) {
        return false;
      }
      continue;
    }

    /* parse the "unit" stanza */
    if (ttype == T_STRING && tbuf == "unit") {
      consumeToken();
      if (!parseMetricDefinitionUnitStanza(&metric_config)) {
        return false;
      }
      continue;
    }

    /* parse the "unit_scale" stanza */
    if (ttype == T_STRING && tbuf == "unit_scale") {
      consumeToken();
      if (!parseMetricDefinitionUnitScaleStanza(&metric_config)) {
        return false;
      }
      continue;
    }

    setError(
        StringUtil::format(
            "invalid token: $0",
            printToken(ttype, tbuf)));
    return false;
  }

  if (!expectAndConsumeToken(T_RCBRACE)) {
    return false;
  }

  config->addMetricConfig(metric_name, metric_config);
  return true;
}

bool ConfigParser::parseMetricDefinitionKindStanza(
    MetricConfig* metric_config) {
  TokenType ttype;
  std::string tbuf;
  if (!getToken(&ttype, &tbuf) || ttype != T_STRING) {
    setError("aggregation requires an argument");
    return false;
  }

  consumeToken();

  static const std::map<std::string, MetricKind> kind_map = {
    { "sample(uint64)",     MetricKind::SAMPLE_UINT64 },
    { "sample(int64)",      MetricKind::SAMPLE_INT64 },
    { "sample(float64)",    MetricKind::SAMPLE_FLOAT64 },
    { "counter(uint64)",    MetricKind::COUNTER_UINT64 },
    { "counter(int64)",     MetricKind::COUNTER_INT64 },
    { "counter(float64)",   MetricKind::COUNTER_FLOAT64 },
    { "monotonic(uint64)",  MetricKind::MONOTONIC_UINT64 },
    { "monotonic(int64)",   MetricKind::MONOTONIC_INT64 },
    { "monotonic(float64)", MetricKind::MONOTONIC_FLOAT64 },
    { "min(uint64)",        MetricKind::MIN_UINT64 },
    { "min(int64)",         MetricKind::MIN_INT64 },
    { "min(float64)",       MetricKind::MIN_FLOAT64 },
    { "max(uint64)",        MetricKind::MAX_UINT64 },
    { "max(int64)",         MetricKind::MAX_INT64 },
    { "max(float64)",       MetricKind::MAX_FLOAT64 },
    { "average(uint64)",    MetricKind::AVERAGE_UINT64 },
    { "average(int64)",     MetricKind::AVERAGE_INT64 },
    { "average(float64)",   MetricKind::AVERAGE_FLOAT64 }
  };

  auto iter = kind_map.find(tbuf);
  if (iter != kind_map.end()) {
    metric_config->kind = iter->second;
    return true;
  } else {
    setError(
        StringUtil::format(
            "invalid metric kind: $0",
            printToken(ttype, tbuf)));

    return false;
  }
}

bool ConfigParser::parseMetricDefinitionGranularityStanza(
    MetricConfig* metric_config) {
  TokenType ttype;
  std::string tbuf;
  if (!getToken(&ttype, &tbuf) || ttype != T_STRING) {
    setError("granularity requires an argument");
    return false;
  }

  consumeToken();

  uint64_t granularity = 0;
  try {
    granularity = std::stoull(tbuf);
  } catch (...) {
    setError(
        StringUtil::format(
            "invalid value for granularity; got: $0, must be a valid number",
            printToken(ttype, tbuf)));

    return false;
  }

  metric_config->granularity = granularity;
  return true;
}

bool ConfigParser::parseMetricDefinitionUnitStanza(
    MetricConfig* metric_config) {
  TokenType ttype;
  std::string tbuf;
  if (!getToken(&ttype, &tbuf) || ttype != T_STRING) {
    setError("unit requires an argument");
    return false;
  }

  metric_config->unit_id = tbuf;
  consumeToken();
  return true;
}

bool ConfigParser::parseMetricDefinitionUnitScaleStanza(
    MetricConfig* metric_config) {
  TokenType ttype;
  std::string tbuf;
  if (!getToken(&ttype, &tbuf) || ttype != T_STRING) {
    setError("unit_scale requires an argument");
    return false;
  }

  if (!tval_parsenumber(&metric_config->unit_scale, tbuf)) {
    setError("invalid value for unit_scale <factor>: " + tbuf);
    return false;
  }

  consumeToken();
  return true;
}

bool ConfigParser::parseUnitDefinition(ConfigList* config) {
  UnitConfig unit_config;
  if (!expectAndConsumeString(&unit_config.unit_id)) {
    return false;
  }

  if (!expectAndConsumeToken(T_LCBRACE)) {
    return false;
  }


  TokenType ttype;
  std::string tbuf;
  while (getToken(&ttype, &tbuf)) {
    if (ttype == T_RCBRACE) {
      break;
    }

    if (ttype == T_ENDLINE) {
      consumeToken();
      continue;
    }

    /* parse the "unit_desc" stanza */
    if (ttype == T_STRING && tbuf == "unit_desc") {
      consumeToken();
      if (!parseUnitDefinitionDescriptionStanza(&unit_config)) {
        return false;
      }
      continue;
    }

    /* parse the "unit_name" stanza */
    if (ttype == T_STRING && tbuf == "unit_name") {
      consumeToken();
      if (!parseUnitDefinitionNameStanza(&unit_config)) {
        return false;
      }
      continue;
    }

    setError(
        StringUtil::format(
            "invalid token: $0, expected one of: unit_desc, unit_name",
            printToken(ttype, tbuf)));
    return false;
  }

  if (!expectAndConsumeToken(T_RCBRACE)) {
    return false;
  }

  config->addUnitConfig(std::move(unit_config));
  return true;
}

bool ConfigParser::parseUnitDefinitionDescriptionStanza(
    UnitConfig* unit_config) {
  TokenType ttype;
  std::string tbuf;
  if (!getToken(&ttype, &tbuf) || ttype != T_STRING) {
    setError("unit_desc requires an argument");
    return false;
  }

  unit_config->description = tbuf;
  consumeToken();
  return true;
}

bool ConfigParser::parseUnitDefinitionNameStanza(
    UnitConfig* unit_config) {
  static const std::string kArgError =
      "unit_name requires 5 arguments: unit_name <name> <factor> <singular> <plural> <symbol>";

  UnitNameConfig unc;

  TokenType name_type;
  std::string unit_name;
  if ((!getToken(&name_type, &unit_name) || name_type != T_STRING))  {
    setError(kArgError);
    return false;
  }
  consumeToken();

  // FIXME allow non string factors
  TokenType factor_type;
  std::string factor_str;
  if ((!getToken(&factor_type, &factor_str) || factor_type != T_STRING))  {
    setError(kArgError);
    return false;
  }
  consumeToken();

  if (!tval_parsenumber(&unc.factor, factor_str)) {
    setError("invalid value for unit_name <factor>: " + factor_str);
    return false;
  }

  TokenType singular_type;
  if ((!getToken(&singular_type, &unc.singular) || singular_type != T_STRING))  {
    setError(kArgError);
    return false;
  }
  consumeToken();

  TokenType plural_type;
  std::string plural_buf;
  if ((!getToken(&plural_type, &unc.plural) || plural_type != T_STRING))  {
    setError(kArgError);
    return false;
  }
  consumeToken();

  TokenType symbol_type;
  std::string symbol_buf;
  if ((!getToken(&symbol_type, &unc.symbol) || symbol_type != T_STRING))  {
    setError(kArgError);
    return false;
  }
  consumeToken();

  unit_config->names.emplace(unit_name, std::move(unc));
  return true;
}

bool ConfigParser::getToken(
    TokenType* ttype,
    std::string* tbuf) {
  const char* tbuf_cstr = nullptr;
  size_t tbuf_len = 0;

  tbuf->clear();

  bool ret = getToken(ttype, &tbuf_cstr, &tbuf_len);
  if (tbuf_cstr) {
    tbuf->append(tbuf_cstr, tbuf_len);
  }
  return ret;
}

bool ConfigParser::getToken(
    TokenType* ttype,
    const char** tbuf,
    size_t* tbuf_len) {
  char quote_char = 0;

  if (has_token_) {
    goto return_token;
  }

  /* skip whitespace */
  while (input_cur_ < input_end_) {
    if (*input_cur_ == ' ' ||
        *input_cur_ == '\t' ||
        *input_cur_ == '\r') {
      input_cur_++;
    } else {
      break;
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

    case ',': {
      token_type_ = T_COMMA;
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
        *input_cur_ != ' ' &&
        *input_cur_ != '\t' &&
        *input_cur_ != '\n' &&
        *input_cur_ != '\r' &&
        *input_cur_ != ',' &&
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
  *tbuf = token_buf_.data();
  *tbuf_len = token_buf_.size();
  return true;
}

void ConfigParser::consumeToken() {
  has_token_ = false;
  token_buf_.clear();
}

bool ConfigParser::expectAndConsumeToken(TokenType desired_type) {
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

bool ConfigParser::expectAndConsumeString(std::string* buf) {
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

std::string ConfigParser::printToken(TokenType type) {
  return printToken(type, nullptr, 0);
}

std::string ConfigParser::printToken(
    TokenType type,
    const std::string& buf) {
  return printToken(type, buf.data(), buf.size());
}

std::string ConfigParser::printToken(
    TokenType type,
    const char* buf,
    size_t buf_len) {
  std::string out;
  switch (type) {
    case T_STRING: out = "STRING"; break;
    case T_COMMA: out = "COMMA"; break;
    case T_ENDLINE: out = "ENDLINE"; break;
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

void ConfigParser::setError(const std::string& error) {
  has_error_ = true;
  error_msg_ = error;
  error_lineno_ = 0;
  error_colno_ = 0;
}

} // namespace fnordmetric

