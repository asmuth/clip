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
#include <fnordmetric/sql/query.h>

namespace fnordmetric {
namespace ui {
  class RenderTarget;
}
namespace util {
  class InputStream;
  class OutputStream;
  class JSONOutputStream;
}

namespace query {

/**
 * The query service is the default entry point for executing all queries. A
 * QueryService instance is thread safe.
 */
class QueryService {
public:
  enum kFormat {
    FORMAT_SVG,
    FORMAT_CSV,
    FORMAT_JSON
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
      util::InputStream* input_stream,
      kFormat output_format,
      util::OutputStream* output_stream) const;

  /**
   * Register a query backend
   */
  void registerBackend();

protected:

  void renderCharts(Query* query, ui::RenderTarget* target) const;
  void renderJSON(Query* query, util::JSONOutputStream* target) const;

};

}
}
#endif
