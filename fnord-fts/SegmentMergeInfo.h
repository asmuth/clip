/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef SEGMENTMERGEINFO_H
#define SEGMENTMERGEINFO_H

#include "Term.h"

namespace Lucene {

class SegmentMergeInfo : public LuceneObject {
public:
    SegmentMergeInfo(int32_t b, const TermEnumPtr& te, const IndexReaderPtr& r);
    virtual ~SegmentMergeInfo();

    LUCENE_CLASS(SegmentMergeInfo);

protected:
    TermPositionsPtr postings; // use getPositions()
    Collection<int32_t> docMap; // use getDocMap()

public:
    TermPtr term;
    int32_t base;
    int32_t ord; // the position of the segment in a MultiReader
    TermEnumPtr termEnum;
    IndexReaderWeakPtr _reader;
    int32_t delCount;

public:
    Collection<int32_t> getDocMap();
    TermPositionsPtr getPositions();
    bool next();
    void close();
};

}

#endif
