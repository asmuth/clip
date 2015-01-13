/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef BYTESLICEWRITER_H
#define BYTESLICEWRITER_H

#include "LuceneObject.h"

namespace Lucene {

/// Class to write byte streams into slices of shared byte[].  This is used by DocumentsWriter to hold
/// the posting list for many terms in RAM.
class ByteSliceWriter : public LuceneObject {
public:
    ByteSliceWriter(const ByteBlockPoolPtr& pool);
    virtual ~ByteSliceWriter();

    LUCENE_CLASS(ByteSliceWriter);

protected:
    ByteArray slice;
    int32_t upto;
    ByteBlockPoolPtr pool;

public:
    int32_t offset0;

public:
    /// Set up the writer to write at address.
    void init(int32_t address);

    /// Write byte into byte slice stream
    void writeByte(uint8_t b);

    void writeBytes(const uint8_t* b, int32_t offset, int32_t length);
    int32_t getAddress();
    void writeVInt(int32_t i);
};

}

#endif
