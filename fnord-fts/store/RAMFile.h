/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef RAMFILE_H
#define RAMFILE_H

#include "fnord-fts/util/LuceneObject.h"

namespace fnord {
namespace fts {

/// File used as buffer in RAMDirectory
class RAMFile : public LuceneObject {
public:
    RAMFile(); // File used as buffer, in no RAMDirectory
    RAMFile(const RAMDirectoryPtr& directory);
    virtual ~RAMFile();

    LUCENE_CLASS(RAMFile);

INTERNAL:
    int64_t length;
    RAMDirectoryWeakPtr _directory;

protected:
    Collection<ByteArray> buffers;

    int64_t sizeInBytes;

    /// This is publicly modifiable via Directory.touchFile(), so direct access not supported
    int64_t lastModified;

public:
    /// For non-stream access from thread that might be concurrent with writing.
    int64_t getLength();
    void setLength(int64_t length);

    /// For non-stream access from thread that might be concurrent with writing
    int64_t getLastModified();
    void setLastModified(int64_t lastModified);

    int64_t getSizeInBytes();

    ByteArray addBuffer(int32_t size);
    ByteArray getBuffer(int32_t index);
    int32_t numBuffers();

protected:
    /// Allocate a new buffer.  Subclasses can allocate differently.
    virtual ByteArray newBuffer(int32_t size);
};

}

}
#endif
