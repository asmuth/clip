/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "QueryParserConstants.h"

namespace Lucene {

const wchar_t* QueryParserConstants::_tokenImage[] = {
    L"<EOF>",
    L"<_NUM_CHAR>",
    L"<_ESCAPED_CHAR>",
    L"<_TERM_START_CHAR>",
    L"<_TERM_CHAR>",
    L"<_WHITESPACE>",
    L"<_QUOTED_CHAR>",
    L"<token of kind 7>",
    L"<AND>",
    L"<OR>",
    L"<NOT>",
    L"\"+\"",
    L"\"-\"",
    L"\"(\"",
    L"\")\"",
    L"\":\"",
    L"\"*\"",
    L"\"^\"",
    L"<QUOTED>",
    L"<TERM>",
    L"<FUZZY_SLOP>",
    L"<PREFIXTERM>",
    L"<WILDTERM>",
    L"\"[\"",
    L"\"{\"",
    L"<NUMBER>",
    L"\"TO\"",
    L"\"]\"",
    L"<RANGEIN_QUOTED>",
    L"<RANGEIN_GOOP>",
    L"\"TO\"",
    L"\"}\"",
    L"<RANGEEX_QUOTED>",
    L"<RANGEEX_GOOP>"
};
Collection<String> QueryParserConstants::tokenImage = Collection<String>::newInstance(_tokenImage, _tokenImage + SIZEOF_ARRAY(_tokenImage));

QueryParserConstants::QueryParserConstants() {
}

QueryParserConstants::~QueryParserConstants() {
}

}
