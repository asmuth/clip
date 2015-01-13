/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef SEGMENTTERMENUM_H
#define SEGMENTTERMENUM_H

#include "TermEnum.h"

namespace Lucene {

class SegmentTermEnum : public TermEnum {
public:
    SegmentTermEnum();
    SegmentTermEnum(const IndexInputPtr& i, const FieldInfosPtr& fis, bool isi);
    virtual ~SegmentTermEnum();

    LUCENE_CLASS(SegmentTermEnum);

protected:
    IndexInputPtr input;
    TermBufferPtr termBuffer;
    TermBufferPtr prevBuffer;
    TermBufferPtr scanBuffer; // used for scanning

    TermInfoPtr _termInfo;

    int32_t format;
    bool isIndex;
    int32_t formatM1SkipInterval;

public:
    FieldInfosPtr fieldInfos;
    int64_t size;
    int64_t position;

    int64_t indexPointer;
    int32_t indexInterval;
    int32_t skipInterval;
    int32_t maxSkipLevels;

public:
    virtual LuceneObjectPtr clone(const LuceneObjectPtr& other = LuceneObjectPtr());

    void seek(int64_t pointer, int64_t p, const TermPtr& t, const TermInfoPtr& ti);

    /// Increments the enumeration to the next element.  True if one exists.
    virtual bool next();

    /// Optimized scan, without allocating new terms. Return number of invocations to next().
    int32_t scanTo(const TermPtr& term);

    /// Returns the current Term in the enumeration.
    /// Initially invalid, valid after next() called for the first time.
    virtual TermPtr term();

    /// Returns the previous Term enumerated. Initially null.
    TermPtr prev();

    /// Returns the current TermInfo in the enumeration.
    /// Initially invalid, valid after next() called for the first time.
    TermInfoPtr termInfo();

    /// Sets the argument to the current TermInfo in the enumeration.
    /// Initially invalid, valid after next() called for the first time.
    void termInfo(const TermInfoPtr& ti);

    /// Returns the docFreq of the current Term in the enumeration.
    /// Initially invalid, valid after next() called for the first time.
    virtual int32_t docFreq();

    /// Returns the freqPointer from the current TermInfo in the enumeration.
    /// Initially invalid, valid after next() called for the first time.
    int64_t freqPointer();

    /// Returns the proxPointer from the current TermInfo in the enumeration.
    /// Initially invalid, valid after next() called for the first time.
    int64_t proxPointer();

    /// Closes the enumeration to further activity, freeing resources.
    virtual void close();
};

}

#endif
