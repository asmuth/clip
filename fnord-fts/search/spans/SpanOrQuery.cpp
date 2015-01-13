/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "SpanOrQuery.h"
#include "_SpanOrQuery.h"
#include "MiscUtils.h"

namespace Lucene {

SpanOrQuery::SpanOrQuery(Collection<SpanQueryPtr> clauses) {
    // copy clauses array into an ArrayList
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
}

SpanOrQuery::~SpanOrQuery() {
}

Collection<SpanQueryPtr> SpanOrQuery::getClauses() {
    return clauses;
}

String SpanOrQuery::getField() {
    return field;
}

void SpanOrQuery::extractTerms(SetTerm terms) {
    for (Collection<SpanQueryPtr>::iterator clause = clauses.begin(); clause != clauses.end(); ++clause) {
        (*clause)->extractTerms(terms);
    }
}

LuceneObjectPtr SpanOrQuery::clone(const LuceneObjectPtr& other) {
    int32_t sz = clauses.size();
    Collection<SpanQueryPtr> newClauses(Collection<SpanQueryPtr>::newInstance(sz));

    for (int32_t i = 0; i < sz; ++i) {
        newClauses[i] = boost::dynamic_pointer_cast<SpanQuery>(clauses[i]->clone());
    }

    SpanOrQueryPtr spanOrQuery(newLucene<SpanOrQuery>(newClauses));
    spanOrQuery->setBoost(getBoost());
    return spanOrQuery;
}

QueryPtr SpanOrQuery::rewrite(const IndexReaderPtr& reader) {
    SpanOrQueryPtr clone;
    for (int32_t i = 0; i < clauses.size(); ++i) {
        SpanQueryPtr clause(clauses[i]);
        SpanQueryPtr query(boost::dynamic_pointer_cast<SpanQuery>(clause->rewrite(reader)));
        if (query != clause) { // clause rewrote: must clone
            if (!clone) {
                clone = boost::dynamic_pointer_cast<SpanOrQuery>(this->clone());
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

String SpanOrQuery::toString(const String& field) {
    StringStream buffer;
    buffer << L"SpanOr([";
    for (Collection<SpanQueryPtr>::iterator clause = clauses.begin(); clause != clauses.end(); ++clause) {
        if (clause != clauses.begin()) {
            buffer << L", ";
        }
        buffer << (*clause)->toString(field);
    }
    buffer << L"])" << boostString();
    return buffer.str();
}

bool SpanOrQuery::equals(const LuceneObjectPtr& other) {
    if (LuceneObject::equals(other)) {
        return true;
    }

    SpanOrQueryPtr otherQuery(boost::dynamic_pointer_cast<SpanOrQuery>(other));
    if (!otherQuery) {
        return false;
    }

    if (!clauses.equals(otherQuery->clauses, luceneEquals<SpanQueryPtr>())) {
        return false;
    }
    if (!clauses.empty() && field != otherQuery->field) {
        return false;
    }

    return (getBoost() == otherQuery->getBoost());
}

int32_t SpanOrQuery::hashCode() {
    int32_t result = MiscUtils::hashCode(clauses.begin(), clauses.end(), MiscUtils::hashLucene<SpanQueryPtr>);
    result ^= (result << 10) | MiscUtils::unsignedShift(result, 23);
    result ^= MiscUtils::doubleToRawIntBits(getBoost());
    return result;
}

SpansPtr SpanOrQuery::getSpans(const IndexReaderPtr& reader) {
    if (clauses.size() == 1) { // optimize 1-clause case
        return clauses[0]->getSpans(reader);
    }
    return newLucene<OrSpans>(shared_from_this(), reader);
}

SpanQueue::SpanQueue(int32_t size) : PriorityQueue<SpansPtr>(size) {
}

SpanQueue::~SpanQueue() {
}

bool SpanQueue::lessThan(const SpansPtr& first, const SpansPtr& second) {
    if (first->doc() == second->doc()) {
        if (first->start() == second->start()) {
            return (first->end() < second->end());
        } else {
            return (first->start() < second->start());
        }
    } else {
        return (first->doc() < second->doc());
    }
}

OrSpans::OrSpans(const SpanOrQueryPtr& query, const IndexReaderPtr& reader) {
    this->query = query;
    this->reader = reader;
}

OrSpans::~OrSpans() {
}

bool OrSpans::initSpanQueue(int32_t target) {
    queue = newLucene<SpanQueue>(query->clauses.size());
    for (Collection<SpanQueryPtr>::iterator clause = query->clauses.begin(); clause != query->clauses.end(); ++clause) {
        SpansPtr spans((*clause)->getSpans(reader));
        if ((target == -1 && spans->next()) || (target != -1 && spans->skipTo(target))) {
            queue->add(spans);
        }
    }
    return !queue->empty();
}

bool OrSpans::next() {
    if (!queue) {
        return initSpanQueue(-1);
    }

    if (queue->empty()) { // all done
        return false;
    }

    if (top()->next()) { // move to next
        queue->updateTop();
        return true;
    }

    queue->pop(); // exhausted a clause
    return !queue->empty();
}

SpansPtr OrSpans::top() {
    return queue->top();
}

bool OrSpans::skipTo(int32_t target) {
    if (!queue) {
        return initSpanQueue(target);
    }

    bool skipCalled = false;
    while (!queue->empty() && top()->doc() < target) {
        if (top()->skipTo(target)) {
            queue->updateTop();
        } else {
            queue->pop();
        }
        skipCalled = true;
    }

    if (skipCalled) {
        return !queue->empty();
    }
    return next();
}

int32_t OrSpans::doc() {
    return top()->doc();
}

int32_t OrSpans::start() {
    return top()->start();
}

int32_t OrSpans::end() {
    return top()->end();
}

Collection<ByteArray> OrSpans::getPayload() {
    Collection<ByteArray> result;
    SpansPtr theTop(top());
    if (theTop && theTop->isPayloadAvailable()) {
        Collection<ByteArray> payload(theTop->getPayload());
        result = Collection<ByteArray>::newInstance(payload.begin(), payload.end());
    }
    return result;
}

bool OrSpans::isPayloadAvailable() {
    SpansPtr theTop(top());
    return (theTop && theTop->isPayloadAvailable());
}

String OrSpans::toString() {
    StringStream buffer;
    buffer << L"spans(" << query->toString() << L")@";
    if (!queue) {
        buffer << L"START";
    } else {
        if (!queue->empty()) {
            buffer << doc() << L":" << start() << L"-" << end();
        } else {
            buffer << L"END";
        }
    }
    return buffer.str();
}

}
