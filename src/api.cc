/**
 * This file is part of the "clip" project
 *   Copyright (c) 2018 Paul Asmuth
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "api.h"
#include "environment.h"
#include "eval.h"

#include <iostream>
#include <string.h>

using namespace std::placeholders;
using namespace clip;

struct clip_s {
  OutputFormat format;
  std::string buffer;
  ReturnCode error;
};

clip_t* clip_init() {
  auto ctx = std::make_unique<clip_t>();
  ctx->format = OutputFormat::SVG;
  return ctx.release();
}

void clip_destroy(clip_t* ctx) {
  delete ctx;
}

const char* clip_get_error(const clip_t* ctx) {
  return ctx->error.message.c_str();
}

void clip_set_error(clip_t* ctx, const ReturnCode& err) {
  ctx->error = err;
}

int clip_eval(clip_t* ctx, const char* expr) {
  Environment env;
  auto rc = clip::eval(env, expr, ctx->format, &ctx->buffer);
  clip_set_error(ctx, rc);
  return rc;
}

void clip_get_result(clip_t* ctx, const void** data, size_t* data_len) {
  *data = ctx->buffer.data();
  *data_len = ctx->buffer.size();
}

int clip_set_output_format(clip_t* ctx, const char* path, const char* fmt) {
  std::string format = fmt;

  if (format == "svg") {
    ctx->format = OutputFormat::SVG;
    return OK;
  }

  if (format == "png") {
    ctx->format = OutputFormat::PNG;
    return OK;
  }

  clip_set_error(ctx, errorf(ERROR, "invalid output format: {}", format));
  return ERROR;
}

