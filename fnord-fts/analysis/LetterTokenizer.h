/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef LETTERTOKENIZER_H
#define LETTERTOKENIZER_H

#include "CharTokenizer.h"

namespace Lucene {

/// A LetterTokenizer is a tokenizer that divides text at non-letters.  That's to say, it defines tokens as maximal
/// strings of adjacent letters, as defined UnicodeUtil::isAlpha(c) predicate.
///
/// Note: this does a decent job for most European languages, but does a terrible job for some Asian languages, where
/// words are not separated by spaces.
class LetterTokenizer : public CharTokenizer {
public:
    /// Construct a new LetterTokenizer.
    LetterTokenizer(const ReaderPtr& input);

    /// Construct a new LetterTokenizer using a given {@link AttributeSource}.
    LetterTokenizer(const AttributeSourcePtr& source, const ReaderPtr& input);

    /// Construct a new LetterTokenizer using a given {@link AttributeFactory}.
    LetterTokenizer(const AttributeFactoryPtr& factory, const ReaderPtr& input);

    virtual ~LetterTokenizer();

    LUCENE_CLASS(LetterTokenizer);

public:
    /// Collects only characters which satisfy UnicodeUtil::isAlpha(c).
    virtual bool isTokenChar(wchar_t c);
};

}

#endif
