/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef CHARTOKENIZER_H
#define CHARTOKENIZER_H

#include "fnord-fts/analysis/Tokenizer.h"

namespace Lucene {

/// An abstract base class for simple, character-oriented tokenizers.
class CharTokenizer : public Tokenizer {
public:
    CharTokenizer(const ReaderPtr& input);
    CharTokenizer(const AttributeSourcePtr& source, const ReaderPtr& input);
    CharTokenizer(const AttributeFactoryPtr& factory, const ReaderPtr& input);
    virtual ~CharTokenizer();

    LUCENE_CLASS(CharTokenizer);

protected:
    int32_t offset;
    int32_t bufferIndex;
    int32_t dataLen;

    static const int32_t MAX_WORD_LEN;
    static const int32_t IO_BUFFER_SIZE;

    CharArray ioBuffer;
    TermAttributePtr termAtt;
    OffsetAttributePtr offsetAtt;

public:
    virtual bool incrementToken();
    virtual void end();
    virtual void reset(const ReaderPtr& input);

protected:
    /// Returns true if a character should be included in a token.  This tokenizer generates as tokens adjacent
    /// sequences of characters which satisfy this predicate.  Characters for which this is false are used to
    /// define token boundaries and are not included in tokens.
    virtual bool isTokenChar(wchar_t c) = 0;

    /// Called on each token character to normalize it before it is added to the token.  The default implementation
    /// does nothing.  Subclasses may use this to, eg., lowercase tokens.
    virtual wchar_t normalize(wchar_t c);
};

}

#endif
