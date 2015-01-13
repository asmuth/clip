/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef _MMAPDIRECTORY_H
#define _MMAPDIRECTORY_H

#include <boost/iostreams/device/mapped_file.hpp>
#include "IndexInput.h"

namespace Lucene {

class MMapIndexInput : public IndexInput {
public:
    MMapIndexInput(const String& path = L"");
    virtual ~MMapIndexInput();

    LUCENE_CLASS(MMapIndexInput);

protected:
    int32_t _length;
    bool isClone;
    boost::iostreams::mapped_file_source file;
    int32_t bufferPosition; // next byte to read

public:
    /// Reads and returns a single byte.
    /// @see IndexOutput#writeByte(uint8_t)
    virtual uint8_t readByte();

    /// Reads a specified number of bytes into an array at the specified offset.
    /// @param b the array to read bytes into.
    /// @param offset the offset in the array to start storing bytes.
    /// @param length the number of bytes to read.
    /// @see IndexOutput#writeBytes(const uint8_t*,int)
    virtual void readBytes(uint8_t* b, int32_t offset, int32_t length);

    /// Returns the current position in this file, where the next read will occur.
    /// @see #seek(int64_t)
    virtual int64_t getFilePointer();

    /// Sets current position in this file, where the next read will occur.
    /// @see #getFilePointer()
    virtual void seek(int64_t pos);

    /// The number of bytes in the file.
    virtual int64_t length();

    /// Closes the stream to further operations.
    virtual void close();

    /// Returns a clone of this stream.
    virtual LuceneObjectPtr clone(const LuceneObjectPtr& other = LuceneObjectPtr());
};

}

#endif
