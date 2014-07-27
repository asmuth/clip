/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include <fnordmetric/query/executable.h>
#include <fnordmetric/query/query.h>
#include <fnordmetric/query/queryservice.h>
#include <fnordmetric/query/resultlist.h>
#include <fnordmetric/query/tablerepository.h>
#include <fnordmetric/util/inputstream.h>
#include <fnordmetric/ui/svgtarget.h>

namespace fnordmetric {
namespace query {

QueryService::QueryService() {

}

void QueryService::executeQuery(
    util::InputStream* input_stream,
    kFormat output_format,
    util::OutputStream* output_stream) const {
  std::string query_string;
  input_stream->readUntilEOF(&query_string);

  TableRepository repo;
  Query query(query_string.c_str(), query_string.size(), &repo);
  query.execute();

  switch (output_format) {

    case FORMAT_SVG: {
      ui::SVGTarget target;
      renderCharts(&query, &target);
      break;
    }

  }
}

void QueryService::registerBackend() {
}

void QueryService::renderCharts(Query* query, ui::RenderTarget* target) const {
  for (int i = 0; i < query->getNumCharts(); ++i) {
    query->getChart(i)->render(target);
  }
}

}
}
