/**
 * Copyright (c) 2016 zScale Technology GmbH <legal@zscale.io>
 * Authors:
 *   - Paul Asmuth <paul@zscale.io>
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Affero General Public License ("the license") as
 * published by the Free Software Foundation, either version 3 of the License,
 * or any later version.
 *
 * In accordance with Section 7(e) of the license, the licensing of the Program
 * under the license does not imply a trademark license. Therefore any rights,
 * title and interest in our trademarks remain entirely with us.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the license for more details.
 *
 * You can be released from the requirements of the license by purchasing a
 * commercial license. Buying such a license is mandatory as soon as you develop
 * commercial activities involving this program without disclosing the source
 * code of your own applications
 */
#ifndef _STX_STATS_STATSSINK_H
#define _STX_STATS_STATSSINK_H
#include <stdlib.h>
#include "fnordmetric/util/stdtypes.h"

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
#endif
