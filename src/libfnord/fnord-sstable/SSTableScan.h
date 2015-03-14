/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_SSTABLE_SSTABLESCAN_H
#define _FNORD_SSTABLE_SSTABLESCAN_H
#include <stdlib.h>
#include <string>
#include <vector>
#include <memory>
#include <regex>
#include <fnord-base/buffer.h>
#include <fnord-base/exception.h>
#include <fnord-base/io/file.h>
#include <fnord-base/io/mmappedfile.h>
#include <fnord-base/util/binarymessagewriter.h>
#include <fnord-base/option.h>
#include <fnord-sstable/binaryformat.h>
#include <fnord-sstable/fileheaderreader.h>
#include <fnord-sstable/cursor.h>
#include <fnord-sstable/index.h>
#include <fnord-sstable/indexprovider.h>
#include <fnord-sstable/SSTableColumnSchema.h>

namespace fnord {
namespace sstable {

class SSTableScan {
public:
  typedef Function<bool (const String& a, const String& b)> OrderFn;

  SSTableScan(SSTableColumnSchema* schema);

  void setKeyPrefix(const String& prefix);
  void setKeyFilterRegex(const String& regex);
  void setKeyExactMatchFilter(const String& str);
  void setKeyExactMatchFilter(const Set<String>& str);
  void setLimit(long int limit);
  void setOffset(long unsigned int offset);
  void setOrderBy(const String& column, const String& order_fn);
  void setOrderBy(const String& column, OrderFn order_fn);

  void execute(Cursor* cursor, Function<void (const Vector<String> row)> fn);

  Vector<String> columnNames() const;

protected:
  SSTableColumnSchema* schema_;
  Vector<SSTableColumnID> select_list_;
  bool has_order_by_;
  int order_by_index_;
  OrderFn order_by_fn_;
  long int limit_;
  long unsigned int offset_;
  Option<std::regex> key_filter_regex_;
  Set<String> key_exact_match_;
};

} // namespace sstable
} // namespace fnord

#endif
