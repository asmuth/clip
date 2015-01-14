/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef INDEXOUTPUT_H
#define INDEXOUTPUT_H

#include "fnord-fts/util/LuceneObject.h"

namespace fnord {
namespace fts {

/// Abstract base class for output to a file in a Directory.  A random-access output stream.  Used for all
/// Lucene index output operations.
/// @see Directory
/// @see IndexInput
class IndexOutput : public LuceneObject {
public:
    virtual ~IndexOutput();

    LUCENE_CLASS(IndexOutput);

protected:
    static const int32_t COPY_BUFFER_SIZE;
    ByteArray copyBuffer;

public:
    /// Writes a single byte.
    /// @see IndexInput#readByte()
    virtual void writeByte(uint8_t b) = 0;

    /// Writes an array of bytes.
    /// @param b the bytes to write.
    /// @param length the number of bytes to write.
    /// @see IndexInput#readBytes(uint8_t*, int32_t, int32_t)
    virtual void writeBytes(const uint8_t* b, int32_t offset, int32_t length) = 0;

    /// Forces any buffered output to be written.
    virtual void flush() = 0;

    /// Closes this stream to further operations.
    virtual void close() = 0;

    /// Returns the current position in this file, where the next write will occur.
    virtual int64_t getFilePointer() = 0;

    /// Sets current position in this file, where the next write will occur.
    /// @see #getFilePointer()
    virtual void seek(int64_t pos) = 0;

    /// The number of bytes in the file.
    virtual int64_t length() = 0;

public:
    /// Writes an array of bytes.
    /// @param b the bytes to write.
    /// @param length the number of bytes to write.
    /// @see IndexInput#readBytes(uint8_t*, int32_t, int32_t)
    void writeBytes(const uint8_t* b, int32_t length);

    /// Writes an int as four bytes.
    /// @see IndexInput#readInt()
    void writeInt(int32_t i);

    /// Writes an int in a variable-length format.  Writes between one and five bytes.  Smaller values take fewer bytes.
    /// Negative numbers are not supported.
    /// @see IndexInput#readVInt()
    void writeVInt(int32_t i);

    /// Writes a int64 as eight bytes.
    /// @see IndexInput#readLong()
    void writeLong(int64_t i);

    /// Writes an int64 in a variable-length format.  Writes between one and five bytes.  Smaller values take fewer bytes.
    /// Negative numbers are not supported.
    /// @see IndexInput#readVLong()
    void writeVLong(int64_t i);

    /// Writes a string.
    /// @see IndexInput#readString()
    void writeString(const String& s);

    /// Writes a sub sequence of characters from s as the old format (modified UTF-8 encoded bytes).
    /// @param s the source of the characters.
    /// @param start the first character in the sequence.
    /// @param length the number of characters in the sequence.
    /// @deprecated -- please use {@link #writeString}
    void writeChars(const String& s, int32_t start, int32_t length);

    /// Copy numBytes bytes from input to ourself.
    void copyBytes(const IndexInputPtr& input, int64_t numBytes);

    /// Set the file length. By default, this method does nothing (it's optional for a Directory to implement it).
    /// But, certain Directory implementations (for example @see FSDirectory) can use this to inform the underlying IO
    /// system to pre-allocate the file to the specified size.  If the length is longer than the current file length,
    /// the bytes added to the file are undefined.  Otherwise the file is truncated.
    /// @param length file length.
    void setLength(int64_t length);

    /// Write string map as a series of key/value pairs.
    /// @param map map of string-string key-values.
    void writeStringStringMap(MapStringString map);
};

}

}
#endif
