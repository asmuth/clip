/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <assert.h>
#include <string>
#include <unordered_map>
#include "symboltable.h"
#include <fnordmetric/util/runtimeexception.h>

namespace fnordmetric {
namespace query {

static std::unordered_map<std::string, const SymbolTableEntry*>*
  __globalSymbolTable() {
  static std::unordered_map<std::string, const SymbolTableEntry*> symbols;
  return &symbols;
}

const SymbolTableEntry* lookupSymbol(const std::string& symbol) {
  auto iter = __globalSymbolTable()->find(symbol);

  if (iter == __globalSymbolTable()->end()) {
    RAISE(util::RuntimeException, "symbol not found: %s", symbol.c_str());
    return nullptr;
  } else {
    return iter->second;
  }
}

SymbolTableEntry::SymbolTableEntry(
    const std::string& symbol,
    void (*method)(void*, int, SValue*, SValue*),
    size_t scratchpad_size) :
    call_(method),
    scratchpad_size_(scratchpad_size) {
  assert(__globalSymbolTable()->find(symbol) == __globalSymbolTable()->end());
  (*__globalSymbolTable())[symbol] = this;
}

SymbolTableEntry::SymbolTableEntry(
    const std::string& symbol,
    void (*method)(void*, int, SValue*, SValue*)) :
    SymbolTableEntry(symbol, method, 0) {}

bool SymbolTableEntry::isAggregate() const {
  return scratchpad_size_ > 0;
}

void (*SymbolTableEntry::getFnPtr() const)(void*, int, SValue*, SValue*) {
  return call_;
}

size_t SymbolTableEntry::getScratchpadSize() const {
  return scratchpad_size_;
}

}
}
