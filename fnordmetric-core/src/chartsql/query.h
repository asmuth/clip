/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */

#ifndef _FNORDMETRIC_QUERY_H
#define _FNORDMETRIC_QUERY_H
#include <stdlib.h>
#include <string>
#include <vector>
#include <memory>
#include <fnordmetric/sql/runtime/runtime.h>
#include <fnordmetric/sql/runtime/queryplannode.h>
#include <fnordmetric/chartsql/drawstatement.h>
#include <fnord/chart/canvas.h>

namespace fnordmetric {
namespace query {
class TableRepository;
class DrawStatement;
class ASTNode;
class ResultList;

class Query {
public:

  explicit Query(
      const char* query_string,
      size_t query_string_len,
      Runtime* repo);

  explicit Query(const std::string& query_string, Runtime* runtime);

  explicit Query(
      const std::string& query_string,
      Runtime* runtime,
      std::unique_ptr<TableRepository> table_repo);

  Query(const Query& copy) = delete;
  Query& operator=(const Query& copy) = delete;
  Query(Query&& move);

  /**
   * Execute the query. This may raise an exception.
   */
  void execute();

  /**
   * Get the number of result lists
   */
  size_t getNumResultLists() const;

  /**
   * Get the nth result list of the query. Raises an exception if no such
   * result list exists
   *
   * The returned pointer is owned by the query instance and must not be freed
   * by the caller!
   *
   * @param index the requested result list index
   */
  ResultList* getResultList(size_t index) const;

  /**
   * Get the number of charts
   */
  size_t getNumCharts() const;

  /**
   * Get the nth chart result of the query. Raises an exception if no such
   * chart exists
   *
   * The returned pointer is owned by the query instance and must not be freed
   * by the caller!
   *
   * @param index the requested chart index
   */
  fnord::chart::Canvas* getChart(size_t index) const;

protected:
  Runtime* runtime_;
  std::unique_ptr<TableRepository> table_repo_;
  QueryPlan query_plan_;

  std::vector<std::pair<std::unique_ptr<QueryPlanNode>, DrawStatement*>>
      statements_;
  std::vector<std::vector<std::unique_ptr<DrawStatement>>> draw_statements_;
  std::vector<std::unique_ptr<ResultList>> results_;
  std::vector<std::unique_ptr<fnord::chart::Canvas>> charts_;
};

}
}
#endif
