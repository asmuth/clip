/**
 * This file is part of the "signaltk" project
 *   Copyright (c) 2018 Paul Asmuth
 *
 * libstx is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "context.h"

namespace signaltk {

Status stack_add(
    Context* ctx,
    std::unique_ptr<ElementConfig> config) {
  ctx->elements.emplace(ElementRef {
    .config = std::move(config)
  });

  return Status::OK;
}

} // namespace signaltk

