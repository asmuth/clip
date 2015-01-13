/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "DocFieldConsumersPerThread.h"
#include "DocFieldProcessorPerThread.h"
#include "DocFieldConsumers.h"
#include "DocFieldConsumersPerField.h"

namespace Lucene {

DocFieldConsumersPerThread::DocFieldConsumersPerThread(const DocFieldProcessorPerThreadPtr& docFieldProcessorPerThread,
        const DocFieldConsumersPtr& parent,
        const DocFieldConsumerPerThreadPtr& one, const DocFieldConsumerPerThreadPtr& two) {
    this->_parent = parent;
    this->one = one;
    this->two = two;
    docState = docFieldProcessorPerThread->docState;
}

DocFieldConsumersPerThread::~DocFieldConsumersPerThread() {
}

void DocFieldConsumersPerThread::startDocument() {
    one->startDocument();
    two->startDocument();
}

void DocFieldConsumersPerThread::abort() {
    LuceneException finally;
    try {
        one->abort();
    } catch (LuceneException& e) {
        finally = e;
    }
    try {
        two->abort();
    } catch (LuceneException& e) {
        finally = e;
    }
    finally.throwException();
}

DocWriterPtr DocFieldConsumersPerThread::finishDocument() {
    DocWriterPtr oneDoc(one->finishDocument());
    DocWriterPtr twoDoc(two->finishDocument());
    if (!oneDoc) {
        return twoDoc;
    } else if (!twoDoc) {
        return oneDoc;
    } else {
        DocFieldConsumersPerDocPtr both(DocFieldConsumersPtr(_parent)->getPerDoc());
        both->docID = docState->docID;
        BOOST_ASSERT(oneDoc->docID == docState->docID);
        BOOST_ASSERT(twoDoc->docID == docState->docID);
        both->one = oneDoc;
        both->two = twoDoc;
        return both;
    }
}

DocFieldConsumerPerFieldPtr DocFieldConsumersPerThread::addField(const FieldInfoPtr& fi) {
    return newLucene<DocFieldConsumersPerField>(shared_from_this(), one->addField(fi), two->addField(fi));
}

}
