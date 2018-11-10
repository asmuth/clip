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

namespace signaltk {

template <typename T>
Status stack_head_config(Context* ctx, T** head) {
  if (ctx->elements.empty()) {
    return Status::ERROR_INVALID_ELEM;
  }

  *head = static_cast<T*>(ctx->elements.top().config.get());
  return OK;
}

template <typename T>
Status stack_head_config(const Context& ctx, T const** head) {
  if (ctx.elements.empty()) {
    return Status::ERROR_INVALID_ELEM;
  }

  *head = static_cast<const T*>(ctx.elements.top().config.get());
  return OK;
}

} // namespace signaltk

