/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef LOWERCASETOKENIZER_H
#define LOWERCASETOKENIZER_H

#include "LetterTokenizer.h"

namespace Lucene {

/// LowerCaseTokenizer performs the function of LetterTokenizer and LowerCaseFilter together.  It divides text at
/// non-letters and converts them to lower case.  While it is functionally equivalent to the combination of
/// LetterTokenizer and LowerCaseFilter, there is a performance advantage to doing the two tasks at once, hence
/// this (redundant) implementation.
///
/// Note: this does a decent job for most European languages, but does a terrible job for some Asian languages,
/// where words are not separated by spaces.
class LowerCaseTokenizer : public LetterTokenizer {
public:
    /// Construct a new LowerCaseTokenizer.
    LowerCaseTokenizer(const ReaderPtr& input);

    /// Construct a new LowerCaseTokenizer using a given {@link AttributeSource}.
    LowerCaseTokenizer(const AttributeSourcePtr& source, const ReaderPtr& input);

    /// Construct a new LowerCaseTokenizer using a given {@link AttributeFactory}.
    LowerCaseTokenizer(const AttributeFactoryPtr& factory, const ReaderPtr& input);

    virtual ~LowerCaseTokenizer();

    LUCENE_CLASS(LowerCaseTokenizer);

public:
    /// Converts char to lower case CharFolder::toLower<wchar_t>.
    virtual wchar_t normalize(wchar_t c);
};

}

#endif
