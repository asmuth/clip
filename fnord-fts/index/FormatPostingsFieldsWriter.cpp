/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "FormatPostingsFieldsWriter.h"
#include "FormatPostingsTermsWriter.h"
#include "SegmentWriteState.h"
#include "TermInfosWriter.h"
#include "IndexFileNames.h"
#include "DefaultSkipListWriter.h"

namespace Lucene {

FormatPostingsFieldsWriter::FormatPostingsFieldsWriter(const SegmentWriteStatePtr& state, const FieldInfosPtr& fieldInfos) {
    dir = state->directory;
    segment = state->segmentName;
    totalNumDocs = state->numDocs;
    this->state = state;
    this->fieldInfos = fieldInfos;
    termsOut = newLucene<TermInfosWriter>(dir, segment, fieldInfos, state->termIndexInterval);

    skipListWriter = newLucene<DefaultSkipListWriter>(termsOut->skipInterval, termsOut->maxSkipLevels, totalNumDocs, IndexOutputPtr(), IndexOutputPtr());

    state->flushedFiles.add(state->segmentFileName(IndexFileNames::TERMS_EXTENSION()));
    state->flushedFiles.add(state->segmentFileName(IndexFileNames::TERMS_INDEX_EXTENSION()));
}

FormatPostingsFieldsWriter::~FormatPostingsFieldsWriter() {
}

void FormatPostingsFieldsWriter::initialize() {
    termsWriter = newLucene<FormatPostingsTermsWriter>(state, shared_from_this());
}

FormatPostingsTermsConsumerPtr FormatPostingsFieldsWriter::addField(const FieldInfoPtr& field) {
    termsWriter->setField(field);
    return termsWriter;
}

void FormatPostingsFieldsWriter::finish() {
    termsOut->close();
    termsWriter->close();
}

}
