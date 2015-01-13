/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "fnord-fts/index//LuceneInc.h"
#include "fnord-fts/index/DocInverterPerThread.h"
#include "fnord-fts/index/DocInverterPerField.h"
#include "fnord-fts/index/DocInverter.h"
#include "fnord-fts/analysis/tokenattributes/TermAttribute.h"
#include "fnord-fts/analysis/tokenattributes/OffsetAttribute.h"
#include "fnord-fts/index/DocFieldProcessorPerThread.h"
#include "fnord-fts/index/InvertedDocConsumer.h"
#include "fnord-fts/index/InvertedDocEndConsumer.h"
#include "fnord-fts/index/InvertedDocConsumerPerThread.h"
#include "fnord-fts/index/InvertedDocEndConsumerPerThread.h"
#include "fnord-fts/index/FieldInvertState.h"
#include "fnord-fts/index/ReusableStringReader.h"

namespace Lucene {

DocInverterPerThread::DocInverterPerThread(const DocFieldProcessorPerThreadPtr& docFieldProcessorPerThread, const DocInverterPtr& docInverter) {
    this->fieldState = newLucene<FieldInvertState>();
    this->stringReader = newLucene<ReusableStringReader>();
    this->singleToken = newLucene<SingleTokenAttributeSource>();
    this->_docInverter = docInverter;
    this->docState = docFieldProcessorPerThread->docState;
}

DocInverterPerThread::~DocInverterPerThread() {
}

void DocInverterPerThread::initialize() {
    DocInverterPtr docInverter(_docInverter);
    consumer = docInverter->consumer->addThread(shared_from_this());
    endConsumer = docInverter->endConsumer->addThread(shared_from_this());
}

void DocInverterPerThread::startDocument() {
    consumer->startDocument();
    endConsumer->startDocument();
}

DocWriterPtr DocInverterPerThread::finishDocument() {
    endConsumer->finishDocument();
    return consumer->finishDocument();
}

void DocInverterPerThread::abort() {
    LuceneException finally;
    try {
        consumer->abort();
    } catch (LuceneException& e) {
        finally = e;
    }
    try {
        endConsumer->abort();
    } catch (LuceneException& e) {
        finally = e;
    }
    finally.throwException();
}

DocFieldConsumerPerFieldPtr DocInverterPerThread::addField(const FieldInfoPtr& fi) {
    return newLucene<DocInverterPerField>(shared_from_this(), fi);
}

SingleTokenAttributeSource::SingleTokenAttributeSource() {
    termAttribute = addAttribute<TermAttribute>();
    offsetAttribute = addAttribute<OffsetAttribute>();
}

SingleTokenAttributeSource::~SingleTokenAttributeSource() {
}

void SingleTokenAttributeSource::reinit(const String& stringValue, int32_t startOffset, int32_t endOffset) {
    termAttribute->setTermBuffer(stringValue);
    offsetAttribute->setOffset(startOffset, endOffset);
}

}
