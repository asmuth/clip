/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef STRINGREADER_H
#define STRINGREADER_H

#include "fnord-fts/util/Reader.h"

namespace fnord {
namespace fts {

/// Convenience class for reading strings.
class StringReader : public Reader {
public:
    /// Creates a new StringReader, given the String to read from.
    StringReader(const String& str);
    virtual ~StringReader();

    LUCENE_CLASS(StringReader);

protected:
    String str;
    int32_t position;

public:
    /// Read a single character.
    virtual int32_t read();

    /// Read characters into a portion of an array.
    virtual int32_t read(wchar_t* buffer, int32_t offset, int32_t length);

    /// Close the stream.
    virtual void close();

    /// Tell whether this stream supports the mark() operation
    virtual bool markSupported();

    /// Reset the stream.
    virtual void reset();

    /// The number of bytes in the stream.
    virtual int64_t length();
};

}

}
#endif
