/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_HTTP_VFSFILESERVLET_H
#define _FNORD_HTTP_VFSFILESERVLET_H
#include "fnord-base/VFS.h"
#include "fnord-http/httpservice.h"

namespace fnord {
namespace http {

class VFSFileServlet : public fnord::http::HTTPService {
public:

  VFSFileServlet(const String& base_path, VFS* vfs);

  void handleHTTPRequest(
      fnord::http::HTTPRequest* req,
      fnord::http::HTTPResponse* res);

protected:
  String contentTypeFromFilename(const String& filename) const;

  String base_path_;
  VFS* vfs_;
};

}
}
#endif
