/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2017 Paul Asmuth <paul@asmuth.com>
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <assert.h>
#include <metrictools/storage/sqlite/sqlite_backend.h>
#include <metrictools/util/logging.h>

namespace fnordmetric {
namespace sqlite_backend {

/**
 * TODO:
 *  - check if schema matches and upgrade table/indexes if possible
 *  - cache tables creation (should only happen once for a given config combination)
 */

ReturnCode SQLiteBackend::connect(
    const URI& backend_uri,
    std::unique_ptr<Backend>* backend) {
  backend->reset(new SQLiteBackend());
  return ((SQLiteBackend*) backend->get())->open(backend_uri.path());
}

SQLiteBackend::SQLiteBackend() : db_(nullptr) {}

SQLiteBackend::~SQLiteBackend() {
  if (db_) {
    sqlite3_close(db_);
  }
}

ReturnCode SQLiteBackend::performOperation(InsertStorageOp* op) {
  logDebug("Performing INSERT storage operation (sqlite)");

  std::unique_lock<std::mutex> lk(mutex_);

  auto global_config = op->getGlobalConfig();

  for (const auto& m : op->getMeasurements()) {
    auto rc = insertMeasurement(global_config.get(), m.metric.get(), m);
    if (!rc.isSuccess()) {
      return rc;
    }
  }

  return ReturnCode::success();
}

ReturnCode SQLiteBackend::performOperation(FetchStorageOp* op) {
  logDebug("Performing FETCH storage operation (sqlite)");

  std::unique_lock<std::mutex> lk(mutex_);

  for (const auto& request : op->getRequests()) {
    auto rc = fetchData(op, &request);
    if (!rc.isSuccess()) {
      return rc;
    }
  }

  return ReturnCode::success();
}

ReturnCode SQLiteBackend::open(const std::string& path) {
  if (sqlite3_open(path.c_str(), &db_)){
    return ReturnCode::errorf("EIO", "sqlite error: $0", sqlite3_errmsg(db_));
  }

  return ReturnCode::success();
}

static int sqlite_cb(void* udata, int argc, char** argv, char** col_name) {
  if (!udata) {
    return 0;
  }

  std::vector<std::string> row(argc);
  for (size_t i = 0; i < argc; ++i) {
    row[i] = std::string(argv[i]);
  }

  auto rows = static_cast<std::list<std::vector<std::string>>*>(udata);
  rows->emplace_back(std::move(row)); 

  return 0;
}

ReturnCode SQLiteBackend::executeQuery(
    const std::string& query,
    std::list<std::vector<std::string>>* rows /* = nullptr */) {
  logDebug("Executing SQLite query: $0", query);

  char* err = nullptr;
  auto rc = sqlite3_exec(db_, query.c_str(), &sqlite_cb, rows, &err);
  if (rc != SQLITE_OK){
    auto err_str = std::string(err);
    sqlite3_free(err);
    return ReturnCode::errorf("EIO", "sqlite error: $0", err_str);
  }

  return ReturnCode::success();
}

ReturnCode SQLiteBackend::createTables(
    const GlobalConfig* global_config,
    const MetricConfig* metric_config) {
  std::string value_col;
  switch (metric_config->kind.type) {
    case MetricDataType::UINT64:
      value_col = "value bigint";
      break;
    case MetricDataType::INT64:
      value_col = "value bigint";
      break;
    case MetricDataType::FLOAT64:
      value_col = "value float";
      break;
    case MetricDataType::STRING:
      value_col = "value string";
      break;
  }

  std::vector<std::string> label_col_names;
  std::vector<std::string> label_cols;
  for (const auto& l : global_config->global_label_config.labels) {
    label_col_names.emplace_back(escapeString(l));
    label_cols.emplace_back(StringUtil::format("$0 string", escapeString(l)));
  }

  for (const auto& l : metric_config->label_config.labels) {
    label_col_names.emplace_back(escapeString(l));
    label_cols.emplace_back(StringUtil::format("$0 string", escapeString(l)));
  }

  {
    std::vector<std::string> cols;
    cols.insert(cols.end(), label_cols.begin(), label_cols.end());
    cols.emplace_back(value_col);

    auto qry = StringUtil::format(
        "CREATE TABLE IF NOT EXISTS $0 ($1);",
        escapeString(metric_config->metric_id + ":last"),
        StringUtil::join(cols, ", "));

    auto rc = executeQuery(qry);
    if (!rc.isSuccess()) {
      return rc;
    }
  }

  if (!label_col_names.empty()) {
    auto qry = StringUtil::format(
        "CREATE UNIQUE INDEX IF NOT EXISTS $0 ON $1 ($2);",
        escapeString(metric_config->metric_id + ":last_index"),
        escapeString(metric_config->metric_id + ":last"),
        StringUtil::join(label_col_names, ", "));

    auto rc = executeQuery(qry);
    if (!rc.isSuccess()) {
      return rc;
    }
  }

  {
    std::vector<std::string> cols;
    cols.emplace_back("time bigint");
    cols.insert(cols.end(), label_cols.begin(), label_cols.end());
    cols.emplace_back(value_col);

    auto qry = StringUtil::format(
        "CREATE TABLE IF NOT EXISTS $0 ($1);",
        escapeString(metric_config->metric_id + ":history"),
        StringUtil::join(cols, ", "));

    auto rc = executeQuery(qry);
    if (!rc.isSuccess()) {
      return rc;
    }
  }

  {
    auto qry = StringUtil::format(
        "CREATE UNIQUE INDEX IF NOT EXISTS $0 ON $1 (time);",
        escapeString(metric_config->metric_id + ":history_index"),
        escapeString(metric_config->metric_id + ":history"));

    auto rc = executeQuery(qry);
    if (!rc.isSuccess()) {
      return rc;
    }
  }

  return ReturnCode::success();
}

ReturnCode SQLiteBackend::insertMeasurement(
    const GlobalConfig* global_config,
    const MetricConfig* metric_config,
    const InsertStorageOp::Measurement& m) {
  {
    auto rc = createTables(global_config, metric_config);
    if (!rc.isSuccess()) {
      return rc;
    }
  }

  {
    auto rc = executeQuery("BEGIN");
    if (!rc.isSuccess()) {
      return rc;
    }
  }

  {
    std::vector<std::string> col_names;
    std::vector<std::string> col_values;

    assert(m.label.labels.size() == m.label.values.size());
    for (size_t i = 0; i < m.label.labels.size(); ++i) {
      col_names.emplace_back(escapeString(m.label.labels[i]));
      col_values.emplace_back(escapeString(m.label.values[i]));
    }

    col_names.emplace_back("value");
    col_values.emplace_back(escapeString(m.value));

    auto qry = StringUtil::format(
        "REPLACE INTO $0 ($1) VALUES ($2);",
        escapeString(metric_config->metric_id + ":last"),
        StringUtil::join(col_names, ", "),
        StringUtil::join(col_values, ", "));

    if (m.label.labels.empty()) {
      qry = StringUtil::format(
          "DELETE FROM $0; $1",
          escapeString(metric_config->metric_id + ":last"),
          qry);
    }

    auto rc = executeQuery(qry);
    if (!rc.isSuccess()) {
      return rc;
    }
  }

  {
    std::vector<std::string> col_names;
    std::vector<std::string> col_values;

    col_names.emplace_back("time");
    col_values.emplace_back(std::to_string(m.time));

    assert(m.label.labels.size() == m.label.values.size());
    for (size_t i = 0; i < m.label.labels.size(); ++i) {
      col_names.emplace_back(escapeString(m.label.labels[i]));
      col_values.emplace_back(escapeString(m.label.values[i]));
    }

    col_names.emplace_back("value");
    col_values.emplace_back(escapeString(m.value));

    auto qry = StringUtil::format(
        "INSERT INTO $0 ($1) VALUES ($2);",
        escapeString(metric_config->metric_id + ":history"),
        StringUtil::join(col_names, ", "),
        StringUtil::join(col_values, ", "));

    auto rc = executeQuery(qry);
    if (!rc.isSuccess()) {
      return rc;
    }
  }

  {
    auto rc = executeQuery("COMMIT");
    if (!rc.isSuccess()) {
      return rc;
    }
  }

  return ReturnCode::success();
}

ReturnCode SQLiteBackend::fetchData(
    FetchStorageOp* op,
    const FetchStorageOp::FetchRequest* request) {
  auto global_config = op->getGlobalConfig();

  std::vector<std::string> label_cols;
  for (const auto& l : global_config->global_label_config.labels) {
    label_cols.emplace_back(escapeString(l));
  }

  for (const auto& l : request->metric->label_config.labels) {
    label_cols.emplace_back(escapeString(l));
  }

  std::map<std::string, FetchStorageOp::FetchResponse> resp_map;

  {
    std::vector<std::string> cols;
    cols.insert(cols.end(), label_cols.begin(), label_cols.end());
    cols.emplace_back("value");

    auto qry = StringUtil::format(
        "SELECT $0 FROM $1;",
        StringUtil::join(cols, ", "),
        escapeString(request->metric->metric_id + ":last"));

    std::list<std::vector<std::string>> rows;
    auto rc = executeQuery(qry, &rows);
    if (!rc.isSuccess()) {
      return rc;
    }

    for (const auto& r : rows) {
      if (r.size() != cols.size()) {
        return ReturnCode::error("ERUNTIME", "sqlite error: invalid result");
      }

      FetchStorageOp::FetchResponse resp;
      resp.request = request;
      resp.label.labels = label_cols;
      resp.last_value = r[r.size() - 1];
      for (size_t i = 0; i < label_cols.size(); ++i) {
        resp.label.values.emplace_back(r[i]);
      }

      auto resp_key = StringUtil::join(resp.label.values, "\x1e");
      resp_map.emplace(resp_key, std::move(resp));
    }
  }

  {
    std::vector<std::string> cols;
    cols.emplace_back("time");
    cols.emplace_back("value");
    cols.insert(cols.end(), label_cols.begin(), label_cols.end());

    auto qry = StringUtil::format(
        "SELECT $0 FROM $1;",
        StringUtil::join(cols, ", "),
        escapeString(request->metric->metric_id + ":history"));

    std::list<std::vector<std::string>> rows;
    auto rc = executeQuery(qry, &rows);
    if (!rc.isSuccess()) {
      return rc;
    }

    for (const auto& r : rows) {
      if (r.size() != cols.size()) {
        return ReturnCode::error("ERUNTIME", "sqlite error: invalid result");
      }

      std::vector<std::string> label;
      for (size_t i = 0; i < label_cols.size(); ++i) {
        label.emplace_back(r[i + (cols.size() - label_cols.size())]);
      }

      auto resp_iter = resp_map.find(StringUtil::join(label, "\x1e"));
      if (resp_iter == resp_map.end()) {
        continue;
      }

      uint64_t timestamp;
      try {
        timestamp = std::stoull(r[0]);
      } catch (...) {
        return ReturnCode::error("ERUNTIME", "sqlite error: invalid result");
      }

      auto& resp_ts = resp_iter->second.history;
      size_t pos = std::upper_bound(
          resp_ts.timestamps.begin(),
          resp_ts.timestamps.end(),
          timestamp) - resp_ts.timestamps.begin();

      resp_ts.timestamps.insert(resp_ts.timestamps.begin() + pos, timestamp);
      resp_ts.values.insert(resp_ts.values.begin() + pos, r[1]);
    }
  }

  for (auto& r : resp_map) {
    op->addResponse(std::move(r.second));
  }

  return ReturnCode::success();
}

std::string SQLiteBackend::escapeString(std::string str) const {
  StringUtil::replaceAll(&str, "\"", "\\\"");
  return "\"" + str + "\"";
}

} // namespace sqlite_backend
} // namespace fnordmetric

