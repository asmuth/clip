/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_SSTABLE_SSTABLESERVLET_H
#define _FNORD_SSTABLE_SSTABLESERVLET_H
#include "fnord-base/VFS.h"
#include "fnord-http/httpservice.h"
#include "fnord-json/json.h"

namespace fnord {
namespace sstable {

class SSTableServlet : public fnord::http::HTTPService {
public:
  enum class ResponseFormat {
    JSON,
    CSV
  };

  SSTableServlet(const String& base_path, VFS* vfs);

  void handleHTTPRequest(
      fnord::http::HTTPRequest* req,
      fnord::http::HTTPResponse* res);

protected:

  void scan(
      fnord::http::HTTPRequest* req,
      fnord::http::HTTPResponse* res,
      const URI& uri);

  ResponseFormat formatFromString(const String& format);

  String base_path_;
  VFS* vfs_;
};

}
}
#endif
