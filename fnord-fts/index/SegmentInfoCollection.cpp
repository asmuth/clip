/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "SegmentInfoCollection.h"
#include "SegmentInfo.h"

namespace Lucene {

SegmentInfoCollection::SegmentInfoCollection() {
    segmentInfos = Collection<SegmentInfoPtr>::newInstance();
}

SegmentInfoCollection::~SegmentInfoCollection() {
}

int32_t SegmentInfoCollection::size() {
    return segmentInfos.size();
}

bool SegmentInfoCollection::empty() {
    return segmentInfos.empty();
}

void SegmentInfoCollection::clear() {
    segmentInfos.clear();
}

void SegmentInfoCollection::add(const SegmentInfoPtr& info) {
    segmentInfos.add(info);
}

void SegmentInfoCollection::add(int32_t pos, const SegmentInfoPtr& info) {
    segmentInfos.add(pos, info);
}

void SegmentInfoCollection::addAll(const SegmentInfoCollectionPtr& segmentInfos) {
    this->segmentInfos.addAll(segmentInfos->segmentInfos.begin(), segmentInfos->segmentInfos.end());
}

bool SegmentInfoCollection::equals(const LuceneObjectPtr& other) {
    if (LuceneObject::equals(other)) {
        return true;
    }

    SegmentInfoCollectionPtr otherColl(boost::dynamic_pointer_cast<SegmentInfoCollection>(other));
    if (!otherColl) {
        return false;
    }

    return segmentInfos.equals(otherColl->segmentInfos, luceneEquals<SegmentInfoPtr>());
}

int32_t SegmentInfoCollection::find(const SegmentInfoPtr& info) {
    Collection<SegmentInfoPtr>::iterator idx = segmentInfos.find_if(luceneEqualTo<SegmentInfoPtr>(info));
    return idx == segmentInfos.end() ? -1 : std::distance(segmentInfos.begin(), idx);
}

bool SegmentInfoCollection::contains(const SegmentInfoPtr& info) {
    return segmentInfos.contains_if(luceneEqualTo<SegmentInfoPtr>(info));
}

void SegmentInfoCollection::remove(int32_t pos) {
    segmentInfos.remove(segmentInfos.begin() + pos);
}

void SegmentInfoCollection::remove(int32_t start, int32_t end) {
    segmentInfos.remove(segmentInfos.begin() + start, segmentInfos.begin() + end);
}

LuceneObjectPtr SegmentInfoCollection::clone(const LuceneObjectPtr& other) {
    LuceneObjectPtr clone = LuceneObject::clone(other ? other : newLucene<SegmentInfoCollection>());
    SegmentInfoCollectionPtr cloneInfos(boost::dynamic_pointer_cast<SegmentInfoCollection>(clone));
    for (Collection<SegmentInfoPtr>::iterator info = segmentInfos.begin(); info != segmentInfos.end(); ++info) {
        cloneInfos->segmentInfos.add(*info);
    }
    return cloneInfos;
}

}
