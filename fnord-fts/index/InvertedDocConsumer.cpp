/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "InvertedDocConsumer.h"

namespace Lucene {

InvertedDocConsumer::~InvertedDocConsumer() {
}

void InvertedDocConsumer::setFieldInfos(const FieldInfosPtr& fieldInfos) {
    this->fieldInfos = fieldInfos;
}

}
