/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "fnord-fts/fts.h"
#include "fnord-fts/analysis/WhitespaceAnalyzer.h"
#include "fnord-fts/analysis/WhitespaceTokenizer.h"

namespace fnord {
namespace fts {

WhitespaceAnalyzer::~WhitespaceAnalyzer() {
}

TokenStreamPtr WhitespaceAnalyzer::tokenStream(const String& fieldName, const ReaderPtr& reader) {
    return newLucene<WhitespaceTokenizer>(reader);
}

TokenStreamPtr WhitespaceAnalyzer::reusableTokenStream(const String& fieldName, const ReaderPtr& reader) {
    TokenizerPtr tokenizer(std::dynamic_pointer_cast<Tokenizer>(getPreviousTokenStream()));
    if (!tokenizer) {
        tokenizer = newLucene<WhitespaceTokenizer>(reader);
        setPreviousTokenStream(tokenizer);
    } else {
        tokenizer->reset(reader);
    }
    return tokenizer;
}

}

}
