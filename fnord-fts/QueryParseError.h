/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef QUERYPARSEERROR_H
#define QUERYPARSEERROR_H

#include "LuceneObject.h"

namespace Lucene {

/// Utility class to handle query parse errors
class QueryParseError : public LuceneObject {
public:
    virtual ~QueryParseError();
    LUCENE_CLASS(QueryParseError);

public:
    /// Returns a detailed message for the Error when it is thrown by the token manager to indicate a
    /// lexical error.
    /// @param EOFSeen Indicates if EOF caused the lexical error
    /// @param curLexState Lexical state in which this error occurred
    /// @param errorLine Line number when the error occurred
    /// @param errorColumn Column number when the error occurred
    /// @param errorAfter Prefix that was seen before this error occurred
    /// @param curChar The offending character
    static String lexicalError(bool EOFSeen, int32_t lexState, int32_t errorLine, int32_t errorColumn,
                               const String& errorAfter, wchar_t curChar);

    /// Generate a parse error message and returns it.
    /// @param currentToken This is the last token that has been consumed successfully.  If this object
    /// has been created due to a parse error, the token following this token will (therefore) be the first
    /// error token.
    /// @param expectedTokenSequences Each entry in this array is an array of integers.  Each array of
    /// integers represents a sequence of tokens (by their ordinal values) that is expected at this point
    /// of the parse.
    /// @param tokenImage This is a reference to the "tokenImage" array of the generated parser within
    /// which the parse error occurred.
    static String parseError(const QueryParserTokenPtr& currentToken, Collection< Collection<int32_t> > expectedTokenSequences,
                             Collection<String> tokenImage);


protected:
    /// Replaces unprintable characters by their escaped (or unicode escaped) equivalents in the
    /// given string
    static String addEscapes(const String& str);
};

}

#endif
