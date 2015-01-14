/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef CHARREADER_H
#define CHARREADER_H

#include "fnord-fts/analysis/CharStream.h"

namespace fnord {
namespace fts {

/// CharReader is a Reader wrapper.  It reads chars from Reader and outputs {@link CharStream}, defining an
/// identify function {@link #correctOffset} method that simply returns the provided offset.
class CharReader : public CharStream {
public:
    CharReader(const ReaderPtr& in);
    virtual ~CharReader();

    LUCENE_CLASS(CharReader);

protected:
    ReaderPtr input;

public:
    using CharStream::read;

    static CharStreamPtr get(const ReaderPtr& input);

    virtual int32_t correctOffset(int32_t currentOff);
    virtual void close();
    virtual int32_t read(wchar_t* buffer, int32_t offset, int32_t length);
    virtual bool markSupported();
    virtual void mark(int32_t readAheadLimit);
    virtual void reset();
};

}

}
#endif
