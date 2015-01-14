/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "fnord-fts/fts.h"
#include "fnord-fts/analysis/KeywordAnalyzer.h"
#include "fnord-fts/analysis/KeywordTokenizer.h"

namespace Lucene {

KeywordAnalyzer::~KeywordAnalyzer() {
}

TokenStreamPtr KeywordAnalyzer::tokenStream(const String& fieldName, const ReaderPtr& reader) {
    return newLucene<KeywordTokenizer>(reader);
}

TokenStreamPtr KeywordAnalyzer::reusableTokenStream(const String& fieldName, const ReaderPtr& reader) {
    TokenizerPtr tokenizer(std::dynamic_pointer_cast<Tokenizer>(getPreviousTokenStream()));
    if (!tokenizer) {
        tokenizer = newLucene<KeywordTokenizer>(reader);
        setPreviousTokenStream(tokenizer);
    } else {
        tokenizer->reset(reader);
    }
    return tokenizer;
}

}
