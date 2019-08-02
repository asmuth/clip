/**
 * This file is part of the "fviz" project
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
#include "core/environment.h"
#include "core/eval.h"

#include <iostream>
#include <string.h>

using namespace std::placeholders;
using namespace fviz;

struct fviz_s {
  OutputFormat format;
  std::string buffer;
  ReturnCode error;
};

fviz_t* fviz_init() {
  auto ctx = std::make_unique<fviz_t>();
  ctx->format = OutputFormat::SVG;
  return ctx.release();
}

void fviz_destroy(fviz_t* ctx) {
  delete ctx;
}

const char* fviz_get_error(const fviz_t* ctx) {
  return ctx->error.message.c_str();
}

void fviz_set_error(fviz_t* ctx, const ReturnCode& err) {
  ctx->error = err;
}

int fviz_eval(fviz_t* ctx, const char* expr) {
  Environment env;
  auto rc = fviz::eval(env, expr, ctx->format, &ctx->buffer);
  fviz_set_error(ctx, rc);
  return rc;
}

void fviz_get_result(fviz_t* ctx, const void** data, size_t* data_len) {
  *data = ctx->buffer.data();
  *data_len = ctx->buffer.size();
}

int fviz_set_output_format(fviz_t* ctx, const char* path, const char* fmt) {
  std::string format = fmt;

  if (format == "svg") {
    ctx->format == OutputFormat::SVG;
    return OK;
  }

  if (format == "png") {
    ctx->format == OutputFormat::PNG;
    return OK;
  }

  fviz_set_error(ctx, errorf(ERROR, "invalid output format: {}", format));
  return ERROR;
}

