/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "SpanFirstQuery.h"
#include "_SpanFirstQuery.h"
#include "SpanQuery.h"
#include "MiscUtils.h"

namespace Lucene {

SpanFirstQuery::SpanFirstQuery(const SpanQueryPtr& match, int32_t end) {
    this->match = match;
    this->end = end;
}

SpanFirstQuery::~SpanFirstQuery() {
}

SpanQueryPtr SpanFirstQuery::getMatch() {
    return match;
}

int32_t SpanFirstQuery::getEnd() {
    return end;
}

String SpanFirstQuery::getField() {
    return match->getField();
}

String SpanFirstQuery::toString(const String& field) {
    StringStream buffer;
    buffer << L"spanFirst(" << match->toString(field) << L", " << end << L")" << boostString();
    return buffer.str();
}

LuceneObjectPtr SpanFirstQuery::clone(const LuceneObjectPtr& other) {
    LuceneObjectPtr clone = SpanQuery::clone(other ? other : newLucene<SpanFirstQuery>(boost::dynamic_pointer_cast<SpanQuery>(match->clone()), end));
    SpanFirstQueryPtr spanFirstQuery(boost::dynamic_pointer_cast<SpanFirstQuery>(clone));
    spanFirstQuery->match = match;
    spanFirstQuery->end = end;
    spanFirstQuery->setBoost(getBoost());
    return spanFirstQuery;
}

void SpanFirstQuery::extractTerms(SetTerm terms) {
    match->extractTerms(terms);
}

SpansPtr SpanFirstQuery::getSpans(const IndexReaderPtr& reader) {
    return newLucene<FirstSpans>(shared_from_this(), match->getSpans(reader));
}

QueryPtr SpanFirstQuery::rewrite(const IndexReaderPtr& reader) {
    SpanFirstQueryPtr clone;
    SpanQueryPtr rewritten(boost::dynamic_pointer_cast<SpanQuery>(match->rewrite(reader)));
    if (rewritten != match) {
        clone = boost::dynamic_pointer_cast<SpanFirstQuery>(this->clone());
        clone->match = rewritten;
    }

    if (clone) {
        return clone;    // some clauses rewrote
    } else {
        return shared_from_this();    // no clauses rewrote
    }
}

bool SpanFirstQuery::equals(const LuceneObjectPtr& other) {
    if (LuceneObject::equals(other)) {
        return true;
    }

    SpanFirstQueryPtr otherQuery(boost::dynamic_pointer_cast<SpanFirstQuery>(other));
    if (!otherQuery) {
        return false;
    }

    return (end == otherQuery->end && match->equals(otherQuery->match) && getBoost() == otherQuery->getBoost());
}

int32_t SpanFirstQuery::hashCode() {
    int32_t result = match->hashCode();
    result ^= (result << 8) | MiscUtils::unsignedShift(result, 25); // reversible
    result ^= MiscUtils::doubleToRawIntBits(getBoost()) ^ end;
    return result;
}

FirstSpans::FirstSpans(const SpanFirstQueryPtr& query, const SpansPtr& spans) {
    this->query = query;
    this->spans = spans;
}

FirstSpans::~FirstSpans() {
}

bool FirstSpans::next() {
    while (spans->next()) { // scan to next match
        if (end() <= query->end) {
            return true;
        }
    }
    return false;
}

bool FirstSpans::skipTo(int32_t target) {
    if (!spans->skipTo(target)) {
        return false;
    }
    return (spans->end() <= query->end || next());
}

int32_t FirstSpans::doc() {
    return spans->doc();
}

int32_t FirstSpans::start() {
    return spans->start();
}

int32_t FirstSpans::end() {
    return spans->end();
}

Collection<ByteArray> FirstSpans::getPayload() {
    Collection<ByteArray> result;
    if (spans->isPayloadAvailable()) {
        Collection<ByteArray> payload(spans->getPayload());
        result = Collection<ByteArray>::newInstance(payload.begin(), payload.end());
    }
    return result;
}

bool FirstSpans::isPayloadAvailable() {
    return spans->isPayloadAvailable();
}

}
