/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "fnord-fts/LuceneInc.h"
#include "fnord-fts/search/FieldCacheTermsFilter.h"
#include "fnord-fts/search/_FieldCacheTermsFilter.h"
#include "fnord-fts/search/FieldCache.h"
#include "fnord-fts/util/OpenBitSet.h"

namespace Lucene {

FieldCacheTermsFilter::FieldCacheTermsFilter(const String& field, Collection<String> terms) {
    this->field = field;
    this->terms = terms;
}

FieldCacheTermsFilter::~FieldCacheTermsFilter() {
}

FieldCachePtr FieldCacheTermsFilter::getFieldCache() {
    return FieldCache::DEFAULT();
}

DocIdSetPtr FieldCacheTermsFilter::getDocIdSet(const IndexReaderPtr& reader) {
    return newLucene<FieldCacheTermsFilterDocIdSet>(terms, getFieldCache()->getStringIndex(reader, field));
}

FieldCacheTermsFilterDocIdSet::FieldCacheTermsFilterDocIdSet(Collection<String> terms, const StringIndexPtr& fcsi) {
    this->fcsi = fcsi;
    openBitSet = newLucene<OpenBitSet>(this->fcsi->lookup.size());
    for (Collection<String>::iterator term = terms.begin(); term != terms.end(); ++term) {
        int32_t termNumber = this->fcsi->binarySearchLookup(*term);
        if (termNumber > 0) {
            openBitSet->set(termNumber);
        }
    }
}

FieldCacheTermsFilterDocIdSet::~FieldCacheTermsFilterDocIdSet() {
}

DocIdSetIteratorPtr FieldCacheTermsFilterDocIdSet::iterator() {
    return newLucene<FieldCacheTermsFilterDocIdSetIterator>(fcsi, openBitSet);
}

bool FieldCacheTermsFilterDocIdSet::isCacheable() {
    return true;
}

FieldCacheTermsFilterDocIdSetIterator::FieldCacheTermsFilterDocIdSetIterator(const StringIndexPtr& fcsi, const OpenBitSetPtr& openBitSet) {
    this->fcsi = fcsi;
    this->openBitSet = openBitSet;
    this->doc = -1;
}

FieldCacheTermsFilterDocIdSetIterator::~FieldCacheTermsFilterDocIdSetIterator() {
}

int32_t FieldCacheTermsFilterDocIdSetIterator::docID() {
    return doc;
}

int32_t FieldCacheTermsFilterDocIdSetIterator::nextDoc() {
    try {
        if (++doc >= fcsi->order.size()) {
            boost::throw_exception(IndexOutOfBoundsException());
        }
        while (!openBitSet->fastGet(fcsi->order[doc])) {
            if (++doc >= fcsi->order.size()) {
                boost::throw_exception(IndexOutOfBoundsException());
            }
        }
    } catch (IndexOutOfBoundsException&) {
        doc = NO_MORE_DOCS;
    }
    return doc;
}

int32_t FieldCacheTermsFilterDocIdSetIterator::advance(int32_t target) {
    try {
        doc = target;
        if (doc < 0 || doc >= fcsi->order.size()) {
            boost::throw_exception(IndexOutOfBoundsException());
        }
        while (!openBitSet->fastGet(fcsi->order[doc])) {
            if (++doc >= fcsi->order.size()) {
                boost::throw_exception(IndexOutOfBoundsException());
            }
        }
    } catch (IndexOutOfBoundsException&) {
        doc = NO_MORE_DOCS;
    }
    return doc;
}

}
