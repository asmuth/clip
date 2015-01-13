/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef MERGEDOCIDREMAPPER_H
#define MERGEDOCIDREMAPPER_H

#include "LuceneObject.h"

namespace Lucene {

/// Remaps docIDs after a merge has completed, where the merged segments had at least one deletion.
/// This is used to renumber the buffered deletes in IndexWriter when a merge of segments with deletions
/// commits.
class MergeDocIDRemapper : public LuceneObject {
public:
    MergeDocIDRemapper(const SegmentInfosPtr& infos, Collection< Collection<int32_t> > docMaps, Collection<int32_t> delCounts, const OneMergePtr& merge, int32_t mergedDocCount);
    virtual ~MergeDocIDRemapper();

    LUCENE_CLASS(MergeDocIDRemapper);

public:
    Collection<int32_t> starts; // used for binary search of mapped docID
    Collection<int32_t> newStarts; // starts, minus the deletes
    Collection< Collection<int32_t> > docMaps; // maps docIDs in the merged set
    int32_t minDocID; // minimum docID that needs renumbering
    int32_t maxDocID; // 1+ the max docID that needs renumbering
    int32_t docShift; // total # deleted docs that were compacted by this merge

public:
    int32_t remap(int32_t oldDocID);
};

}

#endif
