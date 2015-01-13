/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef BUFFEREDDELETES_H
#define BUFFEREDDELETES_H

#include "Term.h"
#include "Query.h"

namespace Lucene {

/// Holds buffered deletes, by docID, term or query.  We hold two instances of this class: one for
/// the deletes prior to the last flush, the other for deletes after the last flush.  This is so if
/// we need to abort (discard all buffered docs) we can also discard the buffered deletes yet keep
/// the deletes done during previously flushed segments.
class BufferedDeletes : public LuceneObject {
public:
    BufferedDeletes(bool doTermSort);
    virtual ~BufferedDeletes();

    LUCENE_CLASS(BufferedDeletes);

public:
    int32_t numTerms;
    MapTermNum terms;
    MapQueryInt queries;
    Collection<int32_t> docIDs;
    int64_t bytesUsed;

public:
    int32_t size();
    void update(const BufferedDeletesPtr& in);
    void clear();
    void addBytesUsed(int64_t b);
    bool any();
    void remap(const MergeDocIDRemapperPtr& mapper, const SegmentInfosPtr& infos, Collection< Collection<int32_t> > docMaps, Collection<int32_t> delCounts, const OneMergePtr& merge, int32_t mergedDocCount);
};

/// Number of documents a delete term applies to.
class Num : public LuceneObject {
public:
    Num(int32_t num);

protected:
    int32_t num;

public:
    int32_t getNum();
    void setNum(int32_t num);
};

}

#endif
