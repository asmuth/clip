/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <fnordmetric/http/httpinputstream.h>
#include <fnordmetric/util/runtimeexception.h>

namespace fnordmetric {
namespace http {

HTTPInputStream::HTTPInputStream(
    util::InputStream* input_stream) :
    input_(input_stream),
    state_(HTTP_STATE_METHOD) {}

HTTPInputStream::~HTTPInputStream() {
}

void HTTPInputStream::readStatusLine(
    std::string* method,
    std::string* url,
    std::string* version) {
  while (state_ == HTTP_STATE_METHOD) {
    readNextByte(method);
  }

  while (state_ == HTTP_STATE_URI) {
    readNextByte(url);
  }

  while (state_ == HTTP_STATE_VERSION) {
    readNextByte(version);
  }
}

void HTTPInputStream::readHeaders(
    std::vector<std::pair<std::string, std::string>>* target) {
  std::pair<std::string, std::string>* cur_header;

  while (state_ == HTTP_STATE_HKEY || state_ == HTTP_STATE_HVAL) {
    if (target->size() == 0 || 
        (state_ == HTTP_STATE_HKEY && target->back().second.size() > 0)) {
      target->emplace_back("", "");
    }

    while (state_ == HTTP_STATE_HKEY) {
      readNextByte(&target->back().first);
    }

    while (state_ == HTTP_STATE_HVAL) {
      readNextByte(&target->back().second);
    }
  }
}

void HTTPInputStream::readNextByte(std::string* target) {
  char byte;

  if (!input_->readNextByte(&byte)) {
    RAISE(util::RuntimeException, "unexpected EOF while reading HTTP header");
  }

  switch (byte) {

    case '\r':
      return;

    case ' ':
      switch (state_) {

        case HTTP_STATE_METHOD:
          state_ = HTTP_STATE_URI;
          return;

        case HTTP_STATE_URI:
          state_ = HTTP_STATE_VERSION;
          return;

        default:
          break;

      }
      break;

    case '\n':
      switch (state_) {

        case HTTP_STATE_METHOD:
          RAISE(util::RuntimeException, "invalid HTTP header");

        case HTTP_STATE_URI:
          RAISE(util::RuntimeException, "invalid HTTP header");

        case HTTP_STATE_VERSION:
          state_ = HTTP_STATE_HKEY;
          return;

        case HTTP_STATE_HVAL:
          state_ = HTTP_STATE_HKEY;
          return;

        case HTTP_STATE_HKEY:
          state_ = HTTP_STATE_BODY;
          return;

        default:
          break;

      }
      break;

    case ':':
      if (state_ == HTTP_STATE_HKEY) {
        state_ = HTTP_STATE_HVAL;
        return;
      }
      break;
  }

  if (byte == ' ' && target->size() == 0) {
    return;
  }

  *target += byte;
}

/*
int http_read_method(http_req_t* req, char* method, int len) {
  if (strncmp(method, "HEAD", len) == 0)
    req->method = HTTP_METHOD_HEAD;

  else if (strncmp(method, "GET", len) == 0)
    req->method = HTTP_METHOD_GET;

  else if (strncmp(method, "POST", len) == 0)
    req->method = HTTP_METHOD_POST;

  else
    return -1;

  req->state_ = HTTP_STATE_URI;
  return 0;
}

int http_read_uri(http_req_t* req, char* uri, int len) {
  char *end = uri + len;
  int  n;

  if (len >= (int) sizeof(req->uri))
    return -1;

  strncpy(req->uri, uri, len);
  req->uri[len] = 0;
  req->uri_len  = len;

  for (; *end == ' ' || *end == '/' ||*end == '?' ||
    *end == '&'; end--);

  for (n = 0; n < HTTP_URI_MAXARGS && uri < end; n++) {
    req->uri_argv[n] = uri;

    if (*uri == '&') *uri = '?';

    for (uri++; uri < end &&
      *uri != '/' && *uri != '?' && *uri != '&'; uri++);
  }

  req->uri_argv[n] = uri + 1;
  req->uri_argc = n;
  req->state_ = HTTP_STATE_VERSION;

  return 0;
}


int http_read_version(http_req_t* req, char* version, int len) {
  if (len < 8)
    return -1;

  if (strncmp(version + 5, "1.1", len - 5) == 0)
    req->keepalive = 1;

  else
    req->keepalive = 0;

  req->state_ = HTTP_STATE_HKEY;
  return 0;
}

void http_read_header(http_req_t* req, char* hkey, int hkey_len, char* hval, int hval_len) {
  //hkey[hkey_len] = 0; hval[hval_len] = 0;
  //printf("header: (%i) '%s' => (%i) '%s'\n", hkey_len, hkey, hval_len, hval);

  if (strncmp(hkey, "Connection", hkey_len) == 0)
    if (strncmp(hval, "Keep-Alive", hval_len) == 0)
      req->keepalive = 1;

}

*/


}
}
