/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef REUSABLESTRINGREADER_H
#define REUSABLESTRINGREADER_H

#include "Reader.h"

namespace Lucene {

/// Used by DocumentsWriter to implemented a StringReader that can be reset to a new string; we use this
/// when tokenizing the string value from a Field.
class ReusableStringReader : public Reader {
public:
    ReusableStringReader();
    virtual ~ReusableStringReader();

    LUCENE_CLASS(ReusableStringReader);

public:
    int32_t upto;
    int32_t left;
    String s;

public:
    virtual void init(const String& s);

    using Reader::read;

    /// Read characters into a portion of an array.
    virtual int32_t read(wchar_t* buffer, int32_t offset, int32_t length);

    /// Close the stream.
    virtual void close();
};

}

#endif
