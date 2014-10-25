/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnordmetric/query/query.h>
#include <fnordmetric/query/queryservice.h>
#include <fnordmetric/sql/runtime/queryplannode.h>
#include <fnordmetric/sql/runtime/resultlist.h>
#include <fnordmetric/sql/runtime/tablerepository.h>
#include <fnordmetric/ui/svgtarget.h>
#include <fnordmetric/util/inputstream.h>
#include <fnordmetric/util/jsonoutputstream.h>
#include <fnordmetric/sql/backends/csv/csvbackend.h>
#include <fnordmetric/sql/backends/mysql/mysqlbackend.h>

namespace fnordmetric {
namespace query {

QueryService::QueryService() {
}

void QueryService::executeQuery(
    util::InputStream* input_stream,
    kFormat output_format,
    util::OutputStream* output_stream) {
  std::string query_string;
  input_stream->readUntilEOF(&query_string);

  try {
    Query query(query_string, &runtime_);
    query.execute();

    switch (output_format) {

      case FORMAT_SVG: {
        ui::SVGTarget target(output_stream);
        renderCharts(&query, &target);
        break;
      }

      case FORMAT_JSON: {
        //util::JSONOutputStream target(output_stream);
        //renderJSON(&query, &target);
        break;
      }

      case FORMAT_TABLE: {
        renderTables(&query, output_stream);
        break;
      }

      default:
        RAISE(kRuntimeError, "can't handle this output format");

    }
  } catch (util::RuntimeException e) {
    e.appendMessage(" while executing query: %s", query_string.c_str());
    throw e;
  }
}

void QueryService::registerBackend(std::unique_ptr<Backend>&& backend) {
  runtime_.addBackend(std::move(backend));
}

void QueryService::renderCharts(Query* query, ui::RenderTarget* target) const {
  for (int i = 0; i < query->getNumCharts(); ++i) {
    query->getChart(i)->render(target);
  }
}

void QueryService::renderJSON(Query* query, util::JSONOutputStream* target)
    const {
  target->beginObject();

  if (query->getNumResultLists() > 0) {
    target->addObjectEntry("tables");
    target->beginArray();
    for (int i = 0; i < query->getNumResultLists(); ++i) {
      const auto result_list = query->getResultList(i);
      target->beginObject();

      target->addObjectEntry("columns");
      target->beginArray();
      for (const auto column : result_list->getColumns()) {
        target->addString(column);
        if (column != result_list->getColumns().back()) {
          target->addComma();
        }
      }
      target->endArray();
      target->addComma();

      target->addObjectEntry("rows");
      target->beginArray();
      for (int j = 0; j < result_list->getNumRows(); ++j) {
        const auto& row = result_list->getRow(j);

        target->beginArray();
        for (const auto& col : row) {
          target->addString(col);
          if (col != row.back()) {
            target->addComma();
          }
        }
        target->endArray();

        if (j < result_list->getNumRows() - 1) {
          target->addComma();
        }
      }
      target->endArray();
      target->endObject();

      if (i < query->getNumResultLists() - 1) {
        target->addComma();
      }
    }
    target->endArray();
    target->addComma();
  }

  if (query->getNumCharts() > 0) {
    target->addObjectEntry("charts");
    target->beginArray();

    for (int i = 0; i < query->getNumCharts(); ++i) {
      std::string svg_data;
      auto string_stream = util::StringOutputStream::fromString(&svg_data);
      ui::SVGTarget svg_target(string_stream.get());
      query->getChart(i)->render(&svg_target);

      target->beginObject();
      target->addObjectEntry("svg");
      target->addString(svg_data);
      target->endObject();

      if (i < query->getNumCharts() - 1) {
        target->addComma();
      }
    }

    target->endArray();
    target->addComma();
  }

  target->addObjectEntry("status");
  target->addString("success");

  target->endObject();
}

void QueryService::renderTables(Query* query, util::OutputStream* out) const {
  for (int i = 0; i < query->getNumResultLists(); ++i) {
    const auto result_list = query->getResultList(i);
    result_list->debugPrint();
  }
}

}
}
