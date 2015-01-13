/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "fnord-fts/index//LuceneInc.h"
#include "fnord-fts/index/NormsWriterPerThread.h"
#include "fnord-fts/index/NormsWriterPerField.h"
#include "fnord-fts/index/DocInverterPerThread.h"

namespace Lucene {

NormsWriterPerThread::NormsWriterPerThread(const DocInverterPerThreadPtr& docInverterPerThread, const NormsWriterPtr& normsWriter) {
    this->_normsWriter = normsWriter;
    docState = docInverterPerThread->docState;
}

NormsWriterPerThread::~NormsWriterPerThread() {
}

InvertedDocEndConsumerPerFieldPtr NormsWriterPerThread::addField(const DocInverterPerFieldPtr& docInverterPerField, const FieldInfoPtr& fieldInfo) {
    return newLucene<NormsWriterPerField>(docInverterPerField, shared_from_this(), fieldInfo);
}

void NormsWriterPerThread::abort() {
}

void NormsWriterPerThread::startDocument() {
}

void NormsWriterPerThread::finishDocument() {
}

bool NormsWriterPerThread::freeRAM() {
    return false;
}

}
