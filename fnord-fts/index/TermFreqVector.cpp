/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "TermFreqVector.h"

namespace Lucene {

TermFreqVector::TermFreqVector() {
}

TermFreqVector::~TermFreqVector() {
}

String TermFreqVector::getField() {
    BOOST_ASSERT(false);
    return L""; // override
}

int32_t TermFreqVector::size() {
    BOOST_ASSERT(false);
    return 0; // override
}

Collection<String> TermFreqVector::getTerms() {
    BOOST_ASSERT(false);
    return Collection<String>(); // override
}

Collection<int32_t> TermFreqVector::getTermFrequencies() {
    BOOST_ASSERT(false);
    return Collection<int32_t>(); // override
}

int32_t TermFreqVector::indexOf(const String& term) {
    BOOST_ASSERT(false);
    return 0; // override
}

Collection<int32_t> TermFreqVector::indexesOf(Collection<String> terms, int32_t start, int32_t length) {
    BOOST_ASSERT(false);
    return Collection<int32_t>(); // override
}

}
