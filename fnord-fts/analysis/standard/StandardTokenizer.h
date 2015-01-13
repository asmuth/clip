/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef STANDARDTOKENIZER_H
#define STANDARDTOKENIZER_H

#include "Tokenizer.h"

namespace Lucene {

/// A grammar-based tokenizer
///
/// This should be a good tokenizer for most European-language documents:
///
/// <ul>
///   <li>Splits words at punctuation characters, removing punctuation. However, a dot that's not followed by
///     whitespace is considered part of a token.
///   <li>Splits words at hyphens, unless there's a number in the token, in which case the whole token is interpreted
///     as a product number and is not split.
///   <li>Recognizes email addresses and internet hostnames as one token.
/// </ul>
///
/// Many applications have specific tokenizer needs.  If this tokenizer does not suit your application, please consider
/// copying this source code directory to your project and maintaining your own grammar-based tokenizer.
///
/// You must specify the required {@link Version} compatibility when creating StandardAnalyzer:
///
/// <ul>
///   <li> As of 2.4, Tokens incorrectly identified as acronyms are corrected
/// </ul>
class StandardTokenizer : public Tokenizer {
public:
    /// Creates a new instance of the {@link StandardTokenizer}.  Attaches the input to the newly created scanner.
    /// @param input The input reader
    StandardTokenizer(LuceneVersion::Version matchVersion, const ReaderPtr& input);

    /// Creates a new StandardTokenizer with a given {@link AttributeSource}.
    StandardTokenizer(LuceneVersion::Version matchVersion, const AttributeSourcePtr& source, const ReaderPtr& input);

    /// Creates a new StandardTokenizer with a given {@link AttributeSource.AttributeFactory}
    StandardTokenizer(LuceneVersion::Version matchVersion, const AttributeFactoryPtr& factory, const ReaderPtr& input);

    virtual ~StandardTokenizer();

    LUCENE_CLASS(StandardTokenizer);

protected:
    /// A private instance of the scanner
    StandardTokenizerImplPtr scanner;

    bool replaceInvalidAcronym;
    int32_t maxTokenLength;

    // this tokenizer generates three attributes: offset, positionIncrement and type
    TermAttributePtr termAtt;
    OffsetAttributePtr offsetAtt;
    PositionIncrementAttributePtr posIncrAtt;
    TypeAttributePtr typeAtt;

public:
    static const int32_t ALPHANUM;
    static const int32_t APOSTROPHE;
    static const int32_t ACRONYM;
    static const int32_t COMPANY;
    static const int32_t EMAIL;
    static const int32_t HOST;
    static const int32_t NUM;
    static const int32_t CJ;

    /// @deprecated this solves a bug where HOSTs that end with '.' are identified as ACRONYMs.
    static const int32_t ACRONYM_DEP;

    /// String token types that correspond to token type int constants
    static const Collection<String> TOKEN_TYPES();

protected:
    void init(const ReaderPtr& input, LuceneVersion::Version matchVersion);

public:
    /// Set the max allowed token length.  Any token longer than this is skipped.
    void setMaxTokenLength(int32_t length);

    /// @see #setMaxTokenLength
    int32_t getMaxTokenLength();

    /// @see TokenStream#next()
    virtual bool incrementToken();

    virtual void end();

    virtual void reset(const ReaderPtr& input);

    /// @return true if StandardTokenizer now returns these tokens as Hosts, otherwise false
    /// @deprecated Remove in 3.X and make true the only valid value
    bool isReplaceInvalidAcronym();

    /// @param replaceInvalidAcronym Set to true to replace mischaracterized acronyms as HOST.
    /// @deprecated Remove in 3.X and make true the only valid value
    void setReplaceInvalidAcronym(bool replaceInvalidAcronym);
};

}

#endif
