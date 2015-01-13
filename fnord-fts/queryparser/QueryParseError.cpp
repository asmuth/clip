/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "QueryParseError.h"
#include "QueryParserToken.h"
#include "StringUtils.h"

namespace Lucene {

QueryParseError::~QueryParseError() {
}

String QueryParseError::lexicalError(bool EOFSeen, int32_t lexState, int32_t errorLine, int32_t errorColumn,
                                     const String& errorAfter, wchar_t curChar) {
    StringStream buffer;
    buffer << L"Lexical error at line " << errorLine << L", column " << errorColumn << L".  Encountered:";
    if (EOFSeen) {
        buffer << L"<EOF>";
    } else {
        buffer << L"\"" << addEscapes(String(1, curChar)) << L"\"";
    }
    buffer << L" (" << (int32_t)curChar << L"), after : \"" << addEscapes(errorAfter) + L"\"";
    return buffer.str();
}

String QueryParseError::parseError(const QueryParserTokenPtr& currentToken, Collection< Collection<int32_t> > expectedTokenSequences,
                                   Collection<String> tokenImage) {
    StringStream expected;
    int32_t maxSize = 0;
    for (int32_t i = 0; i < expectedTokenSequences.size(); ++i) {
        if (maxSize < expectedTokenSequences[i].size()) {
            maxSize = expectedTokenSequences[i].size();
        }
        for (int32_t j = 0; j < expectedTokenSequences[i].size(); ++j) {
            expected << tokenImage[expectedTokenSequences[i][j]] << L" ";
        }
        if (expectedTokenSequences[i][expectedTokenSequences[i].size() - 1] != 0) {
            expected << L"...";
        }
        expected << L"\n    ";
    }
    StringStream retval;
    retval << L"Encountered \"";
    QueryParserTokenPtr token(currentToken->next);
    for (int32_t i = 0; i < maxSize; ++i) {
        if (i != 0) {
            retval << L" ";
        }
        if (token->kind == 0) {
            retval << tokenImage[0];
            break;
        }
        retval << L" " << tokenImage[token->kind] << L" \"" << addEscapes(token->image) << L" \"";
        token = token->next;
    }
    retval << L"\" at line " << currentToken->next->beginLine << L", column " << currentToken->next->beginColumn;
    retval << L".\n";
    if (expectedTokenSequences.size() == 1) {
        retval << L"Was expecting:\n    ";
    } else {
        retval << L"Was expecting one of:\n    ";
    }
    retval << expected.str();
    return retval.str();
}

String QueryParseError::addEscapes(const String& str) {
    StringStream buffer;
    for (String::const_iterator ch = str.begin(); ch != str.end(); ++ch) {
        switch (*ch) {
        case L'\0':
            continue;
        case L'\b':
            buffer << L"\\b";
            continue;
        case L'\t':
            buffer << L"\\t";
            continue;
        case L'\n':
            buffer << L"\\n";
            continue;
        case L'\f':
            buffer << L"\\f";
            continue;
        case L'\r':
            buffer << L"\\r";
            continue;
        case L'\"':
            buffer << L"\\\"";
            continue;
        case L'\'':
            buffer << L"\\\'";
            continue;
        case L'\\':
            buffer << L"\\\\";
            continue;
        default:
            if (*ch < 0x20 || *ch > 0x7e) {
                String hexChar(L"0000" + StringUtils::toString(*ch, 16));
                buffer << L"\\u" + hexChar.substr(hexChar.length() - 4);
            } else {
                buffer << *ch;
            }
            continue;
        }
    }
    return buffer.str();
}

}
