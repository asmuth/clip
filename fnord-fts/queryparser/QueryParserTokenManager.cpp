/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "QueryParserTokenManager.h"
#include "QueryParserCharStream.h"
#include "QueryParserToken.h"
#include "QueryParseError.h"
#include "InfoStream.h"
#include "StringUtils.h"

namespace Lucene {

const int64_t QueryParserTokenManager::jjbitVec0[] = {0x1LL, 0x0LL, 0x0LL, 0x0LL};
const int64_t QueryParserTokenManager::jjbitVec1[] = {static_cast<int64_t>(0xfffffffffffffffeLL), static_cast<int64_t>(0xffffffffffffffffLL), static_cast<int64_t>(0xffffffffffffffffLL), static_cast<int64_t>(0xffffffffffffffffLL)};
const int64_t QueryParserTokenManager::jjbitVec3[] = {0x0LL, 0x0LL, static_cast<int64_t>(0xffffffffffffffffLL), static_cast<int64_t>(0xffffffffffffffffLL)};
const int64_t QueryParserTokenManager::jjbitVec4[] = {static_cast<int64_t>(0xfffefffffffffffeLL), static_cast<int64_t>(0xffffffffffffffffLL), static_cast<int64_t>(0xffffffffffffffffLL), static_cast<int64_t>(0xffffffffffffffffLL)};

const int32_t QueryParserTokenManager::jjnextStates[] = {15, 16, 18, 29, 32, 23, 33, 30, 20, 21, 32, 23, 33, 31, 34, 27, 2, 4, 5, 0, 1};

/// Token literal values.
const wchar_t* QueryParserTokenManager::jjstrLiteralImages[] = {
    L"", L"", L"", L"", L"", L"", L"", L"", L"", L"", L"", L"\53", L"\55",
    L"\50", L"\51", L"\72", L"\52", L"\136", L"", L"", L"", L"", L"", L"\133",
    L"\173", L"", L"\124\117", L"\135", L"", L"", L"\124\117", L"\175", L"", L""
};

/// Lexer state names.
const wchar_t* QueryParserTokenManager::lexStateNames[] = {
    L"Boost", L"RangeEx", L"RangeIn", L"DEFAULT"
};

/// Lex State array.
const int32_t QueryParserTokenManager::jjnewLexState[] = {
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 0,
    -1, -1, -1, -1, -1, 2, 1, 3, -1, 3, -1, -1, -1, 3, -1, -1
};

const int64_t QueryParserTokenManager::jjtoToken[] = {0x3ffffff01LL};
const int64_t QueryParserTokenManager::jjtoSkip[] = {0x80LL};

QueryParserTokenManager::QueryParserTokenManager(const QueryParserCharStreamPtr& stream) {
    debugStream = newLucene<InfoStreamOut>();
    jjrounds = IntArray::newInstance(36);
    jjstateSet = IntArray::newInstance(72);
    curChar = 0;
    curLexState = 3;
    defaultLexState = 3;
    jjnewStateCnt = 0;
    jjround = 0;
    jjmatchedPos = 0;
    jjmatchedKind = 0;
    input_stream = stream;
}

QueryParserTokenManager::QueryParserTokenManager(const QueryParserCharStreamPtr& stream, int32_t lexState) {
    debugStream = newLucene<InfoStreamOut>();
    jjrounds = IntArray::newInstance(36);
    jjstateSet = IntArray::newInstance(72);
    input_stream = stream;
    curChar = 0;
    curLexState = 3;
    defaultLexState = 3;
    jjnewStateCnt = 0;
    jjround = 0;
    jjmatchedPos = 0;
    jjmatchedKind = 0;
    SwitchTo(lexState);
}

QueryParserTokenManager::~QueryParserTokenManager() {
}

void QueryParserTokenManager::setDebugStream(const InfoStreamPtr& debugStream) {
    this->debugStream = debugStream;
}

int32_t QueryParserTokenManager::jjStopStringLiteralDfa_3(int32_t pos, int64_t active0) {
    return -1;
}

int32_t QueryParserTokenManager::jjStartNfa_3(int32_t pos, int64_t active0) {
    return jjMoveNfa_3(jjStopStringLiteralDfa_3(pos, active0), pos + 1);
}

int32_t QueryParserTokenManager::jjStopAtPos(int32_t pos, int32_t kind) {
    jjmatchedKind = kind;
    jjmatchedPos = pos;
    return pos + 1;
}

int32_t QueryParserTokenManager::jjMoveStringLiteralDfa0_3() {
    switch (curChar) {
    case 40:
        return jjStopAtPos(0, 13);
    case 41:
        return jjStopAtPos(0, 14);
    case 42:
        return jjStartNfaWithStates_3(0, 16, 36);
    case 43:
        return jjStopAtPos(0, 11);
    case 45:
        return jjStopAtPos(0, 12);
    case 58:
        return jjStopAtPos(0, 15);
    case 91:
        return jjStopAtPos(0, 23);
    case 94:
        return jjStopAtPos(0, 17);
    case 123:
        return jjStopAtPos(0, 24);
    default:
        return jjMoveNfa_3(0, 0);
    }
}

int32_t QueryParserTokenManager::jjStartNfaWithStates_3(int32_t pos, int32_t kind, int32_t state) {
    jjmatchedKind = kind;
    jjmatchedPos = pos;
    try {
        curChar = input_stream->readChar();
    } catch (IOException&) {
        return pos + 1;
    }
    return jjMoveNfa_3(state, pos + 1);
}

int32_t QueryParserTokenManager::jjMoveNfa_3(int32_t startState, int32_t curPos) {
    int32_t startsAt = 0;
    jjnewStateCnt = 36;
    int32_t i = 1;
    jjstateSet[0] = startState;
    int32_t kind = 0x7fffffff;
    while (true) {
        if (++jjround == 0x7fffffff) {
            ReInitRounds();
        }
        if (curChar < 64) {
            int64_t l = (int64_t)1 << curChar;
            do {
                switch (jjstateSet[--i]) {
                case 36:
                case 25:
                    if ((0xfbfffcf8ffffd9ffLL & l) == 0) {
                        break;
                    }
                    if (kind > 22) {
                        kind = 22;
                    }
                    jjCheckNAddTwoStates(25, 26);
                    break;
                case 0:
                    if ((0xfbffd4f8ffffd9ffLL & l) != 0) {
                        if (kind > 22) {
                            kind = 22;
                        }
                        jjCheckNAddTwoStates(25, 26);
                    } else if ((0x100002600LL & l) != 0) {
                        if (kind > 7) {
                            kind = 7;
                        }
                    } else if (curChar == 34) {
                        jjCheckNAddStates(0, 2);
                    } else if (curChar == 33) {
                        if (kind > 10) {
                            kind = 10;
                        }
                    }
                    if ((0x7bffd0f8ffffd9ffLL & l) != 0) {
                        if (kind > 19) {
                            kind = 19;
                        }
                        jjCheckNAddStates(3, 7);
                    } else if (curChar == 42) {
                        if (kind > 21) {
                            kind = 21;
                        }
                    }
                    if (curChar == 38) {
                        jjstateSet[jjnewStateCnt++] = 4;
                    }
                    break;
                case 4:
                    if (curChar == 38 && kind > 8) {
                        kind = 8;
                    }
                    break;
                case 5:
                    if (curChar == 38) {
                        jjstateSet[jjnewStateCnt++] = 4;
                    }
                    break;
                case 13:
                    if (curChar == 33 && kind > 10) {
                        kind = 10;
                    }
                    break;
                case 14:
                    if (curChar == 34) {
                        jjCheckNAddStates(0, 2);
                    }
                    break;
                case 15:
                    if ((0xfffffffbffffffffLL & l) != 0) {
                        jjCheckNAddStates(0, 2);
                    }
                    break;
                case 17:
                    jjCheckNAddStates(0, 2);
                    break;
                case 18:
                    if (curChar == 34 && kind > 18) {
                        kind = 18;
                    }
                    break;
                case 20:
                    if ((0x3ff000000000000LL & l) == 0) {
                        break;
                    }
                    if (kind > 20) {
                        kind = 20;
                    }
                    jjAddStates(8, 9);
                    break;
                case 21:
                    if (curChar == 46) {
                        jjCheckNAdd(22);
                    }
                    break;
                case 22:
                    if ((0x3ff000000000000LL & l) == 0) {
                        break;
                    }
                    if (kind > 20) {
                        kind = 20;
                    }
                    jjCheckNAdd(22);
                    break;
                case 23:
                    if (curChar == 42 && kind > 21) {
                        kind = 21;
                    }
                    break;
                case 24:
                    if ((0xfbffd4f8ffffd9ffLL & l) == 0) {
                        break;
                    }
                    if (kind > 22) {
                        kind = 22;
                    }
                    jjCheckNAddTwoStates(25, 26);
                    break;
                case 27:
                    if (kind > 22) {
                        kind = 22;
                    }
                    jjCheckNAddTwoStates(25, 26);
                    break;
                case 28:
                    if ((0x7bffd0f8ffffd9ffLL & l) == 0) {
                        break;
                    }
                    if (kind > 19) {
                        kind = 19;
                    }
                    jjCheckNAddStates(3, 7);
                    break;
                case 29:
                    if ((0x7bfff8f8ffffd9ffLL & l) == 0) {
                        break;
                    }
                    if (kind > 19) {
                        kind = 19;
                    }
                    jjCheckNAddTwoStates(29, 30);
                    break;
                case 31:
                    if (kind > 19) {
                        kind = 19;
                    }
                    jjCheckNAddTwoStates(29, 30);
                    break;
                case 32:
                    if ((0x7bfff8f8ffffd9ffLL & l) != 0) {
                        jjCheckNAddStates(10, 12);
                    }
                    break;
                case 34:
                    jjCheckNAddStates(10, 12);
                    break;
                default:
                    break;
                }
            } while (i != startsAt);
        } else if (curChar < 128) {
            int64_t l = (int64_t)1 << (curChar & 077);
            do {
                switch (jjstateSet[--i]) {
                case 36:
                    if ((0x97ffffff87ffffffLL & l) != 0) {
                        if (kind > 22) {
                            kind = 22;
                        }
                        jjCheckNAddTwoStates(25, 26);
                    } else if (curChar == 92) {
                        jjCheckNAddTwoStates(27, 27);
                    }
                    break;
                case 0:
                    if ((0x97ffffff87ffffffLL & l) != 0) {
                        if (kind > 19) {
                            kind = 19;
                        }
                        jjCheckNAddStates(3, 7);
                    } else if (curChar == 92) {
                        jjCheckNAddStates(13, 15);
                    } else if (curChar == 126) {
                        if (kind > 20) {
                            kind = 20;
                        }
                        jjstateSet[jjnewStateCnt++] = 20;
                    }
                    if ((0x97ffffff87ffffffLL & l) != 0) {
                        if (kind > 22) {
                            kind = 22;
                        }
                        jjCheckNAddTwoStates(25, 26);
                    }
                    if (curChar == 78) {
                        jjstateSet[jjnewStateCnt++] = 11;
                    } else if (curChar == 124) {
                        jjstateSet[jjnewStateCnt++] = 8;
                    } else if (curChar == 79) {
                        jjstateSet[jjnewStateCnt++] = 6;
                    } else if (curChar == 65) {
                        jjstateSet[jjnewStateCnt++] = 2;
                    }
                    break;
                case 1:
                    if (curChar == 68 && kind > 8) {
                        kind = 8;
                    }
                    break;
                case 2:
                    if (curChar == 78) {
                        jjstateSet[jjnewStateCnt++] = 1;
                    }
                    break;
                case 3:
                    if (curChar == 65) {
                        jjstateSet[jjnewStateCnt++] = 2;
                    }
                    break;
                case 6:
                    if (curChar == 82 && kind > 9) {
                        kind = 9;
                    }
                    break;
                case 7:
                    if (curChar == 79) {
                        jjstateSet[jjnewStateCnt++] = 6;
                    }
                    break;
                case 8:
                    if (curChar == 124 && kind > 9) {
                        kind = 9;
                    }
                    break;
                case 9:
                    if (curChar == 124) {
                        jjstateSet[jjnewStateCnt++] = 8;
                    }
                    break;
                case 10:
                    if (curChar == 84 && kind > 10) {
                        kind = 10;
                    }
                    break;
                case 11:
                    if (curChar == 79) {
                        jjstateSet[jjnewStateCnt++] = 10;
                    }
                    break;
                case 12:
                    if (curChar == 78) {
                        jjstateSet[jjnewStateCnt++] = 11;
                    }
                    break;
                case 15:
                    if ((0xffffffffefffffffLL & l) != 0) {
                        jjCheckNAddStates(0, 2);
                    }
                    break;
                case 16:
                    if (curChar == 92) {
                        jjstateSet[jjnewStateCnt++] = 17;
                    }
                    break;
                case 17:
                    jjCheckNAddStates(0, 2);
                    break;
                case 19:
                    if (curChar != 126) {
                        break;
                    }
                    if (kind > 20) {
                        kind = 20;
                    }
                    jjstateSet[jjnewStateCnt++] = 20;
                    break;
                case 24:
                    if ((0x97ffffff87ffffffLL & l) == 0) {
                        break;
                    }
                    if (kind > 22) {
                        kind = 22;
                    }
                    jjCheckNAddTwoStates(25, 26);
                    break;
                case 25:
                    if ((0x97ffffff87ffffffLL & l) == 0) {
                        break;
                    }
                    if (kind > 22) {
                        kind = 22;
                    }
                    jjCheckNAddTwoStates(25, 26);
                    break;
                case 26:
                    if (curChar == 92) {
                        jjCheckNAddTwoStates(27, 27);
                    }
                    break;
                case 27:
                    if (kind > 22) {
                        kind = 22;
                    }
                    jjCheckNAddTwoStates(25, 26);
                    break;
                case 28:
                    if ((0x97ffffff87ffffffLL & l) == 0) {
                        break;
                    }
                    if (kind > 19) {
                        kind = 19;
                    }
                    jjCheckNAddStates(3, 7);
                    break;
                case 29:
                    if ((0x97ffffff87ffffffLL & l) == 0) {
                        break;
                    }
                    if (kind > 19) {
                        kind = 19;
                    }
                    jjCheckNAddTwoStates(29, 30);
                    break;
                case 30:
                    if (curChar == 92) {
                        jjCheckNAddTwoStates(31, 31);
                    }
                    break;
                case 31:
                    if (kind > 19) {
                        kind = 19;
                    }
                    jjCheckNAddTwoStates(29, 30);
                    break;
                case 32:
                    if ((0x97ffffff87ffffffLL & l) != 0) {
                        jjCheckNAddStates(10, 12);
                    }
                    break;
                case 33:
                    if (curChar == 92) {
                        jjCheckNAddTwoStates(34, 34);
                    }
                    break;
                case 34:
                    jjCheckNAddStates(10, 12);
                    break;
                case 35:
                    if (curChar == 92) {
                        jjCheckNAddStates(13, 15);
                    }
                    break;
                default:
                    break;
                }
            } while (i != startsAt);
        } else {
            int32_t hiByte = (int32_t)(curChar >> 8);
            int32_t i1 = hiByte >> 6;
            int64_t l1 = (int64_t)1 << (hiByte & 077);
            int32_t i2 = (curChar & 0xff) >> 6;
            int64_t l2 = (int64_t)1 << (curChar & 077);
            do {
                switch (jjstateSet[--i]) {
                case 36:
                case 25:
                    if (!jjCanMove_2(hiByte, i1, i2, l1, l2)) {
                        break;
                    }
                    if (kind > 22) {
                        kind = 22;
                    }
                    jjCheckNAddTwoStates(25, 26);
                    break;
                case 0:
                    if (jjCanMove_0(hiByte, i1, i2, l1, l2)) {
                        if (kind > 7) {
                            kind = 7;
                        }
                    }
                    if (jjCanMove_2(hiByte, i1, i2, l1, l2)) {
                        if (kind > 22) {
                            kind = 22;
                        }
                        jjCheckNAddTwoStates(25, 26);
                    }
                    if (jjCanMove_2(hiByte, i1, i2, l1, l2)) {
                        if (kind > 19) {
                            kind = 19;
                        }
                        jjCheckNAddStates(3, 7);
                    }
                    break;
                case 15:
                case 17:
                    if (jjCanMove_1(hiByte, i1, i2, l1, l2)) {
                        jjCheckNAddStates(0, 2);
                    }
                    break;
                case 24:
                    if (!jjCanMove_2(hiByte, i1, i2, l1, l2)) {
                        break;
                    }
                    if (kind > 22) {
                        kind = 22;
                    }
                    jjCheckNAddTwoStates(25, 26);
                    break;
                case 27:
                    if (!jjCanMove_1(hiByte, i1, i2, l1, l2)) {
                        break;
                    }
                    if (kind > 22) {
                        kind = 22;
                    }
                    jjCheckNAddTwoStates(25, 26);
                    break;
                case 28:
                    if (!jjCanMove_2(hiByte, i1, i2, l1, l2)) {
                        break;
                    }
                    if (kind > 19) {
                        kind = 19;
                    }
                    jjCheckNAddStates(3, 7);
                    break;
                case 29:
                    if (!jjCanMove_2(hiByte, i1, i2, l1, l2)) {
                        break;
                    }
                    if (kind > 19) {
                        kind = 19;
                    }
                    jjCheckNAddTwoStates(29, 30);
                    break;
                case 31:
                    if (!jjCanMove_1(hiByte, i1, i2, l1, l2)) {
                        break;
                    }
                    if (kind > 19) {
                        kind = 19;
                    }
                    jjCheckNAddTwoStates(29, 30);
                    break;
                case 32:
                    if (jjCanMove_2(hiByte, i1, i2, l1, l2)) {
                        jjCheckNAddStates(10, 12);
                    }
                    break;
                case 34:
                    if (jjCanMove_1(hiByte, i1, i2, l1, l2)) {
                        jjCheckNAddStates(10, 12);
                    }
                    break;
                default:
                    break;
                }
            } while (i != startsAt);
        }
        if (kind != 0x7fffffff) {
            jjmatchedKind = kind;
            jjmatchedPos = curPos;
            kind = 0x7fffffff;
        }
        ++curPos;
        i = jjnewStateCnt;
        jjnewStateCnt = startsAt;
        if (i == (startsAt = 36 - jjnewStateCnt)) {
            return curPos;
        }
        try {
            curChar = input_stream->readChar();
        } catch (IOException&) {
            return curPos;
        }
    }
}

int32_t QueryParserTokenManager::jjStopStringLiteralDfa_1(int32_t pos, int64_t active0) {
    switch (pos) {
    case 0:
        if ((active0 & 0x40000000LL) != 0) {
            jjmatchedKind = 33;
            return 6;
        }
        return -1;
    default:
        return -1;
    }
}

int32_t QueryParserTokenManager::jjStartNfa_1(int32_t pos, int64_t active0) {
    return jjMoveNfa_1(jjStopStringLiteralDfa_1(pos, active0), pos + 1);
}

int32_t QueryParserTokenManager::jjMoveStringLiteralDfa0_1() {
    switch (curChar) {
    case 84:
        return jjMoveStringLiteralDfa1_1(0x40000000LL);
    case 125:
        return jjStopAtPos(0, 31);
    default:
        return jjMoveNfa_1(0, 0);
    }
}

int32_t QueryParserTokenManager::jjMoveStringLiteralDfa1_1(int64_t active0) {
    try {
        curChar = input_stream->readChar();
    } catch (IOException&) {
        jjStopStringLiteralDfa_1(0, active0);
        return 1;
    }
    switch (curChar) {
    case 79:
        if ((active0 & 0x40000000LL) != 0) {
            return jjStartNfaWithStates_1(1, 30, 6);
        }
        break;
    default:
        break;
    }
    return jjStartNfa_1(0, active0);
}

int32_t QueryParserTokenManager::jjStartNfaWithStates_1(int32_t pos, int32_t kind, int32_t state) {
    jjmatchedKind = kind;
    jjmatchedPos = pos;
    try {
        curChar = input_stream->readChar();
    } catch (IOException&) {
        return pos + 1;
    }
    return jjMoveNfa_1(state, pos + 1);
}

int32_t QueryParserTokenManager::jjMoveNfa_1(int32_t startState, int32_t curPos) {
    int32_t startsAt = 0;
    jjnewStateCnt = 7;
    int32_t i = 1;
    jjstateSet[0] = startState;
    int32_t kind = 0x7fffffff;
    while (true) {
        if (++jjround == 0x7fffffff) {
            ReInitRounds();
        }
        if (curChar < 64) {
            int64_t l = (int64_t)1 << curChar;
            do {
                switch (jjstateSet[--i]) {
                case 0:
                    if ((0xfffffffeffffffffLL & l) != 0) {
                        if (kind > 33) {
                            kind = 33;
                        }
                        jjCheckNAdd(6);
                    }
                    if ((0x100002600LL & l) != 0) {
                        if (kind > 7) {
                            kind = 7;
                        }
                    } else if (curChar == 34) {
                        jjCheckNAddTwoStates(2, 4);
                    }
                    break;
                case 1:
                    if (curChar == 34) {
                        jjCheckNAddTwoStates(2, 4);
                    }
                    break;
                case 2:
                    if ((0xfffffffbffffffffLL & l) != 0) {
                        jjCheckNAddStates(16, 18);
                    }
                    break;
                case 3:
                    if (curChar == 34) {
                        jjCheckNAddStates(16, 18);
                    }
                    break;
                case 5:
                    if (curChar == 34 && kind > 32) {
                        kind = 32;
                    }
                    break;
                case 6:
                    if ((0xfffffffeffffffffLL & l) == 0) {
                        break;
                    }
                    if (kind > 33) {
                        kind = 33;
                    }
                    jjCheckNAdd(6);
                    break;
                default:
                    break;
                }
            } while (i != startsAt);
        } else if (curChar < 128) {
            int64_t l = (int64_t)1 << (curChar & 077);
            do {
                switch (jjstateSet[--i]) {
                case 0:
                case 6:
                    if ((0xdfffffffffffffffLL & l) == 0) {
                        break;
                    }
                    if (kind > 33) {
                        kind = 33;
                    }
                    jjCheckNAdd(6);
                    break;
                case 2:
                    jjAddStates(16, 18);
                    break;
                case 4:
                    if (curChar == 92) {
                        jjstateSet[jjnewStateCnt++] = 3;
                    }
                    break;
                default:
                    break;
                }
            } while (i != startsAt);
        } else {
            int32_t hiByte = (int32_t)(curChar >> 8);
            int32_t i1 = hiByte >> 6;
            int64_t l1 = (int64_t)1 << (hiByte & 077);
            int32_t i2 = (curChar & 0xff) >> 6;
            int64_t l2 = (int64_t)1 << (curChar & 077);
            do {
                switch (jjstateSet[--i]) {
                case 0:
                    if (jjCanMove_0(hiByte, i1, i2, l1, l2)) {
                        if (kind > 7) {
                            kind = 7;
                        }
                    }
                    if (jjCanMove_1(hiByte, i1, i2, l1, l2)) {
                        if (kind > 33) {
                            kind = 33;
                        }
                        jjCheckNAdd(6);
                    }
                    break;
                case 2:
                    if (jjCanMove_1(hiByte, i1, i2, l1, l2)) {
                        jjAddStates(16, 18);
                    }
                    break;
                case 6:
                    if (!jjCanMove_1(hiByte, i1, i2, l1, l2)) {
                        break;
                    }
                    if (kind > 33) {
                        kind = 33;
                    }
                    jjCheckNAdd(6);
                    break;
                default:
                    break;
                }
            } while (i != startsAt);
        }
        if (kind != 0x7fffffff) {
            jjmatchedKind = kind;
            jjmatchedPos = curPos;
            kind = 0x7fffffff;
        }
        ++curPos;
        i = jjnewStateCnt;
        jjnewStateCnt = startsAt;
        if (i == (startsAt = 7 - jjnewStateCnt)) {
            return curPos;
        }
        try {
            curChar = input_stream->readChar();
        } catch (IOException&) {
            return curPos;
        }
    }
}

int32_t QueryParserTokenManager::jjMoveStringLiteralDfa0_0() {
    return jjMoveNfa_0(0, 0);
}

int32_t QueryParserTokenManager::jjMoveNfa_0(int32_t startState, int32_t curPos) {
    int32_t startsAt = 0;
    jjnewStateCnt = 3;
    int32_t i = 1;
    jjstateSet[0] = startState;
    int32_t kind = 0x7fffffff;
    while (true) {
        if (++jjround == 0x7fffffff) {
            ReInitRounds();
        }
        if (curChar < 64) {
            int64_t l = (int64_t)1 << curChar;
            do {
                switch (jjstateSet[--i]) {
                case 0:
                    if ((0x3ff000000000000LL & l) == 0) {
                        break;
                    }
                    if (kind > 25) {
                        kind = 25;
                    }
                    jjAddStates(19, 20);
                    break;
                case 1:
                    if (curChar == 46) {
                        jjCheckNAdd(2);
                    }
                    break;
                case 2:
                    if ((0x3ff000000000000LL & l) == 0) {
                        break;
                    }
                    if (kind > 25) {
                        kind = 25;
                    }
                    jjCheckNAdd(2);
                    break;
                default:
                    break;
                }
            } while (i != startsAt);
        } else if (curChar < 128) {
            int64_t l = (int64_t)1 << (curChar & 077);
            do {
                jjstateSet[--i];
            } while (i != startsAt);
        } else {
            int32_t hiByte = (int32_t)(curChar >> 8);
            int32_t i1 = hiByte >> 6;
            int64_t l1 = (int64_t)1 << (hiByte & 077);
            int32_t i2 = (curChar & 0xff) >> 6;
            int64_t l2 = (int64_t)1 << (curChar & 077);
            do {
                jjstateSet[--i];
            } while (i != startsAt);
        }
        if (kind != 0x7fffffff) {
            jjmatchedKind = kind;
            jjmatchedPos = curPos;
            kind = 0x7fffffff;
        }
        ++curPos;
        i = jjnewStateCnt;
        jjnewStateCnt = startsAt;
        if (i == (startsAt = 3 - jjnewStateCnt)) {
            return curPos;
        }
        try {
            curChar = input_stream->readChar();
        } catch (IOException&) {
            return curPos;
        }
    }
}

int32_t QueryParserTokenManager::jjStopStringLiteralDfa_2(int32_t pos, int64_t active0) {
    switch (pos) {
    case 0:
        if ((active0 & 0x4000000LL) != 0) {
            jjmatchedKind = 29;
            return 6;
        }
        return -1;
    default:
        return -1;
    }
}

int32_t QueryParserTokenManager::jjStartNfa_2(int32_t pos, int64_t active0) {
    return jjMoveNfa_2(jjStopStringLiteralDfa_2(pos, active0), pos + 1);
}

int32_t QueryParserTokenManager::jjMoveStringLiteralDfa0_2() {
    switch (curChar) {
    case 84:
        return jjMoveStringLiteralDfa1_2(0x4000000LL);
    case 93:
        return jjStopAtPos(0, 27);
    default:
        return jjMoveNfa_2(0, 0);
    }
}

int32_t QueryParserTokenManager::jjMoveStringLiteralDfa1_2(int64_t active0) {
    try {
        curChar = input_stream->readChar();
    } catch (IOException&) {
        jjStopStringLiteralDfa_2(0, active0);
        return 1;
    }
    switch (curChar) {
    case 79:
        if ((active0 & 0x4000000LL) != 0) {
            return jjStartNfaWithStates_2(1, 26, 6);
        }
        break;
    default:
        break;
    }
    return jjStartNfa_2(0, active0);
}

int32_t QueryParserTokenManager::jjStartNfaWithStates_2(int32_t pos, int32_t kind, int32_t state) {
    jjmatchedKind = kind;
    jjmatchedPos = pos;
    try {
        curChar = input_stream->readChar();
    } catch (IOException&) {
        return pos + 1;
    }
    return jjMoveNfa_2(state, pos + 1);
}

int32_t QueryParserTokenManager::jjMoveNfa_2(int32_t startState, int32_t curPos) {
    int32_t startsAt = 0;
    jjnewStateCnt = 7;
    int32_t i = 1;
    jjstateSet[0] = startState;
    int32_t kind = 0x7fffffff;
    while (true) {
        if (++jjround == 0x7fffffff) {
            ReInitRounds();
        }
        if (curChar < 64) {
            int64_t l = (int64_t)1 << curChar;
            do {
                switch (jjstateSet[--i]) {
                case 0:
                    if ((0xfffffffeffffffffLL & l) != 0) {
                        if (kind > 29) {
                            kind = 29;
                        }
                        jjCheckNAdd(6);
                    }
                    if ((0x100002600LL & l) != 0) {
                        if (kind > 7) {
                            kind = 7;
                        }
                    } else if (curChar == 34) {
                        jjCheckNAddTwoStates(2, 4);
                    }
                    break;
                case 1:
                    if (curChar == 34) {
                        jjCheckNAddTwoStates(2, 4);
                    }
                    break;
                case 2:
                    if ((0xfffffffbffffffffLL & l) != 0) {
                        jjCheckNAddStates(16, 18);
                    }
                    break;
                case 3:
                    if (curChar == 34) {
                        jjCheckNAddStates(16, 18);
                    }
                    break;
                case 5:
                    if (curChar == 34 && kind > 28) {
                        kind = 28;
                    }
                    break;
                case 6:
                    if ((0xfffffffeffffffffLL & l) == 0) {
                        break;
                    }
                    if (kind > 29) {
                        kind = 29;
                    }
                    jjCheckNAdd(6);
                    break;
                default:
                    break;
                }
            } while (i != startsAt);
        } else if (curChar < 128) {
            int64_t l = (int64_t)1 << (curChar & 077);
            do {
                switch (jjstateSet[--i]) {
                case 0:
                case 6:
                    if ((0xffffffffdfffffffLL & l) == 0) {
                        break;
                    }
                    if (kind > 29) {
                        kind = 29;
                    }
                    jjCheckNAdd(6);
                    break;
                case 2:
                    jjAddStates(16, 18);
                    break;
                case 4:
                    if (curChar == 92) {
                        jjstateSet[jjnewStateCnt++] = 3;
                    }
                    break;
                default:
                    break;
                }
            } while (i != startsAt);
        } else {
            int32_t hiByte = (int32_t)(curChar >> 8);
            int32_t i1 = hiByte >> 6;
            int64_t l1 = (int64_t)1 << (hiByte & 077);
            int32_t i2 = (curChar & 0xff) >> 6;
            int64_t l2 = (int64_t)1 << (curChar & 077);
            do {
                switch (jjstateSet[--i]) {
                case 0:
                    if (jjCanMove_0(hiByte, i1, i2, l1, l2)) {
                        if (kind > 7) {
                            kind = 7;
                        }
                    }
                    if (jjCanMove_1(hiByte, i1, i2, l1, l2)) {
                        if (kind > 29) {
                            kind = 29;
                        }
                        jjCheckNAdd(6);
                    }
                    break;
                case 2:
                    if (jjCanMove_1(hiByte, i1, i2, l1, l2)) {
                        jjAddStates(16, 18);
                    }
                    break;
                case 6:
                    if (!jjCanMove_1(hiByte, i1, i2, l1, l2)) {
                        break;
                    }
                    if (kind > 29) {
                        kind = 29;
                    }
                    jjCheckNAdd(6);
                    break;
                default:
                    break;
                }
            } while (i != startsAt);
        }
        if (kind != 0x7fffffff) {
            jjmatchedKind = kind;
            jjmatchedPos = curPos;
            kind = 0x7fffffff;
        }
        ++curPos;
        i = jjnewStateCnt;
        jjnewStateCnt = startsAt;
        if (i == (startsAt = 7 - jjnewStateCnt)) {
            return curPos;
        }
        try {
            curChar = input_stream->readChar();
        } catch (IOException&) {
            return curPos;
        }
    }
}

bool QueryParserTokenManager::jjCanMove_0(int32_t hiByte, int32_t i1, int32_t i2, int64_t l1, int64_t l2) {
    switch (hiByte) {
    case 48:
        return ((jjbitVec0[i2] & l2) != 0);
    default:
        return false;
    }
}

bool QueryParserTokenManager::jjCanMove_1(int32_t hiByte, int32_t i1, int32_t i2, int64_t l1, int64_t l2) {
    switch (hiByte) {
    case 0:
        return ((jjbitVec3[i2] & l2) != 0);
    default:
        if ((jjbitVec1[i1] & l1) != 0) {
            return true;
        }
        return false;
    }
}

bool QueryParserTokenManager::jjCanMove_2(int32_t hiByte, int32_t i1, int32_t i2, int64_t l1, int64_t l2) {
    switch (hiByte) {
    case 0:
        return ((jjbitVec3[i2] & l2) != 0);
    case 48:
        return ((jjbitVec1[i2] & l2) != 0);
    default:
        if ((jjbitVec4[i1] & l1) != 0) {
            return true;
        }
        return false;
    }
}

void QueryParserTokenManager::ReInit(const QueryParserCharStreamPtr& stream) {
    jjmatchedPos = 0;
    jjnewStateCnt = 0;
    curLexState = defaultLexState;
    input_stream = stream;
    ReInitRounds();
}

void QueryParserTokenManager::ReInitRounds() {
    jjround = 0x80000001;
    for (int32_t i = 36; i-- > 0;) {
        jjrounds[i] = 0x80000000;
    }
}

void QueryParserTokenManager::ReInit(const QueryParserCharStreamPtr& stream, int32_t lexState) {
    ReInit(stream);
    SwitchTo(lexState);
}

void QueryParserTokenManager::SwitchTo(int32_t lexState) {
    if (lexState >= 4 || lexState < 0) {
        boost::throw_exception(QueryParserError(L"Error: Ignoring invalid lexical state : " +
                                                StringUtils::toString(lexState) + L". State unchanged."));
    } else {
        curLexState = lexState;
    }
}

QueryParserTokenPtr QueryParserTokenManager::jjFillToken() {
    String im(jjstrLiteralImages[jjmatchedKind]);
    String curTokenImage(im.empty() ? input_stream->GetImage() : im);
    int32_t beginLine = input_stream->getBeginLine();
    int32_t beginColumn = input_stream->getBeginColumn();
    int32_t endLine = input_stream->getEndLine();
    int32_t endColumn = input_stream->getEndColumn();
    QueryParserTokenPtr t(QueryParserToken::newToken(jjmatchedKind, curTokenImage));

    t->beginLine = beginLine;
    t->endLine = endLine;
    t->beginColumn = beginColumn;
    t->endColumn = endColumn;

    return t;
}

QueryParserTokenPtr QueryParserTokenManager::getNextToken() {
    QueryParserTokenPtr matchedToken;
    int32_t curPos = 0;

    while (true) {
        try {
            curChar = input_stream->BeginToken();
        } catch (IOException&) {
            jjmatchedKind = 0;
            matchedToken = jjFillToken();
            return matchedToken;
        }

        switch (curLexState) {
        case 0:
            jjmatchedKind = 0x7fffffff;
            jjmatchedPos = 0;
            curPos = jjMoveStringLiteralDfa0_0();
            break;
        case 1:
            jjmatchedKind = 0x7fffffff;
            jjmatchedPos = 0;
            curPos = jjMoveStringLiteralDfa0_1();
            break;
        case 2:
            jjmatchedKind = 0x7fffffff;
            jjmatchedPos = 0;
            curPos = jjMoveStringLiteralDfa0_2();
            break;
        case 3:
            jjmatchedKind = 0x7fffffff;
            jjmatchedPos = 0;
            curPos = jjMoveStringLiteralDfa0_3();
            break;
        }

        if (jjmatchedKind != 0x7fffffff) {
            if (jjmatchedPos + 1 < curPos) {
                input_stream->backup(curPos - jjmatchedPos - 1);
            }
            if ((jjtoToken[jjmatchedKind >> 6] & ((int64_t)1 << (jjmatchedKind & 077))) != 0) {
                matchedToken = jjFillToken();
                if (jjnewLexState[jjmatchedKind] != -1) {
                    curLexState = jjnewLexState[jjmatchedKind];
                }
                return matchedToken;
            } else {
                if (jjnewLexState[jjmatchedKind] != -1) {
                    curLexState = jjnewLexState[jjmatchedKind];
                }
                continue;
            }
        }
        int32_t error_line = input_stream->getEndLine();
        int32_t error_column = input_stream->getEndColumn();
        String error_after;
        bool EOFSeen = false;
        try {
            input_stream->readChar();
            input_stream->backup(1);
        } catch (IOException&) {
            EOFSeen = true;
            error_after = curPos <= 1 ? L"" : input_stream->GetImage();
            if (curChar == L'\n' || curChar == L'\r') {
                ++error_line;
                error_column = 0;
            } else {
                ++error_column;
            }
        }

        if (!EOFSeen) {
            input_stream->backup(1);
            error_after = curPos <= 1 ? L"" : input_stream->GetImage();
        }

        boost::throw_exception(QueryParserError(QueryParseError::lexicalError(EOFSeen, curLexState, error_line, error_column, error_after, curChar)));
    }
}

void QueryParserTokenManager::jjCheckNAdd(int32_t state) {
    if (jjrounds[state] != jjround) {
        jjstateSet[jjnewStateCnt++] = state;
        jjrounds[state] = jjround;
    }
}

void QueryParserTokenManager::jjAddStates(int32_t start, int32_t end) {
    do {
        jjstateSet[jjnewStateCnt++] = jjnextStates[start];
    } while (start++ != end);
}

void QueryParserTokenManager::jjCheckNAddTwoStates(int32_t state1, int32_t state2) {
    jjCheckNAdd(state1);
    jjCheckNAdd(state2);
}

void QueryParserTokenManager::jjCheckNAddStates(int32_t start, int32_t end) {
    do {
        jjCheckNAdd(jjnextStates[start]);
    } while (start++ != end);
}

}
