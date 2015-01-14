/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef WHITESPACETOKENIZER_H
#define WHITESPACETOKENIZER_H

#include "fnord-fts/analysis/CharTokenizer.h"

namespace fnord {
namespace fts {

/// A WhitespaceTokenizer is a tokenizer that divides text at whitespace.  Adjacent sequences of non-Whitespace
/// characters form tokens.
class WhitespaceTokenizer : public CharTokenizer {
public:
    /// Construct a new WhitespaceTokenizer.
    WhitespaceTokenizer(const ReaderPtr& input);

    /// Construct a new WhitespaceTokenizer using a given {@link AttributeSource}.
    WhitespaceTokenizer(const AttributeSourcePtr& source, const ReaderPtr& input);

    /// Construct a new WhitespaceTokenizer using a given {@link AttributeSource.AttributeFactory}.
    WhitespaceTokenizer(const AttributeFactoryPtr& factory, const ReaderPtr& input);

    virtual ~WhitespaceTokenizer();

    LUCENE_CLASS(WhitespaceTokenizer);

public:
    /// Collects only characters which do not satisfy {@link Character#isWhitespace(char)}.
    virtual bool isTokenChar(wchar_t c);
};

}

}
#endif
