/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef _DIRECTORYREADER_H
#define _DIRECTORYREADER_H

#include "_SegmentInfos.h"

namespace Lucene {

class FindSegmentsOpen : public FindSegmentsFileT<IndexReaderPtr> {
public:
    FindSegmentsOpen(bool readOnly, const IndexDeletionPolicyPtr& deletionPolicy, int32_t termInfosIndexDivisor, const SegmentInfosPtr& infos, const DirectoryPtr& directory);
    virtual ~FindSegmentsOpen();

    LUCENE_CLASS(FindSegmentsOpen);

protected:
    bool readOnly;
    IndexDeletionPolicyPtr deletionPolicy;
    int32_t termInfosIndexDivisor;

public:
    virtual IndexReaderPtr doBody(const String& segmentFileName);
};

class FindSegmentsReopen : public FindSegmentsFileT<DirectoryReaderPtr> {
public:
    FindSegmentsReopen(const DirectoryReaderPtr& reader, bool openReadOnly, const SegmentInfosPtr& infos, const DirectoryPtr& directory);
    virtual ~FindSegmentsReopen();

    LUCENE_CLASS(FindSegmentsReopen);

protected:
    DirectoryReaderWeakPtr _reader;
    bool openReadOnly;

public:
    virtual DirectoryReaderPtr doBody(const String& segmentFileName);
};

}

#endif
