/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "fnord-fts/LuceneInc.h"
#include "fnord-fts/analysis/PorterStemFilter.h"
#include "fnord-fts/analysis/PorterStemmer.h"
#include "fnord-fts/analysis/tokenattributes/TermAttribute.h"

namespace Lucene {

PorterStemFilter::PorterStemFilter(const TokenStreamPtr& input) : TokenFilter(input) {
    stemmer = newLucene<PorterStemmer>();
    termAtt = addAttribute<TermAttribute>();
}

PorterStemFilter::~PorterStemFilter() {
}

bool PorterStemFilter::incrementToken() {
    if (!input->incrementToken()) {
        return false;
    }

    if (stemmer->stem(termAtt->termBuffer())) {
        termAtt->setTermBuffer(stemmer->getResultBuffer(), 0, stemmer->getResultLength());
    }
    return true;
}

}
