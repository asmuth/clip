/**
 * Copyright (c) 2016, Paul Asmuth <paul@asmuth.com>
 * All rights reserved.
 * 
 * This file is part of the "libzdb" project. libzdb is free software licensed
 * under the 3-Clause BSD License (BSD-3-Clause).
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

