/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef STANDARDTOKENIZERIMPL_H
#define STANDARDTOKENIZERIMPL_H

#include "LuceneObject.h"

namespace Lucene {

class StandardTokenizerImpl : public LuceneObject {
public:
    /// Creates a new scanner
    /// @param in the Reader to read input from.
    StandardTokenizerImpl(const ReaderPtr& in);

    virtual ~StandardTokenizerImpl();

    LUCENE_CLASS(StandardTokenizerImpl);

protected:
    /// Initial size of the lookahead buffer
    static const int32_t ZZ_BUFFERSIZE;

    /// Translates characters to character classes
    static CharArray _ZZ_CMAP;
    static const wchar_t ZZ_CMAP_PACKED[];
    static const int32_t ZZ_CMAP_LENGTH;
    static const int32_t ZZ_CMAP_PACKED_LENGTH;

    /// Translates characters to character classes
    static void ZZ_CMAP_INIT();
    static const wchar_t* ZZ_CMAP();

    /// Translates DFA states to action switch labels.
    static IntArray _ZZ_ACTION;
    static const wchar_t ZZ_ACTION_PACKED_0[];
    static const int32_t ZZ_ACTION_LENGTH;
    static const int32_t ZZ_ACTION_PACKED_LENGTH;

    /// Translates DFA states to action switch labels.
    static void ZZ_ACTION_INIT();
    static const int32_t* ZZ_ACTION();

    /// Translates a state to a row index in the transition table
    static IntArray _ZZ_ROWMAP;
    static const wchar_t ZZ_ROWMAP_PACKED_0[];
    static const int32_t ZZ_ROWMAP_LENGTH;
    static const int32_t ZZ_ROWMAP_PACKED_LENGTH;

    /// Translates a state to a row index in the transition table
    static void ZZ_ROWMAP_INIT();
    static const int32_t* ZZ_ROWMAP();

    /// The transition table of the DFA
    static IntArray _ZZ_TRANS;
    static const wchar_t ZZ_TRANS_PACKED_0[];
    static const int32_t ZZ_TRANS_LENGTH;
    static const int32_t ZZ_TRANS_PACKED_LENGTH;

    /// The transition table of the DFA
    static void ZZ_TRANS_INIT();
    static const int32_t* ZZ_TRANS();

    // error codes
    static const int32_t ZZ_UNKNOWN_ERROR;
    static const int32_t ZZ_NO_MATCH;
    static const int32_t ZZ_PUSHBACK_2BIG;

    static const wchar_t* ZZ_ERROR_MSG[];

    /// ZZ_ATTRIBUTE[aState] contains the attributes of state aState
    static IntArray _ZZ_ATTRIBUTE;
    static const wchar_t ZZ_ATTRIBUTE_PACKED_0[];
    static const int32_t ZZ_ATTRIBUTE_LENGTH;
    static const int32_t ZZ_ATTRIBUTE_PACKED_LENGTH;

    /// ZZ_ATTRIBUTE[aState] contains the attributes of state aState
    static void ZZ_ATTRIBUTE_INIT();
    static const int32_t* ZZ_ATTRIBUTE();

    /// The input device
    ReaderPtr zzReader;

    /// The current state of the DFA
    int32_t zzState;

    /// The current lexical state
    int32_t zzLexicalState;

    /// This buffer contains the current text to be matched and is the source of the yytext() string
    CharArray zzBuffer;

    /// The text position at the last accepting state
    int32_t zzMarkedPos;

    /// The text position at the last state to be included in yytext
    int32_t zzPushbackPos;

    /// The current text position in the buffer
    int32_t zzCurrentPos;

    /// StartRead marks the beginning of the yytext() string in the buffer
    int32_t zzStartRead;

    /// EndRead marks the last character in the buffer, that has been read from input
    int32_t zzEndRead;

    /// Number of newlines encountered up to the start of the matched text
    int32_t yyline;

    /// The number of characters up to the start of the matched text
    int32_t _yychar;

    /// The number of characters from the last newline up to the start of the matched text
    int32_t yycolumn;

    /// zzAtBOL == true if the scanner is currently at the beginning of a line
    bool zzAtBOL;

    /// zzAtEOF == true if the scanner is at the EOF
    bool zzAtEOF;

public:
    /// This character denotes the end of file
    static const int32_t YYEOF;

    /// Lexical states
    static const int32_t YYINITIAL;

public:
    int32_t yychar();

    /// Resets the Tokenizer to a new Reader.
    void reset(const ReaderPtr& r);

    /// Fills Lucene token with the current token text.
    void getText(const TokenPtr& t);

    /// Fills TermAttribute with the current token text.
    void getText(const TermAttributePtr& t);

    /// Closes the input stream.
    void yyclose();

    /// Resets the scanner to read from a new input stream.  Does not close the old reader.
    ///
    /// All internal variables are reset, the old input stream cannot be reused (internal buffer is discarded and lost).
    /// Lexical state is set to ZZ_INITIAL.
    ///
    /// @param reader the new input stream.
    void yyreset(const ReaderPtr& reader);

    /// Returns the current lexical state.
    int32_t yystate();

    /// Enters a new lexical state
    /// @param newState the new lexical state.
    void yybegin(int32_t newState);

    /// Returns the text matched by the current regular expression.
    String yytext();

    /// Returns the character at position pos from the  matched text.
    ///
    /// It is equivalent to yytext()[pos], but faster
    /// @param pos the position of the character to fetch.  A value from 0 to yylength() - 1.
    /// @return the character at position pos.
    wchar_t yycharat(int32_t pos);

    /// Returns the length of the matched text region.
    int32_t yylength();

    /// Pushes the specified amount of characters back into the input stream.
    ///
    /// They will be read again by then next call of the scanning method
    /// @param number  the number of characters to be read again.  This number must not be greater than yylength()
    void yypushback(int32_t number);

    /// Resumes scanning until the next regular expression is matched, the end of input is encountered or an I/O-
    /// Error occurs.
    int32_t getNextToken();

protected:
    /// Refills the input buffer.
    bool zzRefill();

    /// Reports an error that occurred while scanning.
    ///
    /// In a well-formed scanner (no or only correct usage of yypushback(int32_t) and a match-all fallback rule)
    /// this method will only be called with things that "Can't Possibly Happen".  If this method is called,
    /// something is seriously wrong.
    ///
    /// Usual syntax/scanner level error handling should be done in error fallback rules.
    ///
    /// @param errorCode The code of the errormessage to display.
    void zzScanError(int32_t errorCode);
};

}

#endif
