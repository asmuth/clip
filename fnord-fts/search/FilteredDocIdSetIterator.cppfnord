/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "fnord-fts/LuceneInc.h"
#include "fnord-fts/FilteredDocIdSetIterator.h"

namespace Lucene {

FilteredDocIdSetIterator::FilteredDocIdSetIterator(const DocIdSetIteratorPtr& innerIter) {
    if (!innerIter) {
        boost::throw_exception(IllegalArgumentException(L"null iterator"));
    }
    this->innerIter = innerIter;
    this->doc = -1;
}

FilteredDocIdSetIterator::~FilteredDocIdSetIterator() {
}

int32_t FilteredDocIdSetIterator::docID() {
    return doc;
}

int32_t FilteredDocIdSetIterator::nextDoc() {
    while ((doc = innerIter->nextDoc()) != NO_MORE_DOCS) {
        if (match(doc)) {
            return doc;
        }
    }
    return doc;
}

int32_t FilteredDocIdSetIterator::advance(int32_t target) {
    doc = innerIter->advance(target);
    if (doc != NO_MORE_DOCS) {
        if (match(doc)) {
            return doc;
        } else {
            while ((doc = innerIter->nextDoc()) != NO_MORE_DOCS) {
                if (match(doc)) {
                    return doc;
                }
            }
            return doc;
        }
    }
    return doc;
}

}
