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
#pragma once

namespace fviz {

template <typename T>
ElementBuilder elem_builder(
    ElementConfigureAsFn<T> config_fn,
    ElementDrawAsFn<T> draw_fn) {
  return elem_builder(config_fn, ElementReflowAsFn<T>(nullptr), draw_fn);
}

template <typename T>
ElementBuilder elem_builder(
    ElementConfigureAsFn<T> config_fn,
    ElementReflowAsFn<T> reflow_fn,
    ElementDrawAsFn<T> draw_fn) {
  using namespace std::placeholders;

  return [=] (
      const plist::PropertyList& prop,
      const Environment& env,
      ElementRef* elem) -> ReturnCode {
    auto e = std::make_unique<ElementInstance<T>>();

    if (auto rc = config_fn(prop, env, &e->config); !rc) {
      return rc;
    }

    e->draw = bind(draw_fn, e->config, _1, _2);
    e->reflow = reflow_fn ? 
        bind(reflow_fn, e->config, _1, _2, _3, _4, _5) :
        ElementReflowFn(nullptr);

    *elem = std::move(e);
    return OK;
  };
}

} // namespace fviz

