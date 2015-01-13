/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "SegmentMergeInfo.h"
#include "IndexReader.h"
#include "TermEnum.h"
#include "TermPositions.h"

namespace Lucene {

SegmentMergeInfo::SegmentMergeInfo(int32_t b, const TermEnumPtr& te, const IndexReaderPtr& r) {
    base = b;
    _reader = r;
    termEnum = te;
    term = te->term();
    ord = 0;
    delCount = 0;
}

SegmentMergeInfo::~SegmentMergeInfo() {
}

Collection<int32_t> SegmentMergeInfo::getDocMap() {
    if (!docMap) {
        delCount = 0;
        IndexReaderPtr reader(_reader);

        // build array which maps document numbers around deletions
        if (reader->hasDeletions()) {
            int32_t maxDoc = reader->maxDoc();
            docMap = Collection<int32_t>::newInstance(maxDoc);
            int32_t j = 0;
            for (int32_t i = 0; i < maxDoc; ++i) {
                if (reader->isDeleted(i)) {
                    ++delCount;
                    docMap[i] = -1;
                } else {
                    docMap[i] = j++;
                }
            }
        }
    }
    return docMap;
}

TermPositionsPtr SegmentMergeInfo::getPositions() {
    if (!postings) {
        postings = IndexReaderPtr(_reader)->termPositions();
    }
    return postings;
}

bool SegmentMergeInfo::next() {
    if (termEnum->next()) {
        term = termEnum->term();
        return true;
    } else {
        term.reset();
        return false;
    }
}

void SegmentMergeInfo::close() {
    termEnum->close();
    if (postings) {
        postings->close();
    }
}

}
