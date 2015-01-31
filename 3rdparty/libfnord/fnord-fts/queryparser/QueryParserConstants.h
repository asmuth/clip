/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef QUERYPARSERCONSTANTS_H
#define QUERYPARSERCONSTANTS_H

#include "fnord-fts/util/LuceneObject.h"

namespace fnord {
namespace fts {

/// Token literal values and constants.
class QueryParserConstants {
protected:
    QueryParserConstants();

public:
    virtual ~QueryParserConstants();
    LUCENE_INTERFACE(QueryParserConstants);

public:
    enum RegularExpressionId {
        _EOF = 0,
        _NUM_CHAR = 1,
        _ESCAPED_CHAR = 2,
        _TERM_START_CHAR = 3,
        _TERM_CHAR = 4,
        _WHITESPACE = 5,
        _QUOTED_CHAR = 6,
        AND = 8,
        OR = 9,
        NOT = 10,
        PLUS = 11,
        MINUS = 12,
        LPAREN = 13,
        RPAREN = 14,
        COLON = 15,
        STAR = 16,
        CARAT = 17,
        QUOTED = 18,
        TERM = 19,
        FUZZY_SLOP = 20,
        PREFIXTERM = 21,
        WILDTERM = 22,
        RANGEIN_START = 23,
        RANGEEX_START = 24,
        NUMBER = 25,
        RANGEIN_TO = 26,
        RANGEIN_END = 27,
        RANGEIN_QUOTED = 28,
        RANGEIN_GOOP = 29,
        RANGEEX_TO = 30,
        RANGEEX_END = 31,
        RANGEEX_QUOTED = 32,
        RANGEEX_GOOP = 33
    };

    enum LexicalState {
        Boost = 0,
        RangeEx = 1,
        RangeIn = 2,
        DEFAULT = 3
    };

    /// Literal token values.
    static Collection<String> tokenImage;

protected:
    /// Literal token values.
    static const wchar_t* _tokenImage[];
};

}

}
#endif
