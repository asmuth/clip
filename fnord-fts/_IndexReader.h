/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef _INDEXREADER_H
#define _INDEXREADER_H

#include "_SegmentInfos.h"

namespace Lucene {

class FindSegmentsModified : public FindSegmentsFileT<uint64_t> {
public:
    FindSegmentsModified(const SegmentInfosPtr& infos, const DirectoryPtr& directory);
    virtual ~FindSegmentsModified();

    LUCENE_CLASS(FindSegmentsModified);

public:
    virtual uint64_t doBody(const String& segmentFileName);
};

}

#endif
