/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef HITQUEUE_H
#define HITQUEUE_H

#include "fnord-fts/search/HitQueueBase.h"

namespace fnord {
namespace fts {

class HitQueue : public HitQueueBase {
public:
    /// Creates a new instance with size elements.
    HitQueue(int32_t size, bool prePopulate);
    virtual ~HitQueue();

    LUCENE_CLASS(HitQueue);

protected:
    bool prePopulate;

protected:
    virtual bool lessThan(const ScoreDocPtr& first, const ScoreDocPtr& second);

    /// Returns null if prePopulate is false.
    virtual ScoreDocPtr getSentinelObject();
};

}

}
#endif
