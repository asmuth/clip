/**
 * Copyright (c) 2016 zScale Technology GmbH <legal@zscale.io>
 * Authors:
 *   - Paul Asmuth <paul@zscale.io>
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Affero General Public License ("the license") as
 * published by the Free Software Foundation, either version 3 of the License,
 * or any later version.
 *
 * In accordance with Section 7(e) of the license, the licensing of the Program
 * under the license does not imply a trademark license. Therefore any rights,
 * title and interest in our trademarks remain entirely with us.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the license for more details.
 *
 * You can be released from the requirements of the license by purchasing a
 * commercial license. Buying such a license is mandatory as soon as you develop
 * commercial activities involving this program without disclosing the source
 * code of your own applications
 */
#include "fnordmetric/transport/http/VFSFileServlet.h"
#include "fnordmetric/util/io/fileutil.h"

namespace fnordmetric {
namespace http {

VFSFileServlet::VFSFileServlet(
    const String& base_path,
    VFS* vfs) :
    base_path_(base_path),
    vfs_(vfs) {}

void VFSFileServlet::handleHTTPRequest(
    http::HTTPRequest* req,
    http::HTTPResponse* res) {
  URI uri(req->uri());
  URI::ParamList params = uri.queryParams();

  res->addHeader("Access-Control-Allow-Origin", "*");

  std::string file_path;
  if (!URI::getParam(params, "file", &file_path)) {
    res->addBody("error: missing ?file=... parameter");
    res->setStatus(http::kStatusBadRequest);
    return;
  }

  if (uri.path() == base_path_ + "/get") {
    auto file = vfs_->openFile(file_path);
    res->setStatus(http::kStatusOK);
    res->addHeader("Content-Type", contentTypeFromFilename(file_path));
    res->addBody(file->data(), file->size());
    return;
  }

  if (uri.path() == base_path_ + "/size") {
    auto file = vfs_->openFile(file_path);
    res->setStatus(http::kStatusOK);
    res->addBody(StringUtil::toString(file->size()));
    return;
  }

  res->setStatus(http::kStatusNotFound);
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
