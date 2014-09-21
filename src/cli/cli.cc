/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <vector>
#include <string>
#include <fnordmetric/environment.h>
#include <fnordmetric/cli/cli.h>
#include <fnordmetric/cli/flagparser.h>
#include <fnordmetric/ev/eventloop.h>
#include <fnordmetric/ev/acceptor.h>
#include <fnordmetric/http/httpserver.h>
#include <fnordmetric/util/exceptionhandler.h>
#include <fnordmetric/util/inputstream.h>
#include <fnordmetric/util/outputstream.h>
#include <fnordmetric/util/runtimeexception.h>
#include <fnordmetric/web/webinterface.h>
#include <fnordmetric/web/queryendpoint.h>

namespace fnordmetric {
namespace cli {

void CLI::parseArgs(Environment* env, const std::vector<std::string>& argv) {
  auto flags = env->flags();

  // Execute queries from the commandline:
  flags->defineFlag(
      "format",
      FlagParser::T_STRING,
      false,
      "f",
      "table",
      "The output format { svg, csv, table }",
      "<format>");

  flags->defineFlag(
      "output",
      FlagParser::T_STRING,
      false,
      "o",
      NULL,
      "Write output to a file",
      "<format>");

  // Start a user interface:
  flags->defineFlag(
      "repl",
      FlagParser::T_SWITCH,
      false,
      NULL,
      NULL,
      "Start an interactive readline shell",
      NULL);

  flags->defineFlag(
      "web",
      FlagParser::T_INTEGER,
      false,
      NULL,
      NULL,
      "Start a web interface on this port",
      "<port>");

  flags->defineFlag(
      "cgi",
      FlagParser::T_SWITCH,
      false,
      NULL,
      NULL,
      "Run as CGI script");

  flags->defineFlag(
      "verbose",
      FlagParser::T_SWITCH,
      false,
      NULL,
      NULL,
      "Be verbose");

  flags->defineFlag(
      "help",
      FlagParser::T_SWITCH,
      false,
      "h",
      NULL,
      "You are reading it...");

  flags->parseArgv(argv);
  env->setVerbose(flags->isSet("verbose"));
}


int CLI::executeSafely(Environment* env) {
  auto err_stream = util::OutputStream::getStderr();

  try {
    execute(env);
  } catch (const util::RuntimeException& e) {
    if (e.getTypeName() == "UsageError") {
      env->flags()->printUsage(err_stream.get());
      return 1;
    }

    if (env->verbose()) {
      env->logger()->exception("FATAL", "Fatal error", e);
    } else {
      auto msg = e.getMessage();
      err_stream->printf("[ERROR] ");
      err_stream->write(msg.c_str(), msg.size());
      err_stream->printf("\n");
    }

    return 1;
  }

  return 0;
}


void CLI::execute(Environment* env) {
  auto flags = env->flags();
  const auto& args = flags->getArgv();

  if (flags->isSet("web")) {
    fnordmetric::util::ThreadPool thread_pool(
        32,
        std::unique_ptr<util::ExceptionHandler>(
            new util::CatchAndPrintExceptionHandler(env->logger())));

    fnordmetric::ev::EventLoop ev_loop;
    fnordmetric::ev::Acceptor acceptor(&ev_loop);
    fnordmetric::http::ThreadedHTTPServer http(&thread_pool);
    http.addHandler(fnordmetric::web::WebInterface::getHandler());
    http.addHandler(fnordmetric::web::QueryEndpoint::getHandler());
    acceptor.listen(flags->getInt("web"), &http);
    ev_loop.loop();
  }

  /* open input stream */
  std::unique_ptr<util::InputStream> input;
  if (args.size() == 1) {
    if (args[0] == "-") {
      if (env->verbose()) {
        env->logger()->log("DEBUG", "input from stdin");
      }

      input = std::move(util::InputStream::getStdin());
    } else {
      if (env->verbose()) {
        env->logger()->log("DEBUG", "input file: " + args[0]);
      }

      input = std::move(util::FileInputStream::openFile(args[0]));
    }
  } else {
    RAISE(UsageError);
  }

  /* open output stream */
  std::unique_ptr<util::OutputStream> output;
  if (flags->isSet("output")) {
    auto output_file = flags->getString("output");

    if (env->verbose()) {
      env->logger()->log("DEBUG", "output file: " + output_file);
    }

    output = std::move(util::FileOutputStream::openFile(output_file));
  } else {
    if (env->verbose()) {
      env->logger()->log("DEBUG", "output to stout");
    }
    output = std::move(util::OutputStream::getStdout());
  }

  /* execute query */
  query::QueryService query_service;
  query_service.executeQuery(
      input.get(),
      getOutputFormat(env),
      output.get());
}

const query::QueryService::kFormat CLI::getOutputFormat(Environment* env) {
  auto flags = env->flags();

  if (!flags->isSet("format")) {
    return query::QueryService::FORMAT_TABLE;
  }

  auto format_str = flags->getString("format");

  if (format_str == "csv") {
    return query::QueryService::FORMAT_CSV;
  }

  if (format_str == "json") {
    return query::QueryService::FORMAT_JSON;
  }

  if (format_str == "svg") {
    return query::QueryService::FORMAT_SVG;
  }

  if (format_str == "table") {
    return query::QueryService::FORMAT_TABLE;
  }

  RAISE(
      util::RuntimeException,
      "invalid format: '%s', allowed formats are "
      "'svg', 'csv', 'json' and 'table'",
      format_str.c_str());
}

}
}
