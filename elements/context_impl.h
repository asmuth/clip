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
Status element_config_as(context* ctx, T** elem) {
  void* e;
  if (auto rc = element_config(ctx, &e); rc) {
    return rc;
  }

  *elem = static_cast<T*>(e);
  return OK;
}

template <typename T>
Status element_config_as(const context* ctx, T const** elem) {
  const void* e;
  if (auto rc = element_config(ctx, &e); rc) {
    return rc;
  }

  *elem = static_cast<const T*>(e);
  return OK;
}

template <typename T>
Status element_add(context* ctx) {
  return element_add(
      ctx,
      new T(),
      [] (void* c) { delete static_cast<T*>(c); });
}

} // namespace signaltk

