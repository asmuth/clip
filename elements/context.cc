/**
 * This file is part of the "signaltk" project
 *   Copyright (c) 2018 Paul Asmuth
 *
 * libstx is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <memory>
#include "context.h"

namespace signaltk {

struct Element {
  ~Element();
  void* config;
  std::function<void (void* config)> config_destroy;
};

using ElementRef = std::unique_ptr<Element>;

Element::~Element() {
  if (config_destroy) {
    config_destroy(config);
  }
}

struct context {
  std::unique_ptr<Layer> frame;
  std::stack<Element*> element_stack;
  std::vector<ElementRef> elements;
};

context* context_create_image(uint32_t width, uint32_t height) {
  return new context {
    .frame = std::make_unique<Layer>(width, height),
  };
}

void context_reset(context* ctx) {
}

void context_destroy(context* ctx) {
  context_reset(ctx);
  delete ctx;
}

uint32_t context_width(const context* ctx) {
  return ctx->frame->width;
}

uint32_t context_height(const context* ctx) {
  return ctx->frame->height;
}

Layer* context_frame(context* ctx) {
  return ctx->frame.get();
}

Status element_config(const context* ctx, const void** elem) {
  if (ctx->elements.empty()) {
    return Status::ERROR_INVALID_ELEM;
  }

  *elem = ctx->element_stack.top()->config;
  return OK;
}

Status element_config(context* ctx, void** elem) {
  if (ctx->elements.empty()) {
    return Status::ERROR_INVALID_ELEM;
  }

  *elem = ctx->element_stack.top()->config;
  return OK;
}

Status element_add(
    context* ctx,
    void* config,
    std::function<void(void*)> config_destroy) {
  auto e = std::make_unique<Element>();
  e->config = config;
  e->config_destroy = config_destroy;
  ctx->element_stack.emplace(e.get());
  ctx->elements.emplace_back(std::move(e));
  return OK;
}

} // namespace signaltk

