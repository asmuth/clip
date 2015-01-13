/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef QUERYPARSERTOKEN_H
#define QUERYPARSERTOKEN_H

#include "fnord-fts/util/LuceneObject.h"

namespace Lucene {

/// Describes the input token stream.
class QueryParserToken : public LuceneObject {
public:
    /// Constructs a new token for the specified Image and Kind.
    QueryParserToken(int32_t kind = 0, const String& image = EmptyString);

    virtual ~QueryParserToken();

    LUCENE_CLASS(QueryParserToken);

public:
    /// An integer that describes the kind of this token.
    int32_t kind;

    /// The line number of the first character of this Token.
    int32_t beginLine;

    /// The column number of the first character of this Token.
    int32_t beginColumn;

    /// The line number of the last character of this Token.
    int32_t endLine;

    /// The column number of the last character of this Token.
    int32_t endColumn;

    /// The string image of the token.
    String image;

    /// A reference to the next regular (non-special) token from the input stream.  If this is the last
    /// token from the input stream, or if the token manager has not read tokens beyond this one, this
    /// field is set to null.  This is true only if this token is also a regular token.  Otherwise, see
    /// below for a description of the contents of this field.
    QueryParserTokenPtr next;

    /// This field is used to access special tokens that occur prior to this token, but after the
    /// immediately preceding regular (non-special) token.  If there are no such special tokens, this
    /// field is set to null.  When there are more than one such special token, this field refers to the
    /// last of these special tokens, which in turn refers to the next previous special token through
    /// its specialToken field, and so on until the first special token (whose specialToken field is
    /// null).  The next fields of special tokens refer to other special tokens that immediately follow
    /// it (without an intervening regular token).  If there is no such token, this field is null.
    QueryParserTokenPtr specialToken;

public:
    /// Returns the image.
    virtual String toString();

    /// Returns a new Token object, by default. However, if you want, you can create and return subclass
    /// objects based on the value of ofKind.  Simply add the cases to the switch for all those special
    /// cases.
    static QueryParserTokenPtr newToken(int32_t ofKind, const String& image = EmptyString);
};

}

#endif
