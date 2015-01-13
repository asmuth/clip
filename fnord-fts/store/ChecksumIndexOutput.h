/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef CHECKSUMINDEXOUTPUT_H
#define CHECKSUMINDEXOUTPUT_H

#include <boost/crc.hpp>
#include "fnord-fts/store/IndexOutput.h"

namespace Lucene {

/// Writes bytes through to a primary IndexOutput, computing
/// checksum.  Note that you cannot use seek().
class ChecksumIndexOutput : public IndexOutput {
public:
    ChecksumIndexOutput(const IndexOutputPtr& main);
    virtual ~ChecksumIndexOutput();

    LUCENE_CLASS(ChecksumIndexOutput);

protected:
    IndexOutputPtr main;
    boost::crc_32_type checksum;

public:
    /// Writes a single byte.
    /// @see IndexInput#readByte()
    virtual void writeByte(uint8_t b);

    /// Writes an array of bytes.
    /// @param b the bytes to write.
    /// @param length the number of bytes to write.
    /// @see IndexInput#readBytes(uint8_t*, int32_t, int32_t)
    virtual void writeBytes(const uint8_t* b, int32_t offset, int32_t length);

    /// Return calculated checksum.
    int64_t getChecksum();

    /// Forces any buffered output to be written.
    virtual void flush();

    /// Closes the stream to further operations.
    virtual void close();

    /// Returns the current position in this file, where the next write will occur.
    /// @see #seek(int64_t)
    virtual int64_t getFilePointer();

    /// Sets current position in this file, where the next write will occur.
    /// @see #getFilePointer()
    virtual void seek(int64_t pos);

    /// Starts but does not complete the commit of this file (= writing of
    /// the final checksum at the end).  After this is called must call
    /// {@link #finishCommit} and the {@link #close} to complete the commit.
    void prepareCommit();

    /// See {@link #prepareCommit}
    void finishCommit();

    /// The number of bytes in the file.
    virtual int64_t length();
};

}

#endif
