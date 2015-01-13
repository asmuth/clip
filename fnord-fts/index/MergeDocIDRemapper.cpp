/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "MergeDocIDRemapper.h"
#include "SegmentMerger.h"
#include "MergePolicy.h"
#include "SegmentInfo.h"

namespace Lucene {

MergeDocIDRemapper::MergeDocIDRemapper(const SegmentInfosPtr& infos, Collection< Collection<int32_t> > docMaps, Collection<int32_t> delCounts, const OneMergePtr& merge, int32_t mergedDocCount) {
    this->docMaps = docMaps;
    SegmentInfoPtr firstSegment(merge->segments->info(0));
    int32_t i = 0;
    this->minDocID = 0;
    while (true) {
        SegmentInfoPtr info(infos->info(i));
        if (info->equals(firstSegment)) {
            break;
        }
        minDocID += info->docCount;
        ++i;
    }

    int32_t numDocs = 0;
    for (int32_t j = 0; j < docMaps.size(); ++i, ++j) {
        numDocs += infos->info(i)->docCount;
        BOOST_ASSERT(infos->info(i)->equals(merge->segments->info(j)));
    }
    this->maxDocID = minDocID + numDocs;

    starts = Collection<int32_t>::newInstance(docMaps.size());
    newStarts = Collection<int32_t>::newInstance(docMaps.size());

    starts[0] = minDocID;
    newStarts[0] = minDocID;
    for (i = 1; i < docMaps.size(); ++i) {
        int32_t lastDocCount = merge->segments->info(i - 1)->docCount;
        starts[i] = starts[i - 1] + lastDocCount;
        newStarts[i] = newStarts[i - 1] + lastDocCount - delCounts[i - 1];
    }
    this->docShift = numDocs - mergedDocCount;

    // There are rare cases when docShift is 0.  It happens if you try to delete a docID that's
    // out of bounds, because the SegmentReader still allocates deletedDocs and pretends it has
    // deletions ... so we can't make this assert here: BOOST_ASSERT(docShift > 0);

    // Make sure it all adds up
    BOOST_ASSERT(docShift == maxDocID - (newStarts[docMaps.size() - 1] + merge->segments->info(docMaps.size() - 1)->docCount - delCounts[docMaps.size() - 1]));
}

MergeDocIDRemapper::~MergeDocIDRemapper() {
}

int32_t MergeDocIDRemapper::remap(int32_t oldDocID) {
    if (oldDocID < minDocID) {
        // Unaffected by merge
        return oldDocID;
    } else if (oldDocID >= maxDocID) {
        // This doc was "after" the merge, so simple shift
        return oldDocID - docShift;
    } else {
        // Binary search to locate this document & find its new docID
        Collection<int32_t>::iterator doc = std::upper_bound(starts.begin(), starts.begin() + docMaps.size(), oldDocID);
        int32_t docMap = std::distance(starts.begin(), doc) - 1;

        if (docMaps[docMap]) {
            return newStarts[docMap] + docMaps[docMap][oldDocID - starts[docMap]];
        } else {
            return newStarts[docMap] + oldDocID - starts[docMap];
        }
    }
}

}
