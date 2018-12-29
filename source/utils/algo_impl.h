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
#pragma once

namespace plotfx {

template <typename T>
const T& at_or(const std::vector<T>& vec, size_t idx, const T& fallback) {
  if (vec.size() > idx) {
    return vec[idx];
  } else {
    return fallback;
  }
}

template <typename T>
const typename T::mapped_type* find_ptr(
    const T& map,
    const typename T::key_type& key) {
  const auto& iter = map.find(key);
  if (iter == map.end()) {
    return nullptr;
  } else {
    return &iter->second;
  }
}

template <typename T>
typename T::mapped_type* find_ptr(
    T* map,
    const typename T::key_type& key) {
  auto iter = map->find(key);
  if (iter == map->end()) {
    return nullptr;
  } else {
    return &iter->second;
  }
}

template <typename T>
typename T::mapped_type find_maybe(
    const T& map,
    const typename T::key_type& key) {
  auto iter = map.find(key);
  if (iter == map.end()) {
    return nullptr;
  } else {
    return iter->second;
  }
}

template <typename H>
std::vector<H> fallback(const std::vector<H>& head) {
  return head;
}

template <typename H, typename... T>
std::vector<H> fallback(const std::vector<H>& head, const T&... tail) {
  if (head.empty()) {
    return fallback(tail...);
  } else {
    return head;
  }
}

template <typename H>
std::vector<H> fallback(const std::optional<H>& head) {
  if (head) {
    return {*head};
  } else {
    return {};
  }
}

template <typename H, typename... T>
std::vector<H> fallback(const std::optional<H>& head, const T&... tail) {
  if (head) {
    return {*head};
  } else {
    return fallback(tail...);
  }
}

} // namespace plotfx

