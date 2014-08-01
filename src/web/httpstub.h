/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */

#ifndef _FNORDMETRIC_WEB_HTTPSTUB_H
#define _FNORDMETRIC_WEB_HTTPSTUB_H

namespace fnordmetric {
namespace web {

class HTTPStub {
public:

  class Request {
  public:

    virtual ~Request();

  };

  class Response {
  public:

    virtual ~Response();

  };

  virtual ~HTTPStub();
  virtual void onHTTPRequest(Request* request, Response* resp) = 0;
};

}
}
#endif
