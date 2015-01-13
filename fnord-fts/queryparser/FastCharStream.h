/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef FASTCHARSTREAM_H
#define FASTCHARSTREAM_H

#include "fnord-fts/queryparser/QueryParserCharStream.h"

namespace Lucene {

/// An efficient implementation of QueryParserCharStream interface.
///
/// Note that this does not do line-number counting, but instead keeps track of the character position of
/// the token in the input, as required by Lucene's {@link Token} API.
class FastCharStream : public QueryParserCharStream, public LuceneObject {
public:
    /// Constructs from a Reader.
    FastCharStream(const ReaderPtr& reader);
    virtual ~FastCharStream();

    LUCENE_CLASS(FastCharStream);

public:
    CharArray buffer;

    int32_t bufferLength; // end of valid chars
    int32_t bufferPosition; // next char to read

    int32_t tokenStart; // offset in buffer
    int32_t bufferStart; // position in file of buffer

    ReaderPtr input; // source of chars

public:
    virtual wchar_t readChar();
    virtual wchar_t BeginToken();
    virtual void backup(int32_t amount);
    virtual String GetImage();
    virtual CharArray GetSuffix(int32_t length);
    virtual void Done();
    virtual int32_t getColumn();
    virtual int32_t getLine();
    virtual int32_t getEndColumn();
    virtual int32_t getEndLine();
    virtual int32_t getBeginColumn();
    virtual int32_t getBeginLine();

protected:
    void refill();
};

}

#endif
