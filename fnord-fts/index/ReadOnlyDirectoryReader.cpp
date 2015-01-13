/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "ReadOnlyDirectoryReader.h"
#include "ReadOnlySegmentReader.h"

namespace Lucene {

ReadOnlyDirectoryReader::ReadOnlyDirectoryReader(const DirectoryPtr& directory, const SegmentInfosPtr& sis,
        const IndexDeletionPolicyPtr& deletionPolicy, int32_t termInfosIndexDivisor) :
    DirectoryReader(directory, sis, deletionPolicy, true, termInfosIndexDivisor) {
}

ReadOnlyDirectoryReader::ReadOnlyDirectoryReader(const DirectoryPtr& directory, const SegmentInfosPtr& infos,
        Collection<SegmentReaderPtr> oldReaders, Collection<int32_t> oldStarts,
        MapStringByteArray oldNormsCache, bool doClone,
        int32_t termInfosIndexDivisor) :
    DirectoryReader(directory, infos, oldReaders, oldStarts, oldNormsCache, true, doClone, termInfosIndexDivisor) {
}

ReadOnlyDirectoryReader::ReadOnlyDirectoryReader(const IndexWriterPtr& writer, const SegmentInfosPtr& infos, int32_t termInfosIndexDivisor) :
    DirectoryReader(writer, infos, termInfosIndexDivisor) {
}

ReadOnlyDirectoryReader::~ReadOnlyDirectoryReader() {
}

void ReadOnlyDirectoryReader::acquireWriteLock() {
    ReadOnlySegmentReader::noWrite();
}

}
