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
#include <assert.h>
#include "plist.h"

namespace plist {

const Property& Property::operator[](size_t i) const {
  assert(next);
  assert(i < next->size());
  return (*next)[i];
}

size_t Property::size() const {
  if (next) {
    return next->size();
  } else {
    return 0;
  }
}

Property::operator const std::string&() const {
  return value;
}

bool is_map(const Property& prop) {
  return prop.kind == PropertyKind::MAP;
}

bool is_list(const Property& prop) {
  return prop.kind == PropertyKind::LIST;
}

bool is_tuple(const Property& prop) {
  return prop.kind == PropertyKind::TUPLE;
}

bool is_enum(const Property& prop) {
  return prop.kind == PropertyKind::ENUM;
}

bool is_enum(const Property& prop, const std::string& cmp) {
  return prop.kind == PropertyKind::ENUM && prop.value == cmp;
}

bool is_value(const Property& prop) {
  switch (prop.kind) {
    case PropertyKind::VALUE_LITERAL:
    case PropertyKind::VALUE:
      return true;
    default:
      return false;
  }
}

bool is_value(const Property& prop, const std::string& cmp) {
  switch (prop.kind) {
    case PropertyKind::VALUE_LITERAL:
    case PropertyKind::VALUE:
      break;
    default:
      return false;
  }

  return prop.value == cmp;
}

bool is_value_literal(const Property& prop) {
  return prop.kind == PropertyKind::VALUE_LITERAL;
}

bool is_value_literal(const Property& prop, const std::string& cmp) {
  return prop.kind == PropertyKind::VALUE_LITERAL && prop.value == cmp;
}

bool is_value_quoted(const Property& prop) {
  return prop.kind == PropertyKind::VALUE;
}

bool is_value_quoted(const Property& prop, const std::string& cmp) {
  return prop.kind == PropertyKind::VALUE && prop.value == cmp;
}

std::vector<std::string> flatten(const Property& prop) {
  switch (prop.kind) {
    case PropertyKind::VALUE_LITERAL:
    case PropertyKind::VALUE:
      return {prop.value};
    case PropertyKind::TUPLE:
    case PropertyKind::LIST:
      break;
    default:
      return {};
  }

  std::vector<std::string> flat;
  for (const auto& n : *prop.next) {
    auto next = flatten(n);
    flat.insert(flat.end(), next.begin(), next.end());
  }

  return flat;
}

} // namespace plist

