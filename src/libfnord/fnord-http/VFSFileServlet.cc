/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "fnord-http/VFSFileServlet.h"
#include "fnord-base/io/fileutil.h"

namespace fnord {
namespace http {

VFSFileServlet::VFSFileServlet(
    const String& base_path,
    VFS* vfs) :
    base_path_(base_path),
    vfs_(vfs) {}

void VFSFileServlet::handleHTTPRequest(
    fnord::http::HTTPRequest* req,
    fnord::http::HTTPResponse* res) {
  URI uri(req->uri());
  fnord::URI::ParamList params = uri.queryParams();

  res->addHeader("Access-Control-Allow-Origin", "*");

  std::string file_path;
  if (!fnord::URI::getParam(params, "file", &file_path)) {
    res->addBody("error: missing ?file=... parameter");
    res->setStatus(http::kStatusBadRequest);
    return;
  }

  if (uri.path() == base_path_ + "/get") {
    auto file = vfs_->openFile(file_path);
    res->setStatus(fnord::http::kStatusOK);
    res->addHeader("Content-Type", contentTypeFromFilename(file_path));
    res->addBody(file->data(), file->size());
    return;
  }

  if (uri.path() == base_path_ + "/size") {
    auto file = vfs_->openFile(file_path);
    res->setStatus(fnord::http::kStatusOK);
    res->addBody(StringUtil::toString(file->size()));
    return;
  }

  res->setStatus(fnord::http::kStatusNotFound);
  res->addBody("not found");
}

String VFSFileServlet::contentTypeFromFilename(const String& filename) const {
  if (StringUtil::endsWith(filename, ".csv")) {
    return "text/csv; charset=utf-8";
  }

  if (StringUtil::endsWith(filename, ".html")) {
    return "text/html; charset=utf-8";
  }

  if (StringUtil::endsWith(filename, ".json")) {
    return "application/json; charset=utf-8";
  }

  if (StringUtil::endsWith(filename, ".txt")) {
    return "text/plain; charset=utf-8";
  }

  return "application/octet-stream";
}

}
}
