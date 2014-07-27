/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_QUERY_H
#define _FNORDMETRIC_QUERY_H
#include <stdlib.h>
#include <string>
#include <vector>
#include <memory>
#include "../ui/canvas.h"

namespace fnordmetric {
namespace query {
class TableRepository;
class DrawStatement;
class ASTNode;
class Executable;
class ResultList;

class Query {
public:

  explicit Query(const char* query_string, TableRepository* repo);

  explicit Query(
      const char* query_string,
      size_t query_string_len,
      TableRepository* repo);

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
  ui::Canvas* getChart(size_t index) const;

protected:
  bool addStatement(ASTNode* statement, TableRepository* repo);
  void importTable(ASTNode* statement, TableRepository* repo) const;

  //Drawable* makeDrawable(query::DrawStatement* stmt);
  std::vector<std::unique_ptr<Executable>> statements_;
  std::vector<std::unique_ptr<ResultList>> results_;
  std::vector<std::unique_ptr<ui::Canvas>> charts_;
};

}
}
#endif
