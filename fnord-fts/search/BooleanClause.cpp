/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "fnord-fts/fts.h"
#include "fnord-fts/search/BooleanClause.h"
#include "fnord-fts/search/Query.h"

namespace Lucene {

BooleanClause::BooleanClause(const QueryPtr& query, Occur occur) {
    this->query = query;
    this->occur = occur;
}

BooleanClause::~BooleanClause() {
}

BooleanClause::Occur BooleanClause::getOccur() {
    return occur;
}

void BooleanClause::setOccur(BooleanClause::Occur occur) {
    this->occur = occur;
}

QueryPtr BooleanClause::getQuery() {
    return query;
}

void BooleanClause::setQuery(const QueryPtr& query) {
    this->query = query;
}

bool BooleanClause::isProhibited() {
    return (occur == MUST_NOT);
}

bool BooleanClause::isRequired() {
    return (occur == MUST);
}

bool BooleanClause::equals(const LuceneObjectPtr& other) {
    BooleanClausePtr otherBooleanClause(std::dynamic_pointer_cast<BooleanClause>(other));
    if (!otherBooleanClause) {
        return false;
    }
    return (this->query->equals(otherBooleanClause->query) && this->occur == otherBooleanClause->occur);
}

int32_t BooleanClause::hashCode() {
    return query->hashCode() ^ (occur == MUST ? 1 : 0) ^ (occur == MUST_NOT ? 2 : 0);
}

String BooleanClause::toString() {
    switch (occur) {
    case MUST:
        return L"+" + query->toString();
    case MUST_NOT:
        return L"-" + query->toString();
    default:
        return query->toString();
    }
}

}
