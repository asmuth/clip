/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "CachingTokenFilter.h"

namespace Lucene {

CachingTokenFilter::CachingTokenFilter(const TokenStreamPtr& input) : TokenFilter(input) {
}

CachingTokenFilter::~CachingTokenFilter() {
}

bool CachingTokenFilter::incrementToken() {
    if (!cache) {
        // fill cache lazily
        cache = Collection<AttributeSourceStatePtr>::newInstance();
        fillCache();
        iterator = cache.begin();
    }

    if (iterator == cache.end()) {
        // the cache is exhausted, return false
        return false;
    }

    // Since the TokenFilter can be reset, the tokens need to be preserved as immutable.
    restoreState(*iterator++);
    return true;
}

void CachingTokenFilter::end() {
    if (finalState) {
        restoreState(finalState);
    }
}

void CachingTokenFilter::reset() {
    if (cache) {
        iterator = cache.begin();
    }
}

void CachingTokenFilter::fillCache() {
    while (input->incrementToken()) {
        cache.add(captureState());
    }
    // capture final state
    input->end();
    finalState = captureState();
}

}
