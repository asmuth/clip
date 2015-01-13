/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "FormatPostingsTermsWriter.h"
#include "FormatPostingsDocsWriter.h"
#include "FormatPostingsFieldsWriter.h"
#include "FormatPostingsPositionsWriter.h"
#include "IndexOutput.h"
#include "DefaultSkipListWriter.h"

namespace Lucene {

FormatPostingsTermsWriter::FormatPostingsTermsWriter(const SegmentWriteStatePtr& state, const FormatPostingsFieldsWriterPtr& parent) {
    currentTermStart = 0;
    freqStart = 0;
    proxStart = 0;

    this->_parent = parent;
    this->state = state;
    termsOut = parent->termsOut;
}

FormatPostingsTermsWriter::~FormatPostingsTermsWriter() {
}

void FormatPostingsTermsWriter::initialize() {
    docsWriter = newLucene<FormatPostingsDocsWriter>(state, shared_from_this());
}

void FormatPostingsTermsWriter::setField(const FieldInfoPtr& fieldInfo) {
    this->fieldInfo = fieldInfo;
    docsWriter->setField(fieldInfo);
}

FormatPostingsDocsConsumerPtr FormatPostingsTermsWriter::addTerm(CharArray text, int32_t start) {
    currentTerm = text;
    currentTermStart = start;

    freqStart = docsWriter->out->getFilePointer();
    if (docsWriter->posWriter->out) {
        proxStart = docsWriter->posWriter->out->getFilePointer();
    }

    FormatPostingsFieldsWriterPtr(_parent)->skipListWriter->resetSkip();

    return docsWriter;
}

void FormatPostingsTermsWriter::finish() {
}

void FormatPostingsTermsWriter::close() {
    docsWriter->close();
}

}
