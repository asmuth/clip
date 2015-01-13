/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "TermDocs.h"

namespace Lucene {

TermDocs::TermDocs() {
}

void TermDocs::seek(const TermPtr& term) {
    BOOST_ASSERT(false);
    // override
}

void TermDocs::seek(const TermEnumPtr& termEnum) {
    BOOST_ASSERT(false);
    // override
}

int32_t TermDocs::doc() {
    BOOST_ASSERT(false);
    return 0; // override
}

int32_t TermDocs::freq() {
    BOOST_ASSERT(false);
    return 0; // override
}

bool TermDocs::next() {
    BOOST_ASSERT(false);
    return false; // override
}

int32_t TermDocs::read(Collection<int32_t> docs, Collection<int32_t> freqs) {
    BOOST_ASSERT(false);
    return 0; // override
}

bool TermDocs::skipTo(int32_t target) {
    BOOST_ASSERT(false);
    return false; // override
}

void TermDocs::close() {
    BOOST_ASSERT(false);
    // override
}

}
