/**
 * This file is part of the "plotfx" project
 *   Copyright (c) 2018 Paul Asmuth
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 * 
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * 
 * * Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "plotfx.h"
#include "document.h"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace plotfx;

plotfx_t* plotfx_init() {
  auto ctx = std::make_unique<Context>();
  return ctx.release();
}

int plotfx_configure(
    plotfx_t* ctx,
    const char* config) {
  auto& doc = static_cast<Context*>(ctx)->document;
  doc.reset(new Document());

  if (auto rc = document_load(config, doc.get()); !rc) {
    ctx_seterr(ctx, rc);
    return ERROR;
  }

  return OK;
}

int plotfx_configure_file(
    plotfx_t* ctx,
    const char* path) {
  std::stringstream config_buf;
  {
    std::ifstream config_file(path);
    if (!config_file.is_open()) {
      ctx_seterrf(ctx, StringUtil::format("file not found: $0", path));
      return ERROR;
    }

    config_buf << config_file.rdbuf();
  }

  return plotfx_configure(ctx, config_buf.str().c_str());
}

int plotfx_render_file(plotfx_t* ctx, const char* path, const char* format) {
  const auto& doc = static_cast<const Context*>(ctx)->document;
  if (!doc) {
    ctx_seterrf(ctx, "no configuration loaded");
    return ERROR;
  }

  if (auto rc = document_render(*doc, format, path); !rc) {
    ctx_seterr(ctx, rc);
    return ERROR;
  }

  return OK;
}

const char* plotfx_geterror(const plotfx_t* ctx) {
  return static_cast<const Context*>(ctx)->error.c_str();
}

