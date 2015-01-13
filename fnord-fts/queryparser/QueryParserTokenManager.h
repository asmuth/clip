/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef QUERYPARSERTOKENMANAGER_H
#define QUERYPARSERTOKENMANAGER_H

#include "fnord-fts/queryparser/QueryParserConstants.h"

namespace Lucene {

/// Token Manager.
class QueryParserTokenManager : public QueryParserConstants, public LuceneObject {
public:
    QueryParserTokenManager(const QueryParserCharStreamPtr& stream);
    QueryParserTokenManager(const QueryParserCharStreamPtr& stream, int32_t lexState);

    virtual ~QueryParserTokenManager();

    LUCENE_CLASS(QueryParserTokenManager);

public:
    /// Debug output.
    InfoStreamPtr debugStream;

protected:
    static const int64_t jjbitVec0[];
    static const int64_t jjbitVec1[];
    static const int64_t jjbitVec3[];
    static const int64_t jjbitVec4[];
    static const int32_t jjnextStates[];

    /// Token literal values.
    static const wchar_t* jjstrLiteralImages[];

    /// Lexer state names.
    static const wchar_t* lexStateNames[];

    /// Lex State array.
    static const int32_t jjnewLexState[];
    static const int64_t jjtoToken[];
    static const int64_t jjtoSkip[];

    int32_t curLexState;
    int32_t defaultLexState;
    int32_t jjnewStateCnt;
    int32_t jjround;
    int32_t jjmatchedPos;
    int32_t jjmatchedKind;

    QueryParserCharStreamPtr input_stream;
    IntArray jjrounds;
    IntArray jjstateSet;
    wchar_t curChar;

public:
    /// Set debug output.
    void setDebugStream(const InfoStreamPtr& debugStream);

    /// Reinitialise parser.
    void ReInit(const QueryParserCharStreamPtr& stream);

    /// Reinitialise parser.
    void ReInit(const QueryParserCharStreamPtr& stream, int32_t lexState);

    /// Switch to specified lex state.
    void SwitchTo(int32_t lexState);

    /// Get the next Token.
    QueryParserTokenPtr getNextToken();

protected:
    int32_t jjStopStringLiteralDfa_3(int32_t pos, int64_t active0);
    int32_t jjStartNfa_3(int32_t pos, int64_t active0);
    int32_t jjStopAtPos(int32_t pos, int32_t kind);
    int32_t jjMoveStringLiteralDfa0_3();
    int32_t jjStartNfaWithStates_3(int32_t pos, int32_t kind, int32_t state);
    int32_t jjMoveNfa_3(int32_t startState, int32_t curPos);
    int32_t jjStopStringLiteralDfa_1(int32_t pos, int64_t active0);
    int32_t jjStartNfa_1(int32_t pos, int64_t active0);
    int32_t jjMoveStringLiteralDfa0_1();
    int32_t jjMoveStringLiteralDfa1_1(int64_t active0);
    int32_t jjStartNfaWithStates_1(int32_t pos, int32_t kind, int32_t state);
    int32_t jjMoveNfa_1(int32_t startState, int32_t curPos);
    int32_t jjMoveStringLiteralDfa0_0();
    int32_t jjMoveNfa_0(int32_t startState, int32_t curPos);
    int32_t jjStopStringLiteralDfa_2(int32_t pos, int64_t active0);
    int32_t jjStartNfa_2(int32_t pos, int64_t active0);
    int32_t jjMoveStringLiteralDfa0_2();
    int32_t jjMoveStringLiteralDfa1_2(int64_t active0);
    int32_t jjStartNfaWithStates_2(int32_t pos, int32_t kind, int32_t state);
    int32_t jjMoveNfa_2(int32_t startState, int32_t curPos);

    static bool jjCanMove_0(int32_t hiByte, int32_t i1, int32_t i2, int64_t l1, int64_t l2);
    static bool jjCanMove_1(int32_t hiByte, int32_t i1, int32_t i2, int64_t l1, int64_t l2);
    static bool jjCanMove_2(int32_t hiByte, int32_t i1, int32_t i2, int64_t l1, int64_t l2);

    void ReInitRounds();
    QueryParserTokenPtr jjFillToken();

    void jjCheckNAdd(int32_t state);
    void jjAddStates(int32_t start, int32_t end);
    void jjCheckNAddTwoStates(int32_t state1, int32_t state2);
    void jjCheckNAddStates(int32_t start, int32_t end);
};

}

#endif
