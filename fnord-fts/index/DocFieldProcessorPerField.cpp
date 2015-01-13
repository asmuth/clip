/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "fnord-fts/LuceneInc.h"
#include "fnord-fts/index/DocFieldProcessorPerField.h"
#include "fnord-fts/index/DocFieldProcessorPerThread.h"
#include "fnord-fts/index/DocFieldConsumerPerThread.h"
#include "fnord-fts/index/DocFieldConsumerPerField.h"

namespace Lucene {

DocFieldProcessorPerField::DocFieldProcessorPerField(const DocFieldProcessorPerThreadPtr& perThread, const FieldInfoPtr& fieldInfo) {
    lastGen = -1;
    fieldCount = 0;
    fields = Collection<FieldablePtr>::newInstance(1);
    this->consumer = perThread->consumer->addField(fieldInfo);
    this->fieldInfo = fieldInfo;
}

DocFieldProcessorPerField::~DocFieldProcessorPerField() {
}

void DocFieldProcessorPerField::abort() {
    consumer->abort();
}

}
