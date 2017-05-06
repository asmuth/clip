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
#include "metrictools/config_parser.h"
#include "metrictools/util/format.h"
#include "metrictools/util/fileutil.h"

namespace fnordmetric {

ReturnCode ConfigParser::openFile(
    std::unique_ptr<ConfigParser>* parser,
    const std::string& path,
    const std::string& basepath /* = "" */) {
  std::vector<std::string> candidates = {};
  if (StringUtil::beginsWith(path, "/")) {
    candidates.emplace_back(path);
  } else {
    if (!basepath.empty()) {
      candidates.emplace_back(FileUtil::joinPaths(basepath, path));
    }

    candidates.emplace_back(FileUtil::joinPaths(FileUtil::cwd(), path));
  }

  std::string conf_path;
  for (const auto& c : candidates) {
    if (FileUtil::exists(c)) {
      conf_path = c;
      break;
    }
  }

  if (conf_path.empty()) {
    return ReturnCode::errorf(
        "EIO",
        "file not found: $0, tried:\n  - $1",
        path,
        StringUtil::join(candidates, "\n  - "));
  }  

  parser->reset(
      new ConfigParser(
          FileUtil::read(conf_path).toString(),
          FileUtil::basedir(FileUtil::realpath(conf_path))));

  return ReturnCode::success();
}

ConfigParser::ConfigParser(
    const std::string& input,
    const std::string& basepath) :
    input_str_(input),
    input_(input_str_.data()),
    input_cur_(input_),
    input_end_(input_ + input_str_.size()),
    basepath_(basepath),
    has_token_(false),
    has_error_(false) {}

ReturnCode ConfigParser::parse(ConfigList* config) {
  TokenType ttype;
  std::string tbuf;

  /* a file consists of a list of top-level definitions */
  while (getToken(&ttype, &tbuf)) {

    /* parse the "backend" stanza */
    if (ttype == T_STRING && tbuf == "backend") {
      consumeToken();
      if (parseBackendStanza(config)) {
        continue;
      } else {
        break;
      }
    }

    /* parse the "include" stanza */
    if (ttype == T_STRING && tbuf == "include") {
      consumeToken();
      if (parseIncludeStanza(config)) {
        continue;
      } else {
        break;
      }
    }

    /* parse the "labels" stanza */
    if (ttype == T_STRING && tbuf == "labels") {
      consumeToken();
      if (parseLabelsStanza(config)) {
        continue;
      } else {
        break;
      }
    }

    /* parse the "label_set" stanza */
    if (ttype == T_STRING && tbuf == "label_set") {
      consumeToken();
      if (parseLabelSetStanza(&config->getGlobalConfig()->global_label_overrides)) {
        continue;
      } else {
        break;
      }
    }

    /* parse the "metric" definition */
    if (ttype == T_STRING && tbuf == "metric") {
      consumeToken();
      if (parseMetricDefinition(config)) {
        continue;
      } else {
        break;
      }
    }

    /* parse the "unit" definition */
    if (ttype == T_STRING && tbuf == "unit") {
      consumeToken();
      if (parseUnitDefinition(config)) {
        continue;
      } else {
        break;
      }
    }

    /* parse the "collect_http" definition */
    if (ttype == T_STRING && tbuf == "collect_http") {
      consumeToken();
      if (parseCollectHTTPDefinition(config)) {
        continue;
      } else {
        break;
      }
    }

    /* parse the "collect_proc" definition */
    if (ttype == T_STRING && tbuf == "collect_proc") {
      consumeToken();
      if (parseCollectProcDefinition(config)) {
        continue;
      } else {
        break;
      }
    }

    /* parse the "listen_udp" definition */
    if (ttype == T_STRING && tbuf == "listen_udp") {
      consumeToken();
      if (parseListenUDPDefinition(config)) {
        continue;
      } else {
        break;
      }
    }

    /* parse the "listen_http" definition */
    if (ttype == T_STRING && tbuf == "listen_http") {
      consumeToken();
      if (parseListenHTTPDefinition(config)) {
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
            "invalid token; got: $0",
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

bool ConfigParser::parseBackendStanza(ConfigList* config) {
  std::string value;
  if (!expectAndConsumeString(&value)) {
    return false;
  }

  config->setBackendURL(value);
  return true;
}

bool ConfigParser::parseIncludeStanza(ConfigList* config) {
  std::string path;
  if (!expectAndConsumeString(&path)) {
    return false;
  }

  std::unique_ptr<ConfigParser> parser;
  auto rc = ConfigParser::openFile(&parser, path, basepath_);
  if (!rc.isSuccess()) {
    setError(rc.getMessage());
    return false;
  }

  rc = parser->parse(config);
  if (rc.isSuccess()) {
    return true;
  } else {
    setError(rc.getMessage());
    return false;
  }
}

bool ConfigParser::parseLabelsStanza(ConfigList* config) {
  size_t arg_count = 0;

  TokenType ttype;
  std::string tbuf;
  while (getToken(&ttype, &tbuf)) {
    if (ttype == T_ENDLINE) {
      break;
    }

    if (ttype == T_COMMA) {
      consumeToken();
      continue;
    }

    std::string column_name;
    if (!expectAndConsumeString(&column_name)) {
      return false;
    }

    config->getGlobalConfig()->global_label_config.labels.emplace_back(column_name);
    ++arg_count;
  }

  if (arg_count == 0) {
    setError("'labels' requires at least one argument");
    return false;
  }

  return true;
}

bool ConfigParser::parseLabelSetStanza(MetricLabelOverrideList* overrides) {
  std::string label;
  if (!expectAndConsumeString(&label)) {
    return false;
  }

  std::string value;
  if (!expectAndConsumeString(&value)) {
    return false;
  }

  bool is_default = false;
  {
    TokenType ttype;
    std::string tbuf;
    if (getToken(&ttype, &tbuf) && ttype == T_STRING) {
      if (tbuf == "default") {
        is_default = true;
        consumeToken();
      } else {
        setError(
            StringUtil::format(
                "unexpected string '$0', expected 'default' or end of line",
                tbuf));
        return false;
      }
    }
  }

  overrides->emplace_back(
      MetricLabelOverride {
        .label = label,
        .value = value,
        .is_default = is_default
      });

  return true;
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
  metric_config.metric_id = metric_name;

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

    /* parse the "labels" stanza */
    if (ttype == T_STRING && tbuf == "labels") {
      consumeToken();
      if (!parseMetricDefinitionLabelsStanza(&metric_config)) {
        return false;
      }
      continue;
    }

    /* parse the "label_set" stanza */
    if (ttype == T_STRING && tbuf == "label_set") {
      consumeToken();
      if (!parseLabelSetStanza(&metric_config.label_overrides)) {
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

    /* parse the "rate" stanza */
    if (ttype == T_STRING && tbuf == "rate") {
      consumeToken();
      if (!parseMetricDefinitionRateStanza(&metric_config)) {
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

    setError(
        StringUtil::format(
            "invalid token: $0",
            printToken(ttype, tbuf)));
    return false;
  }

  if (!expectAndConsumeToken(T_RCBRACE)) {
    return false;
  }

  config->addMetricConfig(metric_config);
  return true;
}

bool ConfigParser::parseMetricDefinitionLabelsStanza(
    MetricConfig* metric_config) {
  size_t arg_count = 0;

  TokenType ttype;
  std::string tbuf;
  while (getToken(&ttype, &tbuf)) {
    if (ttype == T_ENDLINE) {
      break;
    }

    if (ttype == T_COMMA) {
      consumeToken();
      continue;
    }

    std::string column_name;
    if (!expectAndConsumeString(&column_name)) {
      return false;
    }

    metric_config->label_config.labels.emplace_back(column_name);
    ++arg_count;
  }

  if (arg_count == 0) {
    setError("'labels' requires at least one argument");
    return false;
  }

  return true;
}

bool ConfigParser::parseMetricDefinitionKindStanza(
    MetricConfig* metric_config) {
  std::string kind;

  /* read type name */
  {
    TokenType ttype;
    if (!getToken(&ttype, &kind) || ttype != T_STRING) {
      setError("kind requires an argument");
      return false;
    }
    consumeToken();
  }

  /* read optional type arguments */
  {
    TokenType ttype;
    std::string tbuf;
    if (getToken(&ttype, &tbuf) && ttype == T_LPAREN) {
      consumeToken();

      if (!getToken(&ttype, &tbuf) || ttype != T_STRING) {
        setError("invalid argument to 'kind' stanza");
        return false;
      }

      kind += "(";
      kind += tbuf;
      kind += ")";

      consumeToken();

      if (!expectAndConsumeToken(T_RPAREN)) {
        return false;
      }
    }
  }

  if (!parseMetricKind(kind, &metric_config->kind)) {
    setError(StringUtil::format("invalid metric kind: $0", kind));
    return false;
  }

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

bool ConfigParser::parseMetricDefinitionRateStanza(
    MetricConfig* metric_config) {
  TokenType ttype;
  std::string tbuf;
  if (!getToken(&ttype, &tbuf) || ttype != T_STRING) {
    setError("rate requires an argument");
    return false;
  }

  consumeToken();

  uint64_t rate;
  if (parseDuration(tbuf, &rate).isSuccess()) {
    metric_config->rate = rate;
    return true;
  } else {
    setError(std::string("invalid value for rate: ") + tbuf);
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

  uint64_t granularity;
  if (parseDuration(tbuf, &granularity).isSuccess()) {
    metric_config->granularity = granularity;
    return true;
  } else {
    setError(std::string("invalid value for granularity: ") + tbuf);
    return false;
  }
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
  if ((!getToken(&name_type, &unc.name) || name_type != T_STRING))  {
    setError(kArgError);
    return false;
  }
  consumeToken();

  TokenType factor_type;
  if ((!getToken(&factor_type, &unc.factor) || factor_type != T_STRING))  {
    setError(kArgError);
    return false;
  }
  consumeToken();

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

  auto pos = std::lower_bound(
      unit_config->names.begin(),
      unit_config->names.end(),
      std::stod(unc.factor),
      [] (const UnitNameConfig& a, double b) {
        return std::stod(a.factor) < b;
      });

  unit_config->names.insert(pos, std::move(unc));
  return true;
}

bool ConfigParser::parseRewriteStanza(
    IngestionTaskConfig* sensor_config) {
  TokenType ttype;
  std::string regex_str;
  if (!getToken(&ttype, &regex_str) || ttype != T_STRING) {
    setError("metric_id_rewrite requires two arguments");
    return false;
  }

  consumeToken();

  std::string replace_str;
  if (!getToken(&ttype, &replace_str) || ttype != T_STRING) {
    setError("metric_id_rewrite requires two arguments");
    return false;
  }

  std::regex regex;
  try {
    regex = std::regex(regex_str);
  } catch (const std::exception& e) {
    setError(StringUtil::format("invalid regex: $0", e.what()));
    return false;
  }

  consumeToken();

  sensor_config->metric_id_rewrite_enabled = true;
  sensor_config->metric_id_rewrite_regex = regex;
  sensor_config->metric_id_rewrite_replace = replace_str;

  return true;
}

bool ConfigParser::parseCollectProcDefinition(ConfigList* config) {
  std::unique_ptr<CollectProcTaskConfig> ingestion_task(
      new CollectProcTaskConfig());

  ingestion_task->basepath = basepath_;

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

    /* parse the "url" stanza */
    if (ttype == T_STRING && tbuf == "cmd") {
      consumeToken();
      if (!parseCollectProcDefinitionCommandStanza(ingestion_task.get())) {
        return false;
      }
      continue;
    }

    /* parse the "format" stanza */
    if (ttype == T_STRING && tbuf == "format") {
      consumeToken();
      if (!parseCollectProcDefinitionFormatStanza(ingestion_task.get())) {
        return false;
      }
      continue;
    }

    /* parse the "interval" stanza */
    if (ttype == T_STRING && tbuf == "interval") {
      consumeToken();
      if (!parseCollectProcDefinitionIntervalStanza(ingestion_task.get())) {
        return false;
      }
      continue;
    }

    /* parse the "label_set" stanza */
    if (ttype == T_STRING && tbuf == "label_set") {
      consumeToken();
      if (!parseLabelSetStanza(&ingestion_task->label_overrides)) {
        return false;
      }
      continue;
    }

    setError(StringUtil::format("unexpected token: $0", printToken(ttype, tbuf)));
    return false;
  }

  if (!expectAndConsumeToken(T_RCBRACE)) {
    return false;
  }

  config->addIngestionTaskConfig(std::move(ingestion_task));
  return true;
}

bool ConfigParser::parseCollectProcDefinitionCommandStanza(
    CollectProcTaskConfig* config) {
  size_t arg_count = 0;

  TokenType ttype;
  std::string tbuf;
  while (getToken(&ttype, &tbuf)) {
    if (ttype == T_ENDLINE) {
      break;
    }

    std::string part;
    if (!expectAndConsumeString(&part)) {
      return false;
    }

    config->command.emplace_back(part);
    ++arg_count;
  }

  if (arg_count == 0) {
    setError("'cmd' requires at least one argument");
    return false;
  }

  return true;
}

bool ConfigParser::parseCollectProcDefinitionFormatStanza(
    CollectProcTaskConfig* config) {
  TokenType ttype;
  std::string tbuf;
  if (!getToken(&ttype, &tbuf) || ttype != T_STRING) {
    setError("format requires an argument");
    return false;
  }

  consumeToken();

  MeasurementCoding format;
  if (parseMeasurementCoding(tbuf, &format)) {
    config->format = format;
    return true;
  } else {
    setError(std::string("invalid value for format: ") + tbuf);
    return false;
  }
}

bool ConfigParser::parseCollectProcDefinitionIntervalStanza(
    CollectProcTaskConfig* config) {
  TokenType ttype;
  std::string tbuf;
  if (!getToken(&ttype, &tbuf) || ttype != T_STRING) {
    setError("interval requires an argument");
    return false;
  }

  consumeToken();

  uint64_t interval;
  if (parseDuration(tbuf, &interval).isSuccess()) {
    config->interval = interval;
    return true;
  } else {
    setError(std::string("invalid value for interval: ") + tbuf);
    return false;
  }
}


bool ConfigParser::parseCollectHTTPDefinition(ConfigList* config) {
  std::unique_ptr<HTTPPullIngestionTaskConfig> ingestion_task(
      new HTTPPullIngestionTaskConfig());

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

    /* parse the "url" stanza */
    if (ttype == T_STRING && tbuf == "url") {
      consumeToken();
      if (!parseCollectHTTPDefinitionURLStanza(ingestion_task.get())) {
        return false;
      }
      continue;
    }

    /* parse the "format" stanza */
    if (ttype == T_STRING && tbuf == "format") {
      consumeToken();
      if (!parseCollectHTTPDefinitionFormatStanza(ingestion_task.get())) {
        return false;
      }
      continue;
    }

    /* parse the "interval" stanza */
    if (ttype == T_STRING && tbuf == "interval") {
      consumeToken();
      if (!parseCollectHTTPDefinitionIntervalStanza(ingestion_task.get())) {
        return false;
      }
      continue;
    }

    /* parse the "label_set" stanza */
    if (ttype == T_STRING && tbuf == "label_set") {
      consumeToken();
      if (!parseLabelSetStanza(&ingestion_task->label_overrides)) {
        return false;
      }
      continue;
    }

    setError(StringUtil::format("unexpected token: $0", printToken(ttype, tbuf)));
    return false;
  }

  if (!expectAndConsumeToken(T_RCBRACE)) {
    return false;
  }

  config->addIngestionTaskConfig(std::move(ingestion_task));
  return true;
}

bool ConfigParser::parseCollectHTTPDefinitionURLStanza(
    HTTPPullIngestionTaskConfig* config) {
  TokenType ttype;
  std::string tbuf;
  if (!getToken(&ttype, &tbuf) || ttype != T_STRING) {
    setError("http_url requires an argument");
    return false;
  }

  config->url = tbuf;
  consumeToken();
  return true;
}

bool ConfigParser::parseCollectHTTPDefinitionFormatStanza(
    HTTPPullIngestionTaskConfig* config) {
  TokenType ttype;
  std::string tbuf;
  if (!getToken(&ttype, &tbuf) || ttype != T_STRING) {
    setError("format requires an argument");
    return false;
  }

  consumeToken();

  MeasurementCoding format;
  if (parseMeasurementCoding(tbuf, &format)) {
    config->format = format;
    return true;
  } else {
    setError(std::string("invalid value for format: ") + tbuf);
    return false;
  }
}

bool ConfigParser::parseCollectHTTPDefinitionIntervalStanza(
    HTTPPullIngestionTaskConfig* config) {
  TokenType ttype;
  std::string tbuf;
  if (!getToken(&ttype, &tbuf) || ttype != T_STRING) {
    setError("interval requires an argument");
    return false;
  }

  consumeToken();

  uint64_t interval;
  if (parseDuration(tbuf, &interval).isSuccess()) {
    config->interval = interval;
    return true;
  } else {
    setError(std::string("invalid value for interval: ") + tbuf);
    return false;
  }
}

bool ConfigParser::parseListenHTTPDefinition(ConfigList* config) {
  std::unique_ptr<HTTPPushIngestionTaskConfig> ingestion_task(
      new HTTPPushIngestionTaskConfig());

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

    /* parse the "bind" stanza */
    if (ttype == T_STRING && tbuf == "bind") {
      consumeToken();
      if (!parseListenHTTPDefinitionBindStanza(ingestion_task.get())) {
        return false;
      }
      continue;
    }

    /* parse the "port" stanza */
    if (ttype == T_STRING && tbuf == "port") {
      consumeToken();
      if (!parseListenHTTPDefinitionPortStanza(ingestion_task.get())) {
        return false;
      }
      continue;
    }

    setError(StringUtil::format("unexpected token: $0", printToken(ttype, tbuf)));
    return false;
  }

  if (!expectAndConsumeToken(T_RCBRACE)) {
    return false;
  }

  config->addIngestionTaskConfig(std::move(ingestion_task));
  return true;
}

bool ConfigParser::parseListenHTTPDefinitionBindStanza(
    HTTPPushIngestionTaskConfig* config) {
  TokenType ttype;
  std::string tbuf;
  if (!getToken(&ttype, &tbuf) || ttype != T_STRING) {
    setError("bind requires an argument");
    return false;
  }

  config->bind = tbuf;
  consumeToken();
  return true;
}

bool ConfigParser::parseListenHTTPDefinitionPortStanza(
    HTTPPushIngestionTaskConfig* config) {
  TokenType ttype;
  std::string tbuf;
  if (!getToken(&ttype, &tbuf) || ttype != T_STRING) {
    setError("port requires an argument");
    return false;
  }

  try {
    auto val = std::stoul(tbuf);
    if (val > std::numeric_limits<uint16_t>::max()) {
      setError("port number out of range");
      return false;
    }

    config->port = val;
  } catch (...) {
    setError(std::string("invalid value for port: ") + tbuf);
    return false;
  }

  consumeToken();
  return true;
}


bool ConfigParser::parseListenUDPDefinition(ConfigList* config) {
  std::unique_ptr<UDPIngestionTaskConfig> ingestion_task(
      new UDPIngestionTaskConfig());

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

    /* parse the "bind" stanza */
    if (ttype == T_STRING && tbuf == "bind") {
      consumeToken();
      if (!parseListenUDPDefinitionBindStanza(ingestion_task.get())) {
        return false;
      }
      continue;
    }

    /* parse the "port" stanza */
    if (ttype == T_STRING && tbuf == "port") {
      consumeToken();
      if (!parseListenUDPDefinitionPortStanza(ingestion_task.get())) {
        return false;
      }
      continue;
    }

    /* parse the "format" stanza */
    if (ttype == T_STRING && tbuf == "format") {
      consumeToken();
      if (!parseListenUDPDefinitionFormatStanza(ingestion_task.get())) {
        return false;
      }
      continue;
    }

    setError(StringUtil::format("unexpected token: $0", printToken(ttype, tbuf)));
    return false;
  }

  if (!expectAndConsumeToken(T_RCBRACE)) {
    return false;
  }

  config->addIngestionTaskConfig(std::move(ingestion_task));
  return true;
}

bool ConfigParser::parseListenUDPDefinitionBindStanza(
    UDPIngestionTaskConfig* config) {
  TokenType ttype;
  std::string tbuf;
  if (!getToken(&ttype, &tbuf) || ttype != T_STRING) {
    setError("bind requires an argument");
    return false;
  }

  config->bind = tbuf;
  consumeToken();
  return true;
}

bool ConfigParser::parseListenUDPDefinitionPortStanza(
    UDPIngestionTaskConfig* config) {
  TokenType ttype;
  std::string tbuf;
  if (!getToken(&ttype, &tbuf) || ttype != T_STRING) {
    setError("port requires an argument");
    return false;
  }

  try {
    auto val = std::stoul(tbuf);
    if (val > std::numeric_limits<uint16_t>::max()) {
      setError("port number out of range");
      return false;
    }

    config->port = val;
  } catch (...) {
    setError(std::string("invalid value for port: ") + tbuf);
    return false;
  }

  consumeToken();
  return true;
}

bool ConfigParser::parseListenUDPDefinitionFormatStanza(
    UDPIngestionTaskConfig* config) {
  TokenType ttype;
  std::string tbuf;
  if (!getToken(&ttype, &tbuf) || ttype != T_STRING) {
    setError("format requires an argument");
    return false;
  }

  consumeToken();

  MeasurementCoding format;
  if (parseMeasurementCoding(tbuf, &format)) {
    config->format = format;
    return true;
  } else {
    setError(std::string("invalid value for format: ") + tbuf);
    return false;
  }
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
        *input_cur_ != '#' &&
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

