/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef TERMINFO_H
#define TERMINFO_H

#include "LuceneObject.h"

namespace Lucene {

/// A TermInfo is the record of information stored for a term.
class TermInfo : public LuceneObject {
public:
    TermInfo(const TermInfoPtr& ti);
    TermInfo(int32_t df = 0, int64_t fp = 0, int64_t pp = 0);
    virtual ~TermInfo();

    LUCENE_CLASS(TermInfo);

public:
    /// The number of documents which contain the term.
    int32_t docFreq;
    int64_t freqPointer;
    int64_t proxPointer;
    int32_t skipOffset;

public:
    void set(int32_t docFreq, int64_t freqPointer, int64_t proxPointer, int32_t skipOffset);
    void set(const TermInfoPtr& ti);
};

}

#endif
