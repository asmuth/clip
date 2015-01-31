/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "fnord-base/stringutil.h"
#include "fnord/stats/statssink.h"

namespace fnord {
namespace stats {

TextStatsSink::TextStatsSink(
    Function<void (const String& line)> callback) :
    callback_(callback) {}

void TextStatsSink::addStatValue(
    const String& path,
    uint64_t value) {
  callback_(StringUtil::format("$0:$1", path, value));
}

void TextStatsSink::addStatValue(
    const String& path,
    const Labels& labels,
    uint64_t value) {
}

void BufferStatsSinkStatsSink::addStatValue(
    const String& path,
    uint64_t value) {
  values_.emplace_back(path, value);
}

void BufferStatsSinkStatsSink::addStatValue(
    const String& path,
    const Labels& labels,
    uint64_t value) {
}

const Vector<Pair<String, double>>& BufferStatsSinkStatsSink::values() const {
  return values_;
}

}
}
