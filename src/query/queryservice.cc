/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
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

  try {
    TableRepository repo;
    Query query(query_string.c_str(), query_string.size(), &repo);
    query.execute();

    switch (output_format) {

      case FORMAT_SVG: {
        ui::SVGTarget target(output_stream);
        renderCharts(&query, &target);
        break;
      }

    }
  } catch (util::RuntimeException e) {
    e.appendMessage(" while executing query: %s", query_string.c_str());
    throw e;
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
