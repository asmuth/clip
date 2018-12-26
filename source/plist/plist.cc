/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2018 Paul Asmuth
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
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

