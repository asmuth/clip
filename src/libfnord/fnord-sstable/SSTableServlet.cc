/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "fnord-sstable/SSTableServlet.h"
#include "fnord-sstable/sstablereader.h"
#include "fnord-sstable/SSTableScan.h"
#include "fnord-base/io/fileutil.h"

namespace fnord {
namespace sstable {

SSTableServlet::SSTableServlet(
    const String& base_path,
    VFS* vfs) :
    base_path_(base_path),
    vfs_(vfs) {}

void SSTableServlet::handleHTTPRequest(
    fnord::http::HTTPRequest* req,
    fnord::http::HTTPResponse* res) {
  URI uri(req->uri());

  res->addHeader("Access-Control-Allow-Origin", "*");

  if (uri.path() == base_path_ + "/scan") {
    try{
      scan(req, res, uri);
    } catch (const Exception& e) {
      res->setStatus(http::kStatusInternalServerError);
      res->addBody(StringUtil::format("error: $0", e.getMessage()));
    }
    return;
  }

  res->setStatus(fnord::http::kStatusNotFound);
  res->addBody("not found");
}

void SSTableServlet::scan(
    fnord::http::HTTPRequest* req,
    fnord::http::HTTPResponse* res,
    const URI& uri) {
  fnord::URI::ParamList params = uri.queryParams();

  auto format = ResponseFormat::CSV;
  std::string format_param;
  if (fnord::URI::getParam(params, "format", &format_param)) {
    format = formatFromString(format_param);
  }

  std::string file_path;
  if (!fnord::URI::getParam(params, "file", &file_path)) {
    res->addBody("error: missing ?file=... parameter");
    res->setStatus(http::kStatusBadRequest);
    return;
  }

  sstable::SSTableReader reader(vfs_->openFile(file_path));
  if (reader.bodySize() == 0) {
    res->addBody("sstable is unfinished (body_size == 0)");
    res->setStatus(http::kStatusInternalServerError);
    return;
  }

  sstable::SSTableColumnSchema schema;
  schema.loadIndex(&reader);

  sstable::SSTableScan sstable_scan(&schema);

  String limit_str;
  if (fnord::URI::getParam(params, "limit", &limit_str)) {
    sstable_scan.setLimit(std::stoul(limit_str));
  }

  String key_prefix_str;
  if (fnord::URI::getParam(params, "key_prefix", &key_prefix_str)) {
    sstable_scan.setKeyPrefix(key_prefix_str);
  }

  String key_regex_str;
  if (fnord::URI::getParam(params, "key_regex", &key_regex_str)) {
    sstable_scan.setKeyFilterRegex(key_regex_str);
  }

  Set<String> key_match_set;
  for (const auto& p : params) {
    if (p.first == "key_match") {
      key_match_set.emplace(p.second);
    }
  }
  if (key_match_set.size() > 0) {
    sstable_scan.setKeyExactMatchFilter(key_match_set);
  }

  String offset_str;
  if (fnord::URI::getParam(params, "offset", &offset_str)) {
    sstable_scan.setOffset(std::stoul(offset_str));
  }

  String order_by;
  String order_fn = "STRASC";
  if (fnord::URI::getParam(params, "order_by", &order_by)) {
    fnord::URI::getParam(params, "order_fn", &order_fn);
    sstable_scan.setOrderBy(order_by, order_fn);
  }

  Buffer buf;
  json::JSONOutputStream json(
      static_cast<std::shared_ptr<OutputStream>>(
          BufferOutputStream::fromBuffer(&buf)));

  auto headers = sstable_scan.columnNames();

  switch (format) {
    case ResponseFormat::CSV:
      buf.append(StringUtil::join(headers, ";"));
      buf.append("\n");
      break;
    case ResponseFormat::JSON:
      json.beginArray();
      break;
  }

  auto cursor = reader.getCursor();
  int n = 0;
  sstable_scan.execute(
      cursor.get(),
      [&buf, format, &n, &json, &headers] (const Vector<String> row) {
    switch (format) {
      case ResponseFormat::CSV:
        buf.append(StringUtil::join(row, ";"));
        buf.append("\n");
        break;
      case ResponseFormat::JSON:
        if (n++ > 0) json.addComma();
        json.beginObject();
        for (int i = 0; i < row.size(); ++i) {
          if (i > 0) json.addComma();
          json.addString(headers[i]);
          json.addColon();
          json.addString(row[i]);
        }
        json.endObject();
        break;
    }
  });

  switch (format) {
    case ResponseFormat::CSV:
      res->addHeader("Content-Type", "text/csv; charset=utf-8");
      break;
    case ResponseFormat::JSON:
      json.endArray();
      res->addHeader("Content-Type", "application/json; charset=utf-8");
      break;
  }

  res->setStatus(fnord::http::kStatusOK);
  res->addBody(buf);
}

SSTableServlet::ResponseFormat SSTableServlet::formatFromString(
    const String& format) {
  if (format == "csv") {
    return ResponseFormat::CSV;
  }

  if (format == "json") {
    return ResponseFormat::JSON;
  }

  RAISEF(kIllegalArgumentError, "invalid format: $0", format);
}

}
}
