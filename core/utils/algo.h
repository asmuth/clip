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
#include <optional>
#include <vector>

namespace fviz {

template <typename T>
const T& at_or(const std::vector<T>& vec, size_t idx, const T& fallback);

template <typename T>
const typename T::mapped_type* find_ptr(
    const T& map,
    const typename T::key_type& key);

template <typename T>
typename T::mapped_type* find_ptr(
    T* map,
    const typename T::key_type& key);

template <typename T>
typename T::mapped_type find_maybe(
    const T& map,
    const typename T::key_type& key);

template <typename H, typename... T>
std::vector<H> fallback(const std::vector<H>& head, const T&... tail);

template <typename H, typename... T>
std::vector<H> fallback(const std::optional<H>& head, const T&... tail);

} // namespace fviz

#include "algo_impl.h"
