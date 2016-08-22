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
#pragma once
#include <fnordmetric/util/uri.h>
#include <fnordmetric/util/io/file.h>
#include <fnordmetric/transport/http/httpmessage.h>
#include "fnordmetric/transport/http/httprequest.h"
#include "fnordmetric/transport/http/httpresponse.h"
#include "fnordmetric/transport/http/httpstats.h"
#include "fnordmetric/transport/http/httpconnectionpool.h"
#include "fnordmetric/transport/http/httpclient.h"

namespace fnordmetric {
namespace http {

struct HTTPSSEEvent {
  String data;
  Option<String> name;
};

class HTTPSSEParser {
public:

  void onEvent(Function<void (const HTTPSSEEvent& ev)> fn);

  void parse(const char* data, size_t size);

protected:

  void parseEvent(const char* data, size_t size);

  Buffer buf_;
  Function<void (const HTTPSSEEvent& ev)> on_event_;
};

}
}
