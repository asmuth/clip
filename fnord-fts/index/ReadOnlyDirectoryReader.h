/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef READONLYDIRECTORYREADER_H
#define READONLYDIRECTORYREADER_H

#include "DirectoryReader.h"

namespace Lucene {

class ReadOnlyDirectoryReader : public DirectoryReader {
public:
    ReadOnlyDirectoryReader(const DirectoryPtr& directory, const SegmentInfosPtr& sis, const IndexDeletionPolicyPtr& deletionPolicy, int32_t termInfosIndexDivisor);
    ReadOnlyDirectoryReader(const DirectoryPtr& directory, const SegmentInfosPtr& infos, Collection<SegmentReaderPtr> oldReaders,
                            Collection<int32_t> oldStarts, MapStringByteArray oldNormsCache, bool doClone, int32_t termInfosIndexDivisor);
    ReadOnlyDirectoryReader(const IndexWriterPtr& writer, const SegmentInfosPtr& infos, int32_t termInfosIndexDivisor);
    virtual ~ReadOnlyDirectoryReader();

    LUCENE_CLASS(ReadOnlyDirectoryReader);

public:
    /// Tries to acquire the WriteLock on this directory. this method is only valid if this
    /// IndexReader is directory owner.
    virtual void acquireWriteLock();
};

}

#endif
