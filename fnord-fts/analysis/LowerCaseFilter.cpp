/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "LowerCaseFilter.h"
#include "TermAttribute.h"
#include "CharFolder.h"

namespace Lucene {

LowerCaseFilter::LowerCaseFilter(const TokenStreamPtr& input) : TokenFilter(input) {
    termAtt = addAttribute<TermAttribute>();
}

LowerCaseFilter::~LowerCaseFilter() {
}

bool LowerCaseFilter::incrementToken() {
    if (input->incrementToken()) {
        wchar_t* buffer = termAtt->termBufferArray();
        CharFolder::toLower(buffer, buffer + termAtt->termLength());
        return true;
    }
    return false;
}

}
