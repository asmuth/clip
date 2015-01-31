/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_QUERYSERVICE_H
#define _FNORDMETRIC_QUERYSERVICE_H
#include <fnord/chart/rendertarget.h>
#include <fnord/io/inputstream.h>
#include <fnord-json/jsonoutputstream.h>
#include <fnordmetric/chartsql/query.h>
#include <fnordmetric/sql/runtime/defaultruntime.h>

namespace fnordmetric {
namespace query {

/**
 * The query service is the default entry point for executing all queries. A
 * QueryService instance should only be used from one thread at the same time.
 */
class QueryService {
public:
  enum kFormat {
    FORMAT_CSV,
    FORMAT_JSON,
    FORMAT_SVG,
    FORMAT_TABLE
  };

  /**
   * Create a new query service
   */
  QueryService();

  /**
   * Execute a query. This may raise an exception.
   *
   * @param input_stream The input stream to read the SQL query
   * @param output_format The output format
   * @param output_stream The output stream to write the results
   */
  void executeQuery(
      std::shared_ptr<fnord::io::InputStream> input_stream,
      kFormat output_format,
      std::shared_ptr<fnord::io::OutputStream> output_stream);

  /**
   * Execute a query. This may raise an exception.
   *
   * @param input_stream The input stream to read the SQL query
   * @param output_format The output format
   * @param output_stream The output stream to write the results
   */
  void executeQuery(
      std::shared_ptr<fnord::io::InputStream> input_stream,
      kFormat output_format,
      std::shared_ptr<fnord::io::OutputStream> output_stream,
      std::unique_ptr<TableRepository> table_repo,
      int width = -1,
      int height = -1);

  /**
   * Register a query backend
   */
  void registerBackend(std::unique_ptr<Backend>&& backend);

protected:

  void renderCharts(
      Query* query,
      fnord::chart::RenderTarget* target,
      int width,
      int height) const;

  void renderJSON(
      Query* query,
      fnord::json::JSONOutputStream* target,
      int width,
      int height) const;

  void renderTables(Query* query, fnord::io::OutputStream* out) const;

  DefaultRuntime runtime_;
};

}
}
#endif
