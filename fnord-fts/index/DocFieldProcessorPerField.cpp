/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "DocFieldProcessorPerField.h"
#include "DocFieldProcessorPerThread.h"
#include "DocFieldConsumerPerThread.h"
#include "DocFieldConsumerPerField.h"

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
