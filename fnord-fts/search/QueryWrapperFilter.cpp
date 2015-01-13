/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "fnord-fts/LuceneInc.h"
#include "fnord-fts/QueryWrapperFilter.h"
#include "fnord-fts/_QueryWrapperFilter.h"
#include "fnord-fts/Query.h"
#include "fnord-fts/Weight.h"
#include "fnord-fts/Scorer.h"
#include "fnord-fts/IndexSearcher.h"

namespace Lucene {

QueryWrapperFilter::QueryWrapperFilter(const QueryPtr& query) {
    this->query = query;
}

QueryWrapperFilter::~QueryWrapperFilter() {
}

DocIdSetPtr QueryWrapperFilter::getDocIdSet(const IndexReaderPtr& reader) {
    WeightPtr weight(query->weight(newLucene<IndexSearcher>(reader)));
    return newLucene<QueryWrapperFilterDocIdSet>(reader, weight);
}

String QueryWrapperFilter::toString() {
    return L"QueryWrapperFilter(" + query->toString() + L")";
}

bool QueryWrapperFilter::equals(const LuceneObjectPtr& other) {
    QueryWrapperFilterPtr otherQueryWrapperFilter(boost::dynamic_pointer_cast<QueryWrapperFilter>(other));
    if (!otherQueryWrapperFilter) {
        return false;
    }
    return this->query->equals(otherQueryWrapperFilter->query);
}

int32_t QueryWrapperFilter::hashCode() {
    return query->hashCode() ^ 0x923F64B9;
}

QueryWrapperFilterDocIdSet::QueryWrapperFilterDocIdSet(const IndexReaderPtr& reader, const WeightPtr& weight) {
    this->reader = reader;
    this->weight = weight;
}

QueryWrapperFilterDocIdSet::~QueryWrapperFilterDocIdSet() {
}

DocIdSetIteratorPtr QueryWrapperFilterDocIdSet::iterator() {
    return weight->scorer(reader, true, false);
}

bool QueryWrapperFilterDocIdSet::isCacheable() {
    return false;
}

}
