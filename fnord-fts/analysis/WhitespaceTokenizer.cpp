/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "WhitespaceTokenizer.h"
#include "MiscUtils.h"
#include "UnicodeUtils.h"

namespace Lucene {

WhitespaceTokenizer::WhitespaceTokenizer(const ReaderPtr& input) : CharTokenizer(input) {
}

WhitespaceTokenizer::WhitespaceTokenizer(const AttributeSourcePtr& source, const ReaderPtr& input) : CharTokenizer(source, input) {
}

WhitespaceTokenizer::WhitespaceTokenizer(const AttributeFactoryPtr& factory, const ReaderPtr& input) : CharTokenizer(factory, input) {
}

WhitespaceTokenizer::~WhitespaceTokenizer() {
}

bool WhitespaceTokenizer::isTokenChar(wchar_t c) {
    return !UnicodeUtil::isSpace(c);
}

}
