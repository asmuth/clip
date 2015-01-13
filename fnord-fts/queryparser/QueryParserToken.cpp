/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "QueryParserToken.h"

namespace Lucene {

QueryParserToken::QueryParserToken(int32_t kind, const String& image) {
    this->kind = kind;
    this->image = image;
    this->beginLine = 0;
    this->beginColumn = 0;
    this->endLine = 0;
    this->endColumn = 0;
}

QueryParserToken::~QueryParserToken() {
}

String QueryParserToken::toString() {
    return image;
}

QueryParserTokenPtr QueryParserToken::newToken(int32_t ofKind, const String& image) {
    return newLucene<QueryParserToken>(ofKind, image);
}

}
