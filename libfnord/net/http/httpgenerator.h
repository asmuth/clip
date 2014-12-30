/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef __FNORDMETRIC_HTTPGENERATOR_H
#define __FNORDMETRIC_HTTPGENERATOR_H
#include <vector>
#include <string>
#include <utility>
#include <fnord/io/outputstream.h>
#include <fnord/net/http/httpresponse.h>
#include <fnord/net/http/httprequest.h>

using fnord::io::OutputStream;

namespace fnord {
namespace http {

class HTTPGenerator {
public:
  static void generate(const HTTPRequest& req, OutputStream* os);
  static void generate(const HTTPResponse& res, OutputStream* os);
};

}
}
#endif
