/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "FormatPostingsDocsWriter.h"
#include "FormatPostingsTermsWriter.h"
#include "FormatPostingsFieldsWriter.h"
#include "FormatPostingsPositionsWriter.h"
#include "IndexFileNames.h"
#include "SegmentWriteState.h"
#include "Directory.h"
#include "TermInfosWriter.h"
#include "DefaultSkipListWriter.h"
#include "FieldInfo.h"
#include "IndexOutput.h"
#include "TermInfo.h"
#include "MiscUtils.h"
#include "UnicodeUtils.h"
#include "StringUtils.h"

namespace Lucene {

FormatPostingsDocsWriter::FormatPostingsDocsWriter(const SegmentWriteStatePtr& state, const FormatPostingsTermsWriterPtr& parent) {
    this->lastDocID = 0;
    this->df = 0;
    this->omitTermFreqAndPositions = false;
    this->storePayloads = false;
    this->freqStart = 0;

    FormatPostingsFieldsWriterPtr parentPostings(parent->_parent);
    this->_parent = parent;
    this->state = state;
    String fileName(IndexFileNames::segmentFileName(parentPostings->segment, IndexFileNames::FREQ_EXTENSION()));
    state->flushedFiles.add(fileName);
    out = parentPostings->dir->createOutput(fileName);
    totalNumDocs = parentPostings->totalNumDocs;

    skipInterval = parentPostings->termsOut->skipInterval;
    skipListWriter = parentPostings->skipListWriter;
    skipListWriter->setFreqOutput(out);

    termInfo = newLucene<TermInfo>();
    utf8 = newLucene<UTF8Result>();
}

FormatPostingsDocsWriter::~FormatPostingsDocsWriter() {
}

void FormatPostingsDocsWriter::initialize() {
    posWriter = newLucene<FormatPostingsPositionsWriter>(state, shared_from_this());
}

void FormatPostingsDocsWriter::setField(const FieldInfoPtr& fieldInfo) {
    this->fieldInfo = fieldInfo;
    omitTermFreqAndPositions = fieldInfo->omitTermFreqAndPositions;
    storePayloads = fieldInfo->storePayloads;
    posWriter->setField(fieldInfo);
}

FormatPostingsPositionsConsumerPtr FormatPostingsDocsWriter::addDoc(int32_t docID, int32_t termDocFreq) {
    int32_t delta = docID - lastDocID;

    if (docID < 0 || (df > 0 && delta <= 0)) {
        boost::throw_exception(CorruptIndexException(L"docs out of order (" + StringUtils::toString(docID) + L" <= " + StringUtils::toString(lastDocID) + L" )"));
    }

    if ((++df % skipInterval) == 0) {
        skipListWriter->setSkipData(lastDocID, storePayloads, posWriter->lastPayloadLength);
        skipListWriter->bufferSkip(df);
    }

    BOOST_ASSERT(docID < totalNumDocs);

    lastDocID = docID;
    if (omitTermFreqAndPositions) {
        out->writeVInt(delta);
    } else if (termDocFreq == 1) {
        out->writeVInt((delta << 1) | 1);
    } else {
        out->writeVInt(delta << 1);
        out->writeVInt(termDocFreq);
    }

    return posWriter;
}

void FormatPostingsDocsWriter::finish() {
    int64_t skipPointer = skipListWriter->writeSkip(out);
    FormatPostingsTermsWriterPtr parent(_parent);
    termInfo->set(df, parent->freqStart, parent->proxStart, (int32_t)(skipPointer - parent->freqStart));

    StringUtils::toUTF8(parent->currentTerm.get() + parent->currentTermStart, parent->currentTerm.size(), utf8);

    if (df > 0) {
        parent->termsOut->add(fieldInfo->number, utf8->result, utf8->length, termInfo);
    }

    lastDocID = 0;
    df = 0;
}

void FormatPostingsDocsWriter::close() {
    out->close();
    posWriter->close();
}

}
