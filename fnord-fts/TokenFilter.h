/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef TOKENFILTER_H
#define TOKENFILTER_H

#include "TokenStream.h"

namespace Lucene {

/// A TokenFilter is a TokenStream whose input is another TokenStream.
///
/// This is an abstract class; subclasses must override {@link #incrementToken()}.
/// @see TokenStream
class TokenFilter : public TokenStream {
protected:
    /// Construct a token stream filtering the given input.
    TokenFilter(const TokenStreamPtr& input);

public:
    virtual ~TokenFilter();

    LUCENE_CLASS(TokenFilter);

protected:
    /// The source of tokens for this filter.
    TokenStreamPtr input;

public:
    /// Performs end-of-stream operations, if any, and calls then end() on the input TokenStream.
    /// NOTE: Be sure to call TokenFilter::end() first when overriding this method.
    virtual void end();

    /// Close the input TokenStream.
    virtual void close();

    /// Reset the filter as well as the input TokenStream.
    virtual void reset();
};

}

#endif
