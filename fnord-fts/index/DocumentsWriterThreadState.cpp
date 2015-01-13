/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "DocumentsWriterThreadState.h"
#include "DocumentsWriter.h"
#include "DocConsumer.h"

namespace Lucene {

DocumentsWriterThreadState::DocumentsWriterThreadState(const DocumentsWriterPtr& docWriter) {
    this->_docWriter = docWriter;
}

DocumentsWriterThreadState::~DocumentsWriterThreadState() {
}

void DocumentsWriterThreadState::initialize() {
    isIdle = true;
    doFlushAfter = false;
    numThreads = 1;
    DocumentsWriterPtr docWriter(_docWriter);
    docState = newLucene<DocState>();
    docState->maxFieldLength = docWriter->maxFieldLength;
    docState->infoStream = docWriter->infoStream;
    docState->similarity = docWriter->similarity;
    docState->_docWriter = docWriter;
    consumer = docWriter->consumer->addThread(shared_from_this());
}

void DocumentsWriterThreadState::doAfterFlush() {
    numThreads = 0;
    doFlushAfter = false;
}

}
