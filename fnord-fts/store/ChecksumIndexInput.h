/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef CHECKSUMINDEXINPUT_H
#define CHECKSUMINDEXINPUT_H

#include <boost/crc.hpp>
#include "fnord-fts/store/IndexInput.h"

namespace Lucene {

/// Writes bytes through to a primary IndexInput, computing checksum as it goes.
/// Note that you cannot use seek().
class ChecksumIndexInput : public IndexInput {
public:
    ChecksumIndexInput(const IndexInputPtr& main);
    virtual ~ChecksumIndexInput();

    LUCENE_CLASS(ChecksumIndexInput);

protected:
    IndexInputPtr main;
    boost::crc_32_type checksum;

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

    /// Return calculated checksum.
    int64_t getChecksum();

    /// Closes the stream to further operations.
    virtual void close();

    /// Returns the current position in this file, where the next read will occur.
    /// @see #seek(int64_t)
    virtual int64_t getFilePointer();

    /// Sets current position in this file, where the next read will occur.
    /// @see #getFilePointer()
    virtual void seek(int64_t pos);

    /// The number of bytes in the file.
    virtual int64_t length();

    /// Returns a clone of this stream.
    virtual LuceneObjectPtr clone(const LuceneObjectPtr& other = LuceneObjectPtr());
};

}

#endif
