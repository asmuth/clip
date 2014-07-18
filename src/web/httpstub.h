/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
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
