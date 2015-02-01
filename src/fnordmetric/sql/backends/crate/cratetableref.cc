/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnordmetric/environment.h>
#include <fnordmetric/sql/backends/crate/cratetableref.h>
#include <fnordmetric/sql/runtime/tablescan.h>
#include <inttypes.h>
#include <sstream>

using fnord::metric_service::crate_backend::CrateConnection;

namespace fnordmetric {
namespace query {
namespace crate_backend {

CrateTableRef::CrateTableRef(
    CrateConnection conn,
    const std::string& table_name) :
    conn_(conn),
    table_name_(table_name) {
  table_columns_ = conn.describeTable(table_name_);
}

std::vector<std::string> CrateTableRef::columns() {
  std::vector<std::string> cols;
  return cols;
}

int CrateTableRef::getColumnIndex(const std::string& name) {
  for (int i = 0; i < columns_.size(); ++i) {
    if (*columns_[i][0] == name) {
      return i;
    }
  }

  for (int i = 0; i < table_columns_.size(); ++i) {
    if (table_columns_[i][0] == name) {
      std::vector<std::string*> col = {
          &table_columns_[i][0],
          &table_columns_[i][1]};

      columns_.emplace_back(col);
      return columns_.size() - 1;
    }
  }

  return -1;
}

std::string CrateTableRef::getColumnName(int index) {
  if (index >= columns_.size()) {
    RAISE(kIndexError, "no such column");
  }

  return *columns_[index][0];
}

size_t CrateTableRef::count() const {
  std::string count_q = "select count(*) from \"";
  count_q.append(table_name_);
  count_q.append("\"");
  size_t count = 0;
  //conn_.executeQuery(count_q,
  //    [this, &count] (const rapidjson::Value& row) -> bool {
  //      if(row.Size() < 1 || !row[0].IsUint64()){
  //          return false;
  //      }
  //      count = row[0].GetUint64();
  //      return true;
  //    });
  return count;
}

void CrateTableRef::executeScan(TableScan* scan) {
  std::string crate_query = "SELECT";

  for (int i = 0; i < columns_.size(); ++i) {
    crate_query.append(i == 0 ? " " : ",");
    crate_query.append("\"");
    crate_query.append(*columns_[i][0]);
    crate_query.append("\"");
  }

  crate_query.append(" FROM ");
  crate_query.append(table_name_);
  crate_query.append(" LIMIT ?");

  //std::vector<SValue> args;
  //fnordmetric::IntegerType c = count();
  //args.emplace_back(c);
  //std::string response = conn_.executeQuery(crate_query, args);
  //rapidjson::Document d;
  //d.Parse(response.c_str());
  //const rapidjson::Value &rows = d["rows"];

  //for(rapidjson::SizeType i = 0; i < rows.Size(); i++){
  //    std::vector<SValue> row_svals;
  //    for(rapidjson::SizeType j = 0; j < rows[i].Size(); j++){
  //        std::string type = *columns_[j][1];
  //        if(type == "timestamp"){
  //            uint64_t time = rows[i][j].GetUint64();
  //            fnordmetric::TimeType value = time * 1000;
  //            row_svals.emplace_back(value);
  //        } else if(type == "string"){
  //            row_svals.emplace_back(rows[i][j].GetString());
  //        } else if(type == "double" || type == "float"){
  //            row_svals.emplace_back(rows[i][j].GetDouble());
  //        } else if (type == "integer"){
  //            fnordmetric::IntegerType value;
  //            value = rows[i][j].GetInt();
  //            row_svals.emplace_back(value);
  //        } else if (type == "boolean"){
  //            row_svals.emplace_back(rows[i][j].GetBool());
  //        } else {
  //            env()->logger()->printf("WARNING", "UNKNOWN DATA TYPE");
  //        }
  //    }
  //    scan->nextRow(row_svals.data(), row_svals.size());
  //}
}

}
}
}
