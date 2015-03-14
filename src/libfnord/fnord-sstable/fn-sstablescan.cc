/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <stdlib.h>
#include <unistd.h>
#include "fnord-base/application.h"
#include "fnord-base/cli/flagparser.h"
#include "fnord-base/logging.h"
#include "fnord-base/inspect.h"
#include "fnord-sstable/sstablereader.h"
#include "fnord-sstable/SSTableScan.h"

using namespace fnord;

int main(int argc, const char** argv) {
  fnord::Application::init();
  fnord::Application::logToStderr();

  fnord::cli::FlagParser flags;

  flags.defineFlag(
      "file",
      fnord::cli::FlagParser::T_STRING,
      true,
      "f",
      NULL,
      "input sstable file",
      "<file>");

  flags.defineFlag(
      "limit",
      fnord::cli::FlagParser::T_INTEGER,
      false,
      NULL,
      NULL,
      "limit",
      "<num>");

  flags.defineFlag(
      "offset",
      fnord::cli::FlagParser::T_INTEGER,
      false,
      NULL,
      NULL,
      "offset",
      "<num>");

  flags.defineFlag(
      "order_by",
      fnord::cli::FlagParser::T_STRING,
      false,
      NULL,
      NULL,
      "order by",
      "<column>");

  flags.defineFlag(
      "order_fn",
      fnord::cli::FlagParser::T_STRING,
      false,
      NULL,
      "STRASC",
      "one of: STRASC, STRDSC, NUMASC, NUMDSC",
      "<fn>");

  flags.defineFlag(
      "loglevel",
      fnord::cli::FlagParser::T_STRING,
      false,
      NULL,
      "INFO",
      "loglevel",
      "<level>");

  flags.parseArgv(argc, argv);

  Logger::get()->setMinimumLogLevel(
      strToLogLevel(flags.getString("loglevel")));

  /* open input sstable */
  auto input_file = flags.getString("file");
  sstable::SSTableReader reader(File::openFile(input_file, File::O_READ));
  if (reader.bodySize() == 0) {
    fnord::logWarning("fnord.sstablescan", "sstable is unfinished");
  }

  sstable::SSTableColumnSchema schema;
  schema.loadIndex(&reader);

  /* set up scan */
  sstable::SSTableScan scan(&schema);
  if (flags.isSet("limit")) {
    scan.setLimit(flags.getInt("limit"));
  }

  if (flags.isSet("offset")) {
    scan.setOffset(flags.getInt("offset"));
  }

  if (flags.isSet("order_by")) {
    scan.setOrderBy(flags.getString("order_by"), flags.getString("order_fn"));
  }

  /* execute scan */
  auto headers = scan.columnNames();
  fnord::iputs("$0", StringUtil::join(headers, ";"));

  auto cursor = reader.getCursor();
  scan.execute(cursor.get(), [] (const Vector<String> row) {
    fnord::iputs("$0", StringUtil::join(row, ";"));
  });

  return 0;
}

