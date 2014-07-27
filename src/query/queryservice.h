/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_QUERYSERVICE_H
#define _FNORDMETRIC_QUERYSERVICE_H
#include <fnordmetric/query/query.h>

namespace fnordmetric {
namespace ui {
  class RenderTarget;
}
namespace util {
  class InputStream;
  class OutputStream;
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
    FORMAT_CSV
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

};

}
}
#endif
