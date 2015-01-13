/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef _SEGMENTINFOS_H
#define _SEGMENTINFOS_H

#include "LuceneObject.h"

namespace Lucene {

/// Utility class for executing code that needs to do something with the current segments file.
class FindSegmentsFile : public LuceneObject {
public:
    FindSegmentsFile(const SegmentInfosPtr& infos, const DirectoryPtr& directory);
    virtual ~FindSegmentsFile();

    LUCENE_CLASS(FindSegmentsFile);

protected:
    SegmentInfosWeakPtr _segmentInfos;
    DirectoryPtr directory;

public:
    void doRun(const IndexCommitPtr& commit = IndexCommitPtr());
    virtual void runBody(const String& segmentFileName) = 0;
};

template <class TYPE>
class FindSegmentsFileT : public FindSegmentsFile {
public:
    FindSegmentsFileT(const SegmentInfosPtr& infos, const DirectoryPtr& directory) : FindSegmentsFile(infos, directory) {}
    virtual ~FindSegmentsFileT() {}

protected:
    TYPE result;

public:
    virtual TYPE run(const IndexCommitPtr& commit = IndexCommitPtr()) {
        doRun(commit);
        return result;
    }

    virtual void runBody(const String& segmentFileName) {
        result = doBody(segmentFileName);
    }

    virtual TYPE doBody(const String& segmentFileName) = 0;
};

/// Utility class for executing code that needs to do something with the current segments file.  This is necessary with
/// lock-less commits because from the time you locate the current segments file name, until you actually open it, read
/// its contents, or check modified time, etc., it could have been deleted due to a writer commit finishing.
class FindSegmentsRead : public FindSegmentsFileT<int64_t> {
public:
    FindSegmentsRead(const SegmentInfosPtr& infos, const DirectoryPtr& directory);
    virtual ~FindSegmentsRead();

    LUCENE_CLASS(FindSegmentsRead);

public:
    virtual int64_t doBody(const String& segmentFileName);
};

}

#endif
