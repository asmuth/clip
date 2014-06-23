/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_QUERY_SYMBOLTABLE_H
#define _FNORDMETRIC_QUERY_SYMBOLTABLE_H
#include <stdlib.h>
#include <string>
#include <vector>

namespace fnordmetric {
namespace query {
class SymbolTableEntry;
class SValue;

struct ResolvedSymbol {
  SValue* (*call_)(void**, int, SValue**);
  void* scratchpad;
};

void registerSymbol(const std::string& symbol, const SymbolTableEntry* entry);
const SymbolTableEntry* lookupSymbol(const std::string& symbol);

class SymbolTableEntry {
public:

  SymbolTableEntry(
      const std::string& symbol,
      SValue* (*method)(void**, int, SValue**),
      bool is_aggregate);

  inline SValue* call(void** scratchpad, int argc, SValue** argv) const {
    call_(scratchpad, argc, argv);
  }

  bool isAggregate() const;

protected:
  SValue* (*call_)(void**, int, SValue**);
  const bool is_aggregate_;
};

}
}
#endif
