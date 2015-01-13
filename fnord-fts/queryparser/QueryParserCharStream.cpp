/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "QueryParserCharStream.h"

namespace Lucene {

wchar_t QueryParserCharStream::readChar() {
    BOOST_ASSERT(false);
    return 0; // override
}

int32_t QueryParserCharStream::getColumn() {
    BOOST_ASSERT(false);
    return 0; // override
}

int32_t QueryParserCharStream::getLine() {
    BOOST_ASSERT(false);
    return 0; // override
}

int32_t QueryParserCharStream::getEndColumn() {
    BOOST_ASSERT(false);
    return 0; // override
}

int32_t QueryParserCharStream::getEndLine() {
    BOOST_ASSERT(false);
    return 0; // override
}

int32_t QueryParserCharStream::getBeginColumn() {
    BOOST_ASSERT(false);
    return 0; // override
}

int32_t QueryParserCharStream::getBeginLine() {
    BOOST_ASSERT(false);
    return 0; // override
}

void QueryParserCharStream::backup(int32_t amount) {
    BOOST_ASSERT(false);
    // override
}

wchar_t QueryParserCharStream::BeginToken() {
    BOOST_ASSERT(false);
    return 0; // override
}

String QueryParserCharStream::GetImage() {
    BOOST_ASSERT(false);
    return L""; // override
}

CharArray QueryParserCharStream::GetSuffix(int32_t length) {
    BOOST_ASSERT(false);
    return CharArray(); // override
}

void QueryParserCharStream::Done() {
    BOOST_ASSERT(false);
    // override
}

}
