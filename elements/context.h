/**
 * This file is part of the "signaltk" project
 *   Copyright (c) 2018 Paul Asmuth
 *
 * libstx is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#pragma once
#include <stack>
#include "graphics/layer.h"

namespace signaltk {

struct context;

context* context_create_image(uint32_t width, uint32_t height);
void context_reset(context* ctx);
void context_destroy(context* ctx);

uint32_t context_width(const context* ctx);
uint32_t context_height(const context* ctx);
Layer* context_frame(context* ctx);

Status element_config(context* ctx, void** elem);

Status element_config(const context* ctx, const void** elem);

Status element_add(
    context* ctx,
    void* config,
    std::function<void(void* config)> config_destroy);

} // namespace signaltk

#include "context_impl.h"
