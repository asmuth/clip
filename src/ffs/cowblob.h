/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_FFS_COWBLOB_H
#define _FNORDMETRIC_FFS_COWBLOB_H
#include "object.h"
#include <fnordmetric/io/pagemanager.h>

namespace fnordmetric {
namespace ffs {

class COWBlob : public Object {
public:
  COWBlob(std::shared_ptr<io::PageManager> page_manager);
};

}
}
#endif
