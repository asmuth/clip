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
    FORMAT_CSV
  };

  /**
   * Create a new query service
   */
  QueryService();

  /**
   * Execute a query. This may raise an exception.
   */
  void executeQuery(
      util::InputStream* query,
      kFormat output_format,
      util::OutputStream* output_stream);

  /**
   * Register a query backend
   */
  void registerBackend();

protected:

};

}
}
#endif
