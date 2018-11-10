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
Status Context::element_config(T** elem) {
  if (elements.empty()) {
    return Status::ERROR_INVALID_ELEM;
  }

  *elem = static_cast<T*>(elements.top()->config);
  return OK;
}

template <typename T>
Status Context::element_config(T const** elem) const {
  if (elements.empty()) {
    return Status::ERROR_INVALID_ELEM;
  }

  *elem = static_cast<const T*>(elements.top()->config);
  return OK;
}

template <typename T>
Status Context::element_add() {
  auto e = std::make_unique<Element>();
  e->config = new T();
  e->destroy = [] (Element* e) { delete static_cast<T*>(e->config); };

  elements.emplace(std::move(e));
  return OK;
}

} // namespace signaltk

