/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef TOKENSTREAM_H
#define TOKENSTREAM_H

#include "AttributeSource.h"

namespace Lucene {

/// A TokenStream enumerates the sequence of tokens, either from {@link Field}s of a {@link Document} or from
/// query text.
///
/// This is an abstract class; concrete subclasses are: {@link Tokenizer}, a TokenStream whose input is a Reader;
/// and {@link TokenFilter}, a TokenStream whose input is another TokenStream.
///
/// A new TokenStream API has been introduced with Lucene 2.9. This API has moved from being {@link Token}-based
/// to {@link Attribute}-based.  While {@link Token} still exists in 2.9 as a convenience class, the preferred way
/// to store the information of a {@link Token} is to use {@link Attribute}s.
///
/// TokenStream now extends {@link AttributeSource}, which provides access to all of the token {@link Attribute}s
/// for the TokenStream.  Note that only one instance per {@link Attribute} is created and reused for every
/// token. This approach reduces object creation and allows local caching of references to the {@link Attribute}s.
/// See {@link #incrementToken()} for further details.
///
/// The workflow of the new TokenStream API is as follows:
/// - Instantiation of TokenStream/{@link TokenFilter}s which add/get attributes to/from the {@link AttributeSource}.
/// - The consumer calls {@link TokenStream#reset()}.
/// - The consumer retrieves attributes from the stream and stores local references to all attributes it wants to access.
/// - The consumer calls {@link #incrementToken()} until it returns false consuming the attributes after each call.
/// - The consumer calls {@link #end()} so that any end-of-stream operations can be performed.
/// - The consumer calls {@link #close()} to release any resource when finished using the TokenStream.
///
/// To make sure that filters and consumers know which attributes are available, the attributes must be added during
/// instantiation. Filters and consumers are not required to check for availability of attributes in {@link
/// #incrementToken()}.
///
/// Sometimes it is desirable to capture a current state of a TokenStream, eg., for buffering purposes (see {@link
/// CachingTokenFilter}, {@link TeeSinkTokenFilter}). For this use case {@link AttributeSource#captureState} and {@link
/// AttributeSource#restoreState} can be used.
class TokenStream : public AttributeSource {
protected:
    /// A TokenStream using the default attribute factory.
    TokenStream();

    /// A TokenStream that uses the same attributes as the supplied one.
    TokenStream(const AttributeSourcePtr& input);

    /// A TokenStream using the supplied AttributeFactory for creating new {@link Attribute} instances.
    TokenStream(const AttributeFactoryPtr& factory);

public:
    virtual ~TokenStream();

    LUCENE_CLASS(TokenStream);

public:
    /// Consumers (ie., {@link IndexWriter}) use this method to advance the stream to the next token. Implementing
    /// classes must implement this method and update the appropriate {@link Attribute}s with the attributes of
    /// the next token.
    ///
    /// The producer must make no assumptions about the attributes after the method has been returned: the caller may
    /// arbitrarily change it.  If the producer needs to preserve the state for subsequent calls, it can use {@link
    /// #captureState} to create a copy of the current attribute state.
    ///
    /// This method is called for every token of a document, so an efficient implementation is crucial for good
    /// performance.  To avoid calls to {@link #addAttribute(Class)} and {@link #getAttribute(Class)}, references to
    /// all {@link Attribute}s that this stream uses should be retrieved during instantiation.
    ///
    /// To ensure that filters and consumers know which attributes are available, the attributes must be added during
    /// instantiation. Filters and consumers are not required to check for availability of attributes in {@link
    /// #incrementToken()}.
    ///
    /// @return false for end of stream; true otherwise
    virtual bool incrementToken() = 0;

    /// This method is called by the consumer after the last token has been consumed, after {@link #incrementToken()}
    /// returned false (using the new TokenStream API).  Streams implementing the old API should upgrade to use this
    /// feature.
    ///
    /// This method can be used to perform any end-of-stream operations, such as setting the final offset of a stream.
    /// The final offset of a stream might differ from the offset of the last token eg in case one or more whitespaces
    /// followed after the last token, but a {@link WhitespaceTokenizer} was used.
    virtual void end();

    /// Resets this stream to the beginning.  This is an optional operation, so subclasses may or may not implement
    /// this method. {@link #reset()} is not needed for the standard indexing process. However, if the tokens of a
    /// TokenStream are intended to be consumed more than once, it is necessary to implement {@link #reset()}. Note that
    /// if your TokenStream caches tokens and feeds them back again after a reset, it is imperative that you clone the
    /// tokens when you store them away (on the first pass) as well as when you return them (on future passes after
    /// {@link #reset()}).
    virtual void reset();

    /// Releases resources associated with this stream.
    virtual void close();
};

}

#endif
