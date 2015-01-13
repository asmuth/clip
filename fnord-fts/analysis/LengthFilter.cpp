/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "LengthFilter.h"
#include "TermAttribute.h"

namespace Lucene {

LengthFilter::LengthFilter(const TokenStreamPtr& input, int32_t min, int32_t max) : TokenFilter(input) {
    this->min = min;
    this->max = max;
    this->termAtt = addAttribute<TermAttribute>();
}

LengthFilter::~LengthFilter() {
}

bool LengthFilter::incrementToken() {
    // return the first non-stop word found
    while (input->incrementToken()) {
        int32_t len = termAtt->termLength();
        if (len >= min && len <= max) {
            return true;
        }
        // note: else we ignore it but should we index each part of it?
    }
    // reached EOS -- return false
    return false;
}

}
