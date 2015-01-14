/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef SEGMENTINFOCOLLECTION_H
#define SEGMENTINFOCOLLECTION_H

#include "fnord-fts/util/LuceneObject.h"

namespace fnord {
namespace fts {

/// A collection of SegmentInfo objects to be used as a base class for {@link SegmentInfos}
class SegmentInfoCollection : public LuceneObject {
public:
    SegmentInfoCollection();
    virtual ~SegmentInfoCollection();

    LUCENE_CLASS(SegmentInfoCollection);

protected:
    Collection<SegmentInfoPtr> segmentInfos;

public:
    int32_t size();
    bool empty();
    void clear();
    void add(const SegmentInfoPtr& info);
    void add(int32_t pos, const SegmentInfoPtr& info);
    void addAll(const SegmentInfoCollectionPtr& segmentInfos);
    bool equals(const LuceneObjectPtr& other);
    int32_t find(const SegmentInfoPtr& info);
    bool contains(const SegmentInfoPtr& info);
    void remove(int32_t pos);
    void remove(int32_t start, int32_t end);

    virtual LuceneObjectPtr clone(const LuceneObjectPtr& other = LuceneObjectPtr());
};

}

}
#endif
