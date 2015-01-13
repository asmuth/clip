/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "SpanNearQuery.h"
#include "SpanQuery.h"
#include "SpanOrQuery.h"
#include "NearSpansOrdered.h"
#include "NearSpansUnordered.h"
#include "MiscUtils.h"

namespace Lucene {

SpanNearQuery::SpanNearQuery(Collection<SpanQueryPtr> clauses, int32_t slop, bool inOrder, bool collectPayloads) {
    this->clauses = Collection<SpanQueryPtr>::newInstance();
    for (int32_t i = 0; i < clauses.size(); ++i) {
        SpanQueryPtr clause(clauses[i]);
        if (i == 0) { // check field
            field = clause->getField();
        } else if (clause->getField() != field) {
            boost::throw_exception(IllegalArgumentException(L"Clauses must have same field."));
        }
        this->clauses.add(clause);
    }
    this->collectPayloads = collectPayloads;
    this->slop = slop;
    this->inOrder = inOrder;
}

SpanNearQuery::~SpanNearQuery() {
}

Collection<SpanQueryPtr> SpanNearQuery::getClauses() {
    return clauses;
}

int32_t SpanNearQuery::getSlop() {
    return slop;
}

bool SpanNearQuery::isInOrder() {
    return inOrder;
}

String SpanNearQuery::getField() {
    return field;
}

void SpanNearQuery::extractTerms(SetTerm terms) {
    for (Collection<SpanQueryPtr>::iterator clause = clauses.begin(); clause != clauses.end(); ++clause) {
        (*clause)->extractTerms(terms);
    }
}

String SpanNearQuery::toString(const String& field) {
    StringStream buffer;
    buffer << L"spanNear([";
    for (Collection<SpanQueryPtr>::iterator clause = clauses.begin(); clause != clauses.end(); ++clause) {
        if (clause != clauses.begin()) {
            buffer << L", ";
        }
        buffer << (*clause)->toString(field);
    }
    buffer << L"], " << slop << L", " << inOrder << L")" << boostString();
    return buffer.str();
}

SpansPtr SpanNearQuery::getSpans(const IndexReaderPtr& reader) {
    if (clauses.empty()) { // optimize 0-clause case
        return newLucene<SpanOrQuery>(getClauses())->getSpans(reader);
    }

    if (clauses.size() == 1) { // optimize 1-clause case
        return clauses[0]->getSpans(reader);
    }

    return inOrder
           ? boost::static_pointer_cast<Spans>(newLucene<NearSpansOrdered>(shared_from_this(), reader, collectPayloads))
           : boost::static_pointer_cast<Spans>(newLucene<NearSpansUnordered>(shared_from_this(), reader));
}

QueryPtr SpanNearQuery::rewrite(const IndexReaderPtr& reader) {
    SpanNearQueryPtr clone;
    for (int32_t i = 0; i < clauses.size(); ++i) {
        SpanQueryPtr clause(clauses[i]);
        SpanQueryPtr query(boost::dynamic_pointer_cast<SpanQuery>(clause->rewrite(reader)));
        if (query != clause) { // clause rewrote: must clone
            if (!clone) {
                clone = boost::dynamic_pointer_cast<SpanNearQuery>(this->clone());
            }
            clone->clauses[i] = query;
        }
    }
    if (clone) {
        return clone;    // some clauses rewrote
    } else {
        return shared_from_this();    // no clauses rewrote
    }
}

LuceneObjectPtr SpanNearQuery::clone(const LuceneObjectPtr& other) {
    int32_t sz = clauses.size();
    Collection<SpanQueryPtr> newClauses(Collection<SpanQueryPtr>::newInstance(sz));

    for (int32_t i = 0; i < sz; ++i) {
        newClauses[i] = boost::dynamic_pointer_cast<SpanQuery>(clauses[i]->clone());
    }

    SpanNearQueryPtr spanNearQuery(newLucene<SpanNearQuery>(newClauses, slop, inOrder));
    spanNearQuery->setBoost(getBoost());
    return spanNearQuery;
}

bool SpanNearQuery::equals(const LuceneObjectPtr& other) {
    if (LuceneObject::equals(other)) {
        return true;
    }

    SpanNearQueryPtr otherQuery(boost::dynamic_pointer_cast<SpanNearQuery>(other));
    if (!otherQuery) {
        return false;
    }

    if (inOrder != otherQuery->inOrder) {
        return false;
    }
    if (slop != otherQuery->slop) {
        return false;
    }
    if (!clauses.equals(otherQuery->clauses, luceneEquals<SpanQueryPtr>())) {
        return false;
    }

    return (getBoost() == otherQuery->getBoost());
}

int32_t SpanNearQuery::hashCode() {
    int32_t result = MiscUtils::hashCode(clauses.begin(), clauses.end(), MiscUtils::hashLucene<SpanQueryPtr>);
    // Mix bits before folding in things like boost, since it could cancel the last element of clauses.
    // This particular mix also serves to differentiate SpanNearQuery hashcodes from others.
    result ^= (result << 14) | MiscUtils::unsignedShift(result, 19); // reversible
    result += MiscUtils::doubleToRawIntBits(getBoost());
    result += slop;
    result ^= (inOrder ? 0x99afd3bd : 0);
    return result;
}

}
