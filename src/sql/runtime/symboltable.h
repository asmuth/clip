/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_QUERY_SYMBOLTABLE_H
#define _FNORDMETRIC_QUERY_SYMBOLTABLE_H
#include <stdlib.h>
#include <string>
#include <vector>
#include <unordered_map>

namespace fnordmetric {
namespace query {
class SymbolTableEntry;
class SValue;

class SymbolTable {
public:
  const SymbolTableEntry* lookupSymbol(const std::string& symbol) const;
  void registerSymbol(const std::string& symbol, const SymbolTableEntry* entry);
protected:
  std::unordered_map<std::string, const SymbolTableEntry*> symbols_;
};

class SymbolTableEntry {
public:

  SymbolTableEntry(
      const std::string& symbol,
      void (*method)(void*, int, SValue*, SValue*));

  SymbolTableEntry(
      const std::string& symbol,
      void (*method)(void*, int, SValue*, SValue*),
      size_t scratchpad_size);

  inline void call(void* scratchpad, int argc, SValue* argv, SValue* out) const {
    call_(scratchpad, argc, argv, out);
  }

  bool isAggregate() const;
  void (*getFnPtr() const)(void*, int, SValue*, SValue*);
  size_t getScratchpadSize() const;

protected:
  void (*call_)(void*, int, SValue*, SValue*);
  const size_t scratchpad_size_;
};

}
}
#endif
