/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Paul Asmuth <paul@asmuth.com>
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#pragma once
#include <stdlib.h>
#include <string>
#include <iostream>

namespace tsdb {

bool writeVarUInt(std::string* str, uint64_t value);

bool writeVarUInt(std::ostream* os, uint64_t value);

bool readVarUInt(const char** cursor, const char* end, uint64_t* value);

bool readVarUInt(std::istream* is, uint64_t* value);


} // namespace tsdb

