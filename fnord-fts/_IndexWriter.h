/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef _INDEXWRITER_H
#define _INDEXWRITER_H

#include "LuceneObject.h"

namespace Lucene {

/// Holds shared SegmentReader instances. IndexWriter uses SegmentReaders for 1) applying deletes,
/// 2) doing merges, 3) handing out a real-time reader.  This pool reuses instances of the SegmentReaders
/// in all these places if it is in "near real-time mode" (getReader() has been called on this instance).
class ReaderPool : public LuceneObject {
public:
    ReaderPool(const IndexWriterPtr& writer);
    virtual ~ReaderPool();

    LUCENE_CLASS(ReaderPool);

protected:
    IndexWriterWeakPtr _indexWriter;
    MapSegmentInfoSegmentReader readerMap;

public:
    /// Forcefully clear changes for the specified segments, and remove from the pool.
    /// This is called on successful merge.
    void clear(const SegmentInfosPtr& infos);

    /// used only by asserts
    bool infoIsLive(const SegmentInfoPtr& info);
    SegmentInfoPtr mapToLive(const SegmentInfoPtr& info);

    /// Release the segment reader (i.e. decRef it and close if there are no more references.
    void release(const SegmentReaderPtr& sr);

    /// Release the segment reader (i.e. decRef it and close if there are no more references.
    void release(const SegmentReaderPtr& sr, bool drop);

    /// Remove all our references to readers, and commits any pending changes.
    void close();

    /// Commit all segment reader in the pool.
    void commit();

    /// Returns a ref to a clone.  NOTE: this clone is not enrolled in the pool, so you should
    /// simply close() it when you're done (ie, do not call release()).
    IndexReaderPtr getReadOnlyClone(const SegmentInfoPtr& info, bool doOpenStores, int32_t termInfosIndexDivisor);

    /// Obtain a SegmentReader from the readerPool.  The reader must be returned by calling
    /// {@link #release(SegmentReader)}
    SegmentReaderPtr get(const SegmentInfoPtr& info, bool doOpenStores);

    /// Obtain a SegmentReader from the readerPool.  The reader must be returned by calling
    /// {@link #release(SegmentReader)}
    SegmentReaderPtr get(const SegmentInfoPtr& info, bool doOpenStores, int32_t readBufferSize, int32_t termsIndexDivisor);

    /// Returns a ref
    SegmentReaderPtr getIfExists(const SegmentInfoPtr& info);
};

}

#endif
