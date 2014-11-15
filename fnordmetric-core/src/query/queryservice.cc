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
#include <fnordmetric/query/query.h>
#include <fnordmetric/query/queryservice.h>
#include <fnordmetric/sql/runtime/queryplannode.h>
#include <fnordmetric/sql/runtime/resultlist.h>
#include <fnordmetric/sql/runtime/tablerepository.h>
#include <fnordmetric/ui/svgtarget.h>
#include <fnordmetric/util/inputstream.h>
#include <fnordmetric/util/jsonoutputstream.h>

namespace fnordmetric {
namespace query {

QueryService::QueryService() {}

void QueryService::executeQuery(
    std::shared_ptr<util::InputStream> input_stream,
    kFormat output_format,
    std::shared_ptr<util::OutputStream> output_stream) {
  std::unique_ptr<TableRepository> table_repo(new TableRepository());
  executeQuery(
      input_stream,
      output_format,
      output_stream,
      std::move(table_repo));
}

void QueryService::executeQuery(
    std::shared_ptr<util::InputStream> input_stream,
    kFormat output_format,
    std::shared_ptr<util::OutputStream> output_stream,
    std::unique_ptr<TableRepository> table_repo,
    int width /* = -1 */,
    int height /* = -1 */) {
  std::string query_string;
  input_stream->readUntilEOF(&query_string);

  if (fnordmetric::env()->verbose()) {
    fnordmetric::env()->logger()->printf(
        "DEBUG",
        "Executing ChartSQL query: %s",
        query_string.c_str());
  }

  try {
    Query query(query_string, &runtime_, std::move(table_repo));
    query.execute();

    switch (output_format) {
      case FORMAT_SVG: {
        ui::SVGTarget target(output_stream.get());
        renderCharts(&query, &target, width, height);
        break;
      }

      case FORMAT_JSON: {
        util::JSONOutputStream target(output_stream);
        renderJSON(&query, &target, width, height);
        break;
      }

      case FORMAT_TABLE: {
        renderTables(&query, output_stream.get());
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

void QueryService::renderCharts(
    Query* query,
    ui::RenderTarget* target,
    int width,
    int height) const {
  for (int i = 0; i < query->getNumCharts(); ++i) {
    auto chart = query->getChart(i);
    chart->setDimensions(width, height);
    chart->render(target);
  }
}

void QueryService::renderJSON(
    Query* query,
    util::JSONOutputStream* target,
    int width,
    int height) const {
  target->beginObject();

  if (query->getNumResultLists() > 0) {
    target->addObjectEntry("tables");
    target->beginArray();
    for (int i = 0; i < query->getNumResultLists(); ++i) {
      const auto result_list = query->getResultList(i);
      target->beginObject();

      target->addObjectEntry("columns");
      target->beginArray();

      auto columns = result_list->getColumns();
      for (int i = 0; i < columns.size(); ++i) {
        if (i > 0) {
          target->addComma();
        }
        target->addString(columns[i]);
      }
      target->endArray();
      target->addComma();

      target->addObjectEntry("rows");
      target->beginArray();
      for (int j = 0; j < result_list->getNumRows(); ++j) {
        const auto& row = result_list->getRow(j);

        target->beginArray();
        for (int i = 0; i < row.size(); ++i) {
          if (i > 0) {
            target->addComma();
          }

          target->addString(row[i]);
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
      auto chart = query->getChart(i);
      chart->setDimensions(width, height);
      chart->render(&svg_target);

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
