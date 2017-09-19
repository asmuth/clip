/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Paul Asmuth, Laura Schlimmer, FnordCorp B.V.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#pragma once
#include <zdb/util/return_code.h>

namespace zdb {

const char kInitCommandDescription[] = "Create a new zdb file";
const char kInitCommandHelp[] = "Create a new zdb file";

ReturnCode zdbtool_init(const std::vector<std::string>& argv);

} // namespace zdb

