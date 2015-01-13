/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "FreqProxTermsWriterPerField.h"
#include "FreqProxTermsWriter.h"
#include "FieldInfo.h"
#include "Fieldable.h"
#include "TermsHashPerField.h"
#include "FieldInvertState.h"
#include "AttributeSource.h"
#include "Payload.h"
#include "PayloadAttribute.h"
#include "DocumentsWriter.h"
#include "RawPostingList.h"

namespace Lucene {

FreqProxTermsWriterPerField::FreqProxTermsWriterPerField(const TermsHashPerFieldPtr& termsHashPerField, const FreqProxTermsWriterPerThreadPtr& perThread, const FieldInfoPtr& fieldInfo) {
    this->hasPayloads = false;
    this->_termsHashPerField = termsHashPerField;
    this->_perThread = perThread;
    this->fieldInfo = fieldInfo;
    docState = termsHashPerField->docState;
    fieldState = termsHashPerField->fieldState;
    omitTermFreqAndPositions = fieldInfo->omitTermFreqAndPositions;
}

FreqProxTermsWriterPerField::~FreqProxTermsWriterPerField() {
}

int32_t FreqProxTermsWriterPerField::getStreamCount() {
    return fieldInfo->omitTermFreqAndPositions ? 1 : 2;
}

void FreqProxTermsWriterPerField::finish() {
}

void FreqProxTermsWriterPerField::skippingLongTerm() {
}

int32_t FreqProxTermsWriterPerField::compareTo(const LuceneObjectPtr& other) {
    return fieldInfo->name.compare(boost::static_pointer_cast<FreqProxTermsWriterPerField>(other)->fieldInfo->name);
}

void FreqProxTermsWriterPerField::reset() {
    // Record, up front, whether our in-RAM format will be with or without term freqs
    omitTermFreqAndPositions = fieldInfo->omitTermFreqAndPositions;
    payloadAttribute.reset();
}

bool FreqProxTermsWriterPerField::start(Collection<FieldablePtr> fields, int32_t count) {
    for (int32_t i = 0; i < count; ++i) {
        if (fields[i]->isIndexed()) {
            return true;
        }
    }
    return false;
}

void FreqProxTermsWriterPerField::start(const FieldablePtr& field) {
    if (fieldState->attributeSource->hasAttribute<PayloadAttribute>()) {
        payloadAttribute = fieldState->attributeSource->getAttribute<PayloadAttribute>();
    } else {
        payloadAttribute.reset();
    }
}

void FreqProxTermsWriterPerField::writeProx(const FreqProxTermsWriterPostingListPtr& p, int32_t proxCode) {
    PayloadPtr payload;
    if (payloadAttribute) {
        payload = payloadAttribute->getPayload();
    }

    TermsHashPerFieldPtr termsHashPerField(_termsHashPerField);

    if (payload && payload->length() > 0) {
        termsHashPerField->writeVInt(1, (proxCode << 1) | 1);
        termsHashPerField->writeVInt(1, payload->length());
        termsHashPerField->writeBytes(1, payload->getData().get(), payload->getOffset(), payload->length());
        hasPayloads = true;
    } else {
        termsHashPerField->writeVInt(1, proxCode << 1);
    }
    p->lastPosition = fieldState->position;
}

void FreqProxTermsWriterPerField::newTerm(const RawPostingListPtr& p) {
    // First time we're seeing this term since the last flush
    BOOST_ASSERT(docState->testPoint(L"FreqProxTermsWriterPerField.newTerm start"));
    FreqProxTermsWriterPostingListPtr newPostingList(boost::static_pointer_cast<FreqProxTermsWriterPostingList>(p));
    newPostingList->lastDocID = docState->docID;
    if (omitTermFreqAndPositions) {
        newPostingList->lastDocCode = docState->docID;
    } else {
        newPostingList->lastDocCode = docState->docID << 1;
        newPostingList->docFreq = 1;
        writeProx(newPostingList, fieldState->position);
    }
}

void FreqProxTermsWriterPerField::addTerm(const RawPostingListPtr& p) {
    BOOST_ASSERT(docState->testPoint(L"FreqProxTermsWriterPerField.addTerm start"));

    FreqProxTermsWriterPostingListPtr addPostingList(boost::static_pointer_cast<FreqProxTermsWriterPostingList>(p));

    BOOST_ASSERT(omitTermFreqAndPositions || addPostingList->docFreq > 0);
    TermsHashPerFieldPtr termsHashPerField(_termsHashPerField);

    if (omitTermFreqAndPositions) {
        if (docState->docID != addPostingList->lastDocID) {
            BOOST_ASSERT(docState->docID > addPostingList->lastDocID);
            termsHashPerField->writeVInt(0, addPostingList->lastDocCode);
            addPostingList->lastDocCode = docState->docID - addPostingList->lastDocID;
            addPostingList->lastDocID = docState->docID;
        }
    } else {
        if (docState->docID != addPostingList->lastDocID) {
            BOOST_ASSERT(docState->docID > addPostingList->lastDocID);
            // Term not yet seen in the current doc but previously seen in other doc(s) since
            // the last flush

            // Now that we know doc freq for previous doc, write it & lastDocCode
            if (addPostingList->docFreq == 1) {
                termsHashPerField->writeVInt(0, addPostingList->lastDocCode | 1);
            } else {
                termsHashPerField->writeVInt(0, addPostingList->lastDocCode);
                termsHashPerField->writeVInt(0, addPostingList->docFreq);
            }
            addPostingList->docFreq = 1;
            addPostingList->lastDocCode = (docState->docID - addPostingList->lastDocID) << 1;
            addPostingList->lastDocID = docState->docID;
            writeProx(addPostingList, fieldState->position);
        } else {
            ++addPostingList->docFreq;
            writeProx(addPostingList, fieldState->position - addPostingList->lastPosition);
        }
    }
}

void FreqProxTermsWriterPerField::abort() {
}

}
