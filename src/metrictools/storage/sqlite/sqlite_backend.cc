/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2017 Paul Asmuth <paul@asmuth.com>
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <metrictools/storage/sqlite/sqlite_backend.h>

namespace fnordmetric {
namespace sqlite_backend {

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

ReturnCode SQLiteBackend::performOperation(const InsertStorageOp& op) {
  auto global_config = op.getGlobalConfig();

  for (const auto& m : op.getMeasurements()) {
    {
      auto rc = createTables(global_config.get(), m.metric.get());
      if (!rc.isSuccess()) {
        return rc;
      }
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

ReturnCode SQLiteBackend::executeQuery(
    const std::string& query,
    std::list<std::vector<std::string>>* rows /* = nullptr */,
    std::vector<std::string>* columns /* = nullptr */) {
  char* err = nullptr;
  auto rc = sqlite3_exec(db_, query.c_str(), nullptr, 0, &err);
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

  std::vector<std::string> instance_cols;
  for (const auto& l : global_config->global_instance_path.labels) {
    instance_cols.emplace_back(StringUtil::format("\"$0\" string", l));
  }

  for (const auto& l : metric_config->instance_path.labels) {
    instance_cols.emplace_back(StringUtil::format("\"$0\" string", l));
  }

  {
    std::vector<std::string> cols;
    cols.insert(cols.end(), instance_cols.begin(), instance_cols.end());
    cols.emplace_back(value_col);

    auto qry = StringUtil::format(
        "CREATE TABLE IF NOT EXISTS \"$0\" ($1);",
        metric_config->metric_id + ":last",
        StringUtil::join(cols, ", "));

    auto rc = executeQuery(qry);
    if (!rc.isSuccess()) {
      return rc;
    }
  }

  {
    std::vector<std::string> cols;
    cols.emplace_back("time bigint");
    cols.insert(cols.end(), instance_cols.begin(), instance_cols.end());
    cols.emplace_back(value_col);

    auto qry = StringUtil::format(
        "CREATE TABLE IF NOT EXISTS \"$0\" ($1);",
        metric_config->metric_id + ":history",
        StringUtil::join(cols, ", "));

    auto rc = executeQuery(qry);
    if (!rc.isSuccess()) {
      return rc;
    }
  }

  return ReturnCode::success();
}

} // namespace sqlite_backend
} // namespace fnordmetric

