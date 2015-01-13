/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "FreqProxFieldMergeState.h"
#include "FreqProxTermsWriterPerField.h"
#include "FreqProxTermsWriterPerThread.h"
#include "FreqProxTermsWriter.h"
#include "TermsHashPerThread.h"
#include "TermsHashPerField.h"
#include "ByteSliceReader.h"
#include "DocumentsWriter.h"
#include "CharBlockPool.h"
#include "FieldInfo.h"
#include "MiscUtils.h"

namespace Lucene {

FreqProxFieldMergeState::FreqProxFieldMergeState(const FreqProxTermsWriterPerFieldPtr& field) {
    this->numPostings = 0;
    this->textOffset = 0;
    this->docID = 0;
    this->termFreq = 0;
    this->postingUpto = -1;
    this->freq = newLucene<ByteSliceReader>();
    this->prox = newLucene<ByteSliceReader>();

    this->field = field;
    this->charPool = TermsHashPerThreadPtr(FreqProxTermsWriterPerThreadPtr(field->_perThread)->_termsHashPerThread)->charPool;

    TermsHashPerFieldPtr termsHashPerField(field->_termsHashPerField);
    this->numPostings = termsHashPerField->numPostings;
    this->postings = termsHashPerField->sortPostings();
}

FreqProxFieldMergeState::~FreqProxFieldMergeState() {
}

bool FreqProxFieldMergeState::nextTerm() {
    ++postingUpto;
    if (postingUpto == numPostings) {
        return false;
    }

    p = boost::static_pointer_cast<FreqProxTermsWriterPostingList>(postings[postingUpto]);
    docID = 0;

    text = charPool->buffers[p->textStart >> DocumentsWriter::CHAR_BLOCK_SHIFT];
    textOffset = (p->textStart & DocumentsWriter::CHAR_BLOCK_MASK);

    TermsHashPerFieldPtr termsHashPerField(field->_termsHashPerField);
    termsHashPerField->initReader(freq, p, 0);
    if (!field->fieldInfo->omitTermFreqAndPositions) {
        termsHashPerField->initReader(prox, p, 1);
    }

    // Should always be true
    bool result = nextDoc();
    BOOST_ASSERT(result);

    return true;
}

bool FreqProxFieldMergeState::nextDoc() {
    if (freq->eof()) {
        if (p->lastDocCode != -1) {
            // Return last doc
            docID = p->lastDocID;
            if (!field->omitTermFreqAndPositions) {
                termFreq = p->docFreq;
            }
            p->lastDocCode = -1;
            return true;
        } else {
            // EOF
            return false;
        }
    }

    int32_t code = freq->readVInt();
    if (field->omitTermFreqAndPositions) {
        docID += code;
    } else {
        docID += MiscUtils::unsignedShift(code, 1);
        if ((code & 1) != 0) {
            termFreq = 1;
        } else {
            termFreq = freq->readVInt();
        }
    }

    BOOST_ASSERT(docID != p->lastDocID);

    return true;
}

}
