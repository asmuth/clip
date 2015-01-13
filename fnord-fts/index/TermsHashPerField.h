/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef TERMSHASHPERFIELD_H
#define TERMSHASHPERFIELD_H

#include "InvertedDocConsumerPerField.h"

namespace Lucene {

class TermsHashPerField : public InvertedDocConsumerPerField {
public:
    TermsHashPerField(const DocInverterPerFieldPtr& docInverterPerField, const TermsHashPerThreadPtr& perThread, const TermsHashPerThreadPtr& nextPerThread, const FieldInfoPtr& fieldInfo);
    virtual ~TermsHashPerField();

    LUCENE_CLASS(TermsHashPerField);

public:
    TermsHashConsumerPerFieldPtr consumer;
    TermsHashPerFieldPtr nextPerField;
    DocInverterPerFieldWeakPtr _docInverterPerField;
    TermsHashPerThreadPtr nextPerThread;
    TermsHashPerThreadWeakPtr _perThread;
    DocStatePtr docState;
    FieldInvertStatePtr fieldState;
    TermAttributePtr termAtt;

    // Copied from our perThread
    CharBlockPoolPtr charPool;
    IntBlockPoolPtr intPool;
    ByteBlockPoolPtr bytePool;

    int32_t streamCount;
    int32_t numPostingInt;

    FieldInfoPtr fieldInfo;

    bool postingsCompacted;
    int32_t numPostings;

    IntArray intUptos;
    int32_t intUptoStart;

protected:
    int32_t postingsHashSize;
    int32_t postingsHashHalfSize;
    int32_t postingsHashMask;
    Collection<RawPostingListPtr> postingsHash;
    RawPostingListPtr p;
    bool doCall;
    bool doNextCall;

public:
    virtual void initialize();
    void shrinkHash(int32_t targetSize);
    void reset();

    /// Called on hitting an aborting exception
    virtual void abort();

    void initReader(const ByteSliceReaderPtr& reader, const RawPostingListPtr& p, int32_t stream);

    /// Collapse the hash table and sort in-place.
    Collection<RawPostingListPtr> sortPostings();

    /// Called before a field instance is being processed
    virtual void start(const FieldablePtr& field);

    /// Called once per field, and is given all Fieldable occurrences for this field in the document.
    virtual bool start(Collection<FieldablePtr> fields, int32_t count);

    void add(int32_t textStart);

    /// Primary entry point (for first TermsHash)
    virtual void add();

    void writeByte(int32_t stream, int8_t b);
    void writeBytes(int32_t stream, const uint8_t* b, int32_t offset, int32_t length);
    void writeVInt(int32_t stream, int32_t i);

    /// Called once per field per document, after all Fieldable occurrences are inverted
    virtual void finish();

    /// Called when postings hash is too small (> 50% occupied) or too large (< 20% occupied).
    void rehashPostings(int32_t newSize);

protected:
    void compactPostings();

    /// Test whether the text for current RawPostingList p equals current tokenText.
    bool postingEquals(const wchar_t* tokenText, int32_t tokenTextLen);
};

}

#endif
