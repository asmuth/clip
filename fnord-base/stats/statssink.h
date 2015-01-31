/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_STATS_STATSSINK_H
#define _FNORD_STATS_STATSSINK_H
#include <stdlib.h>
#include "fnord-base/stdtypes.h"

namespace fnord {
namespace stats {

class StatsSink {
public:
  typedef Pair<String, String> Label;
  typedef Vector<Label> Labels;

  virtual ~StatsSink() {};

  virtual void addStatValue(
      const String& path,
      uint64_t value) = 0;

  virtual void addStatValue(
      const String& path,
      const Labels& labels,
      uint64_t value) = 0;
};

class TextStatsSink : public StatsSink {
public:

  TextStatsSink(Function<void (const String& line)> callback);

  void addStatValue(
      const String& path,
      uint64_t value) override;

  void addStatValue(
      const String& path,
      const Labels& labels,
      uint64_t value) override;

protected:
  Function<void (const String& line)> callback_;
};

class BufferStatsSinkStatsSink : public StatsSink {
public:

  void addStatValue(
      const String& path,
      uint64_t value) override;

  void addStatValue(
      const String& path,
      const Labels& labels,
      uint64_t value) override;

  const Vector<Pair<String, double>>& values() const;

protected:
  Vector<Pair<String, double>> values_;
};

}
}
#endif
