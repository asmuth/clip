/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef _SEGMENTREADER_H
#define _SEGMENTREADER_H

#include "CloseableThreadLocal.h"

namespace Lucene {

/// Holds core readers that are shared (unchanged) when SegmentReader is cloned or reopened
class CoreReaders : public LuceneObject {
public:
    CoreReaders(const SegmentReaderPtr& origInstance, const DirectoryPtr& dir, const SegmentInfoPtr& si, int32_t readBufferSize, int32_t termsIndexDivisor);
    virtual ~CoreReaders();

    LUCENE_CLASS(CoreReaders);

protected:
    /// Counts how many other reader share the core objects (freqStream, proxStream, tis, etc.) of this reader;
    /// when coreRef drops to 0, these core objects may be closed.  A given instance of SegmentReader may be
    /// closed, even those it shares core objects with other SegmentReaders
    SegmentReaderRefPtr ref;

    SegmentReaderWeakPtr _origInstance;

public:
    String segment;
    FieldInfosPtr fieldInfos;
    IndexInputPtr freqStream;
    IndexInputPtr proxStream;
    TermInfosReaderPtr tisNoIndex;

    DirectoryPtr dir;
    DirectoryPtr cfsDir;
    int32_t readBufferSize;
    int32_t termsIndexDivisor;

    TermInfosReaderPtr tis;
    FieldsReaderPtr fieldsReaderOrig;
    TermVectorsReaderPtr termVectorsReaderOrig;
    CompoundFileReaderPtr cfsReader;
    CompoundFileReaderPtr storeCFSReader;

public:
    TermVectorsReaderPtr getTermVectorsReaderOrig();
    FieldsReaderPtr getFieldsReaderOrig();
    void incRef();
    DirectoryPtr getCFSReader();
    TermInfosReaderPtr getTermsReader();
    bool termsIndexIsLoaded();

    /// NOTE: only called from IndexWriter when a near real-time reader is opened, or applyDeletes is run,
    /// sharing a segment that's still being merged.  This method is not fully thread safe, and relies on the
    /// synchronization in IndexWriter
    void loadTermsIndex(const SegmentInfoPtr& si, int32_t termsIndexDivisor);

    void openDocStores(const SegmentInfoPtr& si);

    void decRef();

    friend class SegmentReader;
};

/// Sets the initial value
class FieldsReaderLocal : public CloseableThreadLocal<FieldsReader> {
public:
    FieldsReaderLocal(const SegmentReaderPtr& reader);

protected:
    SegmentReaderWeakPtr _reader;

protected:
    virtual FieldsReaderPtr initialValue();
};

class SegmentReaderRef : public LuceneObject {
public:
    SegmentReaderRef();
    virtual ~SegmentReaderRef();

    LUCENE_CLASS(SegmentReaderRef);

protected:
    int32_t _refCount;

public:
    virtual String toString();
    int32_t refCount();
    int32_t incRef();
    int32_t decRef();

    friend class SegmentReader;
};

/// Byte[] referencing is used because a new norm object needs to be created for each clone, and the byte
/// array is all that is needed for sharing between cloned readers.  The current norm referencing is for
/// sharing between readers whereas the byte[] referencing is for copy on write which is independent of
/// reader references (i.e. incRef, decRef).
class Norm : public LuceneObject {
public:
    Norm();
    Norm(const SegmentReaderPtr& reader, const IndexInputPtr& in, int32_t number, int64_t normSeek);
    virtual ~Norm();

    LUCENE_CLASS(Norm);

protected:
    SegmentReaderWeakPtr _reader;
    int32_t refCount;

    /// If this instance is a clone, the originalNorm references the Norm that has a real open IndexInput
    NormPtr origNorm;
    SegmentReaderPtr origReader;

    IndexInputPtr in;
    int64_t normSeek;

    SegmentReaderRefPtr _bytesRef;
    ByteArray _bytes;
    bool dirty;
    int32_t number;
    bool rollbackDirty;

public:
    void incRef();
    void decRef();

    /// Load bytes but do not cache them if they were not already cached
    void bytes(uint8_t* bytesOut, int32_t offset, int32_t length);

    /// Load & cache full bytes array.  Returns bytes.
    ByteArray bytes();

    /// Only for testing
    SegmentReaderRefPtr bytesRef();

    /// Called if we intend to change a norm value.  We make a private copy of bytes if it's shared
    // with others
    ByteArray copyOnWrite();

    /// Returns a copy of this Norm instance that shares IndexInput & bytes with the original one
    virtual LuceneObjectPtr clone(const LuceneObjectPtr& other = LuceneObjectPtr());

    /// Flush all pending changes to the next generation separate norms file.
    void reWrite(const SegmentInfoPtr& si);

protected:
    void closeInput();

    friend class SegmentReader;
};

}

#endif
