/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef COMPRESSIONTOOLS_H
#define COMPRESSIONTOOLS_H

#include "LuceneObject.h"

namespace Lucene {

/// Simple utility class providing static methods to compress and decompress binary data for stored fields.
class CompressionTools : public LuceneObject {
public:
    virtual ~CompressionTools();

    LUCENE_CLASS(CompressionTools);

public:
    /// Compresses the specified byte range using the specified compressionLevel
    static ByteArray compress(uint8_t* value, int32_t offset, int32_t length, int32_t compressionLevel);

    /// Compresses the specified byte range, with default BEST_COMPRESSION level
    static ByteArray compress(uint8_t* value, int32_t offset, int32_t length);

    /// Compresses all bytes in the array, with default BEST_COMPRESSION level
    static ByteArray compress(ByteArray value);

    /// Compresses the String value, with default BEST_COMPRESSION level
    static ByteArray compressString(const String& value);

    /// Compresses the String value using the specified compressionLevel
    static ByteArray compressString(const String& value, int32_t compressionLevel);

    /// Decompress the byte array previously returned by compress
    static ByteArray decompress(ByteArray value);

    /// Decompress the byte array previously returned by compressString back into a String
    static String decompressString(ByteArray value);

protected:
    static const int32_t COMPRESS_BUFFER;
};

}

#endif
