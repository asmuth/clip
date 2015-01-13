/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "LowerCaseTokenizer.h"
#include "CharFolder.h"

namespace Lucene {

LowerCaseTokenizer::LowerCaseTokenizer(const ReaderPtr& input) : LetterTokenizer(input) {
}

LowerCaseTokenizer::LowerCaseTokenizer(const AttributeSourcePtr& source, const ReaderPtr& input) : LetterTokenizer(source, input) {
}

LowerCaseTokenizer::LowerCaseTokenizer(const AttributeFactoryPtr& factory, const ReaderPtr& input) : LetterTokenizer(factory, input) {
}

LowerCaseTokenizer::~LowerCaseTokenizer() {
}

wchar_t LowerCaseTokenizer::normalize(wchar_t c) {
    return CharFolder::toLower(c);
}

}
