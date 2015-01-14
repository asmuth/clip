/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_SSTABLE_SSTABLEREPAIR_H
#define _FNORD_SSTABLE_SSTABLEREPAIR_H
#include <string>

namespace fnord {
namespace sstable {

class SSTableRepair {
public:

  SSTableRepair(const std::string& filename);

  bool checkAndRepair(bool repair = false);

protected:
  bool checkAndRepairUnfinishedTable(bool repair);
  const std::string filename_;
};

}
}

#endif
