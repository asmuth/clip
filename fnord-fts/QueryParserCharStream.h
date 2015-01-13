/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef QUERYPARSERCHARSTREAM_H
#define QUERYPARSERCHARSTREAM_H

#include "LuceneObject.h"

namespace Lucene {

/// This interface describes a character stream that maintains line and column number positions of
/// the characters.  It also has the capability to backup the stream to some extent.  An implementation
/// of this interface is used in the QueryParserTokenManager.
///
/// All the methods except backup can be implemented in any fashion. backup needs to be implemented
/// correctly for the correct operation of the lexer.  Rest of the methods are all used to get information
/// like line number, column number and the String that constitutes a token and are not used by the lexer.
/// Hence their implementation won't affect the generated lexer's operation.
class QueryParserCharStream {
public:
    LUCENE_INTERFACE(QueryParserCharStream);

public:
    /// Returns the next character from the selected input.  The method of selecting the input is the
    /// responsibility of the class implementing this interface.
    virtual wchar_t readChar() = 0;

    /// Returns the column position of the character last read.
    /// @deprecated
    /// @see #getEndColumn
    virtual int32_t getColumn() = 0;

    /// Returns the line number of the character last read.
    /// @deprecated
    /// @see #getEndLine
    virtual int32_t getLine() = 0;

    /// Returns the column number of the last character for current token (being matched after the last
    /// call to BeginToken).
    virtual int32_t getEndColumn() = 0;

    /// Returns the line number of the last character for current token (being matched after the last call
    /// to BeginToken).
    virtual int32_t getEndLine() = 0;

    /// Returns the column number of the first character for current token (being matched after the last
    /// call to BeginToken).
    virtual int32_t getBeginColumn() = 0;

    /// Returns the line number of the first character for current token (being matched after the last call
    /// to BeginToken).
    virtual int32_t getBeginLine() = 0;

    /// Backs up the input stream by amount steps.  Lexer calls this method if it had already read some
    /// characters, but could not use them to match a (longer) token. So, they will be used again as the
    /// prefix of the next token and it is the implementation's's responsibility to do this right.
    virtual void backup(int32_t amount) = 0;

    /// Returns the next character that marks the beginning of the next token.  All characters must remain
    /// in the buffer between two successive calls to this method to implement backup correctly.
    virtual wchar_t BeginToken() = 0;

    /// Returns a string made up of characters from the marked token beginning to the current buffer position.
    /// Implementations have the choice of returning anything that they want to. For example, for efficiency,
    /// one might decide to just return null, which is a valid implementation.
    virtual String GetImage() = 0;

    /// Returns an array of characters that make up the suffix of length for the currently matched token.
    /// This is used to build up the matched string for use in actions in the case of MORE.
    virtual CharArray GetSuffix(int32_t length) = 0;

    /// The lexer calls this function to indicate that it is done with the stream and hence implementations
    /// can free any resources held by this class.  Again, the body of this function can be just empty and it
    /// will not affect the lexer's operation.
    virtual void Done() = 0;
};

}

#endif
