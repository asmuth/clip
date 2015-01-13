/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef INPUTSTREAMREADER_H
#define INPUTSTREAMREADER_H

#include "Reader.h"

namespace Lucene {

/// An InputStreamReader is a bridge from byte streams to character streams.
class InputStreamReader : public Reader {
public:
    /// Create an InputStreamReader that uses the utf8 charset.
    InputStreamReader(const ReaderPtr& reader);
    virtual ~InputStreamReader();

    LUCENE_CLASS(InputStreamReader);

protected:
    ReaderPtr reader;
    UTF8DecoderStreamPtr decoder;

public:
    /// Read a single character.
    virtual int32_t read();

    /// Read characters into a portion of an array.
    virtual int32_t read(wchar_t* b, int32_t offset, int32_t length);

    /// Close the stream.
    virtual void close();

    /// Tell whether this stream supports the mark() operation
    virtual bool markSupported();

    /// Reset the stream.
    virtual void reset();
};

}

#endif
