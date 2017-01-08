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
#include "metricd/config/config_list.h"
#include "metricd/util/return_code.h"

namespace fnordmetric {

class ConfigParser {
public:

  enum TokenType {
    T_STRING,
    T_COMMA,
    T_ENDLINE,
    T_LPAREN,
    T_RPAREN,
    T_LCBRACE,
    T_RCBRACE
  };

  ConfigParser(
      const char* input,
      size_t input_len);

  ReturnCode parse(ConfigList* config);

  bool getToken(
      TokenType* type,
      std::string* buf);

  void consumeToken();

protected:

  bool parseMetricDefinition(ConfigList* config);
  bool parseMetricDefinitionKindStanza(MetricConfig* metric_config);
  bool parseMetricDefinitionGranularityStanza(MetricConfig* metric_config);
  bool parseMetricDefinitionUnitStanza(MetricConfig* metric_config);
  bool parseMetricDefinitionUnitScaleStanza(MetricConfig* metric_config);
  bool parseMetricDefinitionSummarizeGroupStanza(MetricConfig* metric_config);
  bool parseMetricDefinitionSummarizeGrossStanza(MetricConfig* metric_config);

  bool parseUnitDefinition(ConfigList* config);
  bool parseUnitDefinitionDescriptionStanza(UnitConfig* unit_config);
  bool parseUnitDefinitionNameStanza(UnitConfig* unit_config);

  bool getToken(
      TokenType* type,
      const char** buf,
      size_t* buf_len);

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
  const char* input_cur_;
  const char* input_end_;
  bool has_token_;
  TokenType token_type_;
  std::string token_buf_;
  bool has_error_;
  std::string error_msg_;
  size_t error_lineno_;
  size_t error_colno_;
};

} // namespace fnordmetric

