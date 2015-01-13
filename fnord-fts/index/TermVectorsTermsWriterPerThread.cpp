/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "TermVectorsTermsWriterPerThread.h"
#include "TermVectorsTermsWriterPerField.h"
#include "TermVectorsTermsWriter.h"
#include "TermsHashPerThread.h"
#include "ByteSliceReader.h"
#include "FieldInfo.h"
#include "MiscUtils.h"
#include "UnicodeUtils.h"

namespace Lucene {

TermVectorsTermsWriterPerThread::TermVectorsTermsWriterPerThread(const TermsHashPerThreadPtr& termsHashPerThread, const TermVectorsTermsWriterPtr& termsWriter) {
    utf8Results = newCollection<UTF8ResultPtr>(newInstance<UTF8Result>(), newInstance<UTF8Result>());
    this->vectorSliceReader = newLucene<ByteSliceReader>();
    this->_termsWriter = termsWriter;
    this->_termsHashPerThread = termsHashPerThread;
    _docState = termsHashPerThread->docState;
}

TermVectorsTermsWriterPerThread::~TermVectorsTermsWriterPerThread() {
}

void TermVectorsTermsWriterPerThread::startDocument() {
    BOOST_ASSERT(clearLastVectorFieldName());
    if (doc) {
        doc->reset();
        doc->docID = DocStatePtr(_docState)->docID;
    }
}

DocWriterPtr TermVectorsTermsWriterPerThread::finishDocument() {
    DocWriterPtr returnDoc(doc);
    doc.reset();
    return returnDoc;
}

TermsHashConsumerPerFieldPtr TermVectorsTermsWriterPerThread::addField(const TermsHashPerFieldPtr& termsHashPerField, const FieldInfoPtr& fieldInfo) {
    return newLucene<TermVectorsTermsWriterPerField>(termsHashPerField, shared_from_this(), fieldInfo);
}

void TermVectorsTermsWriterPerThread::abort() {
    if (doc) {
        doc->abort();
        doc.reset();
    }
}

bool TermVectorsTermsWriterPerThread::clearLastVectorFieldName() {
    lastVectorFieldName.clear();
    return true;
}

bool TermVectorsTermsWriterPerThread::vectorFieldsInOrder(const FieldInfoPtr& fi) {
    bool inOrder = lastVectorFieldName.empty() ? true : (lastVectorFieldName < fi->name);
    lastVectorFieldName = fi->name;
    return inOrder;
}

}
