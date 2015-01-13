/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "StandardFilter.h"
#include "StandardTokenizer.h"
#include "TermAttribute.h"
#include "TypeAttribute.h"

namespace Lucene {

StandardFilter::StandardFilter(const TokenStreamPtr& input) : TokenFilter(input) {
    termAtt = addAttribute<TermAttribute>();
    typeAtt = addAttribute<TypeAttribute>();
}

StandardFilter::~StandardFilter() {
}

const String& StandardFilter::APOSTROPHE_TYPE() {
    static String _APOSTROPHE_TYPE;
    if (_APOSTROPHE_TYPE.empty()) {
        _APOSTROPHE_TYPE = StandardTokenizer::TOKEN_TYPES()[StandardTokenizer::APOSTROPHE];
    }
    return _APOSTROPHE_TYPE;
}

const String& StandardFilter::ACRONYM_TYPE() {
    static String _ACRONYM_TYPE;
    if (_ACRONYM_TYPE.empty()) {
        _ACRONYM_TYPE = StandardTokenizer::TOKEN_TYPES()[StandardTokenizer::ACRONYM];
    }
    return _ACRONYM_TYPE;
}

bool StandardFilter::incrementToken() {
    if (!input->incrementToken()) {
        return false;
    }

    wchar_t* termBuffer = termAtt->termBufferArray();
    int32_t bufferLength = termAtt->termLength();
    String type(typeAtt->type());

    if (type == APOSTROPHE_TYPE() && bufferLength >= 2 && termBuffer[bufferLength - 2] == L'\'' &&
            (termBuffer[bufferLength - 1] == L's' || termBuffer[bufferLength - 1] == L'S')) { // remove 's
        // Strip last 2 characters off
        termAtt->setTermLength(bufferLength - 2);
    } else if (type == ACRONYM_TYPE()) { // remove dots
        int32_t upto = 0;
        for (int32_t i = 0; i < bufferLength; ++i) {
            wchar_t c = termBuffer[i];
            if (c != L'.') {
                termBuffer[upto++] = c;
            }
        }
        termAtt->setTermLength(upto);
    }

    return true;
}

}
