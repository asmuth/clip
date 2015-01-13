/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef READER_H
#define READER_H

#include "LuceneObject.h"

namespace Lucene {

/// Abstract class for reading character streams.
class Reader : public LuceneObject {
protected:
    Reader();

public:
    virtual ~Reader();
    LUCENE_CLASS(Reader);

public:
    static const int32_t READER_EOF;

    /// Read a single character.
    virtual int32_t read();

    /// Read characters into a portion of an array.
    virtual int32_t read(wchar_t* buffer, int32_t offset, int32_t length) = 0;

    /// Skip characters.
    virtual int64_t skip(int64_t n);

    /// Close the stream.
    virtual void close() = 0;

    /// Tell whether this stream supports the mark() operation
    virtual bool markSupported();

    /// Mark the present position in the stream.  Subsequent calls to reset() will attempt to reposition the
    /// stream to this point.
    virtual void mark(int32_t readAheadLimit);

    /// Reset the stream. If the stream has been marked, then attempt to reposition it at the mark.  If the stream
    /// has not been marked, then attempt to reset it in some way appropriate to the particular stream, for example
    /// by repositioning it to its starting point.
    virtual void reset();

    /// The number of bytes in the stream.
    virtual int64_t length();
};

}

#endif
