/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "FormatPostingsTermsConsumer.h"
#include "UTF8Stream.h"
#include "MiscUtils.h"

namespace Lucene {

FormatPostingsTermsConsumer::~FormatPostingsTermsConsumer() {
}

FormatPostingsDocsConsumerPtr FormatPostingsTermsConsumer::addTerm(const String& text) {
    int32_t len = text.length();
    if (!termBuffer) {
        termBuffer = CharArray::newInstance(MiscUtils::getNextSize(len + 1));
    }
    if (termBuffer.size() < len + 1) {
        termBuffer.resize(MiscUtils::getNextSize(len + 1));
    }
    MiscUtils::arrayCopy(text.begin(), 0, termBuffer.get(), 0, len);
    termBuffer[len] = UTF8Base::UNICODE_TERMINATOR;
    return addTerm(termBuffer, 0);
}

}
