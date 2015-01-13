/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef TOKENIZER_H
#define TOKENIZER_H

#include "TokenStream.h"

namespace Lucene {

/// A Tokenizer is a TokenStream whose input is a Reader.
///
/// This is an abstract class; subclasses must override {@link #incrementToken()}
///
/// Note: Subclasses overriding {@link #incrementToken()} must call {@link AttributeSource#clearAttributes()}
/// before setting attributes.
class Tokenizer : public TokenStream {
protected:
    /// Construct a tokenizer with null input.
    Tokenizer();

    /// Construct a token stream processing the given input.
    Tokenizer(const ReaderPtr& input);

    /// Construct a tokenizer with null input using the given AttributeFactory.
    Tokenizer(const AttributeFactoryPtr& factory);

    /// Construct a token stream processing the given input using the given AttributeFactory.
    Tokenizer(const AttributeFactoryPtr& factory, const ReaderPtr& input);

    /// Construct a token stream processing the given input using the given AttributeSource.
    Tokenizer(const AttributeSourcePtr& source);

    /// Construct a token stream processing the given input using the given AttributeSource.
    Tokenizer(const AttributeSourcePtr& source, const ReaderPtr& input);

public:
    virtual ~Tokenizer();

    LUCENE_CLASS(Tokenizer);

protected:
    /// The text source for this Tokenizer.
    ReaderPtr input;
    CharStreamPtr charStream;

public:
    /// By default, closes the input Reader.
    virtual void close();

    /// Return the corrected offset. If {@link #input} is a {@link CharStream} subclass this method calls
    /// {@link CharStream#correctOffset}, else returns currentOff.
    /// @param currentOff offset as seen in the output
    /// @return corrected offset based on the input
    /// @see CharStream#correctOffset
    virtual int32_t correctOffset(int32_t currentOff);

    using TokenStream::reset;

    /// Reset the tokenizer to a new reader.  Typically, an analyzer (in its reusableTokenStream method) will
    /// use this to re-use a previously created tokenizer.
    virtual void reset(const ReaderPtr& input);
};

}

#endif
