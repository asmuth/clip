/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "fnord-fts/fts.h"
#include "fnord-fts/index/FormatPostingsPositionsWriter.h"
#include "fnord-fts/index/FormatPostingsDocsWriter.h"
#include "fnord-fts/index/FormatPostingsTermsWriter.h"
#include "fnord-fts/index/FormatPostingsFieldsWriter.h"
#include "fnord-fts/index/IndexFileNames.h"
#include "fnord-fts/index/FieldInfos.h"
#include "fnord-fts/index/FieldInfo.h"
#include "fnord-fts/index/SegmentWriteState.h"
#include "fnord-fts/store/Directory.h"
#include "fnord-fts/index/DefaultSkipListWriter.h"
#include "fnord-fts/store/IndexOutput.h"

namespace Lucene {

FormatPostingsPositionsWriter::FormatPostingsPositionsWriter(const SegmentWriteStatePtr& state, const FormatPostingsDocsWriterPtr& parent) {
    lastPosition = 0;
    storePayloads = false;
    lastPayloadLength = -1;

    this->_parent = parent;
    FormatPostingsFieldsWriterPtr parentFieldsWriter(FormatPostingsTermsWriterPtr(parent->_parent)->_parent);

    omitTermFreqAndPositions = parent->omitTermFreqAndPositions;

    if (parentFieldsWriter->fieldInfos->hasProx()) {
        // At least one field does not omit TF, so create the prox file
        String fileName(IndexFileNames::segmentFileName(parentFieldsWriter->segment, IndexFileNames::PROX_EXTENSION()));
        state->flushedFiles.add(fileName);
        out = parentFieldsWriter->dir->createOutput(fileName);
        parent->skipListWriter->setProxOutput(out);
    } else {
        // Every field omits TF so we will write no prox file
    }
}

FormatPostingsPositionsWriter::~FormatPostingsPositionsWriter() {
}

void FormatPostingsPositionsWriter::addPosition(int32_t position, ByteArray payload, int32_t payloadOffset, int32_t payloadLength) {
    BOOST_ASSERT(!omitTermFreqAndPositions);
    BOOST_ASSERT(out);

    int32_t delta = position - lastPosition;
    lastPosition = position;

    if (storePayloads) {
        if (payloadLength != lastPayloadLength) {
            lastPayloadLength = payloadLength;
            out->writeVInt((delta << 1) | 1);
            out->writeVInt(payloadLength);
        } else {
            out->writeVInt(delta << 1);
        }
        if (payloadLength > 0) {
            out->writeBytes(payload.get(), payloadLength);
        }
    } else {
        out->writeVInt(delta);
    }
}

void FormatPostingsPositionsWriter::setField(const FieldInfoPtr& fieldInfo) {
    omitTermFreqAndPositions = fieldInfo->omitTermFreqAndPositions;
    storePayloads = omitTermFreqAndPositions ? false : fieldInfo->storePayloads;
}

void FormatPostingsPositionsWriter::finish() {
    lastPosition = 0;
    lastPayloadLength = -1;
}

void FormatPostingsPositionsWriter::close() {
    if (out) {
        out->close();
    }
}

}
