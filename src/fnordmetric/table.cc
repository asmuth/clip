/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V. <paul@asmuth.com>
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <assert.h>
#include <sstream>
#include <fnordmetric/table.h>
#include <fnordmetric/table_map.h>
#include <fnordmetric/util/logging.h>
#include <fnordmetric/util/time.h>

namespace fnordmetric {

TableConfig::TableConfig() :
    interval(10 * kMicrosPerSecond) {}

LabelConfig::LabelConfig(
    const std::string n,
    DataType t /* = DataType::STRING */) :
    column_name(n),
    type(t) {}

} // namespace fnordmetric

