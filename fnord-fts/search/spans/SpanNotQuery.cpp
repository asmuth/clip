/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "SpanNotQuery.h"
#include "_SpanNotQuery.h"
#include "MiscUtils.h"

namespace Lucene {

SpanNotQuery::SpanNotQuery(const SpanQueryPtr& include, const SpanQueryPtr& exclude) {
    this->include = include;
    this->exclude = exclude;

    if (include->getField() != exclude->getField()) {
        boost::throw_exception(IllegalArgumentException(L"Clauses must have same field."));
    }
}

SpanNotQuery::~SpanNotQuery() {
}

SpanQueryPtr SpanNotQuery::getInclude() {
    return include;
}

SpanQueryPtr SpanNotQuery::getExclude() {
    return exclude;
}

String SpanNotQuery::getField() {
    return include->getField();
}

void SpanNotQuery::extractTerms(SetTerm terms) {
    include->extractTerms(terms);
}

String SpanNotQuery::toString(const String& field) {
    StringStream buffer;
    buffer << L"spanNot(" << include->toString(field) << L", " << exclude->toString(field) << L")";
    buffer << boostString();
    return buffer.str();
}

LuceneObjectPtr SpanNotQuery::clone(const LuceneObjectPtr& other) {
    SpanNotQueryPtr spanNotQuery(newLucene<SpanNotQuery>(boost::dynamic_pointer_cast<SpanQuery>(include->clone()),
                                 boost::dynamic_pointer_cast<SpanQuery>(exclude->clone())));
    spanNotQuery->setBoost(getBoost());
    return spanNotQuery;
}

SpansPtr SpanNotQuery::getSpans(const IndexReaderPtr& reader) {
    return newLucene<NotSpans>(shared_from_this(), include->getSpans(reader), exclude->getSpans(reader));
}

QueryPtr SpanNotQuery::rewrite(const IndexReaderPtr& reader) {
    SpanNotQueryPtr clone;
    SpanQueryPtr rewrittenInclude(boost::dynamic_pointer_cast<SpanQuery>(include->rewrite(reader)));
    if (rewrittenInclude != include) {
        clone = boost::dynamic_pointer_cast<SpanNotQuery>(this->clone());
        clone->include = rewrittenInclude;
    }

    SpanQueryPtr rewrittenExclude(boost::dynamic_pointer_cast<SpanQuery>(exclude->rewrite(reader)));
    if (rewrittenExclude != exclude) {
        if (!clone) {
            clone = boost::dynamic_pointer_cast<SpanNotQuery>(this->clone());
        }
        clone->exclude = rewrittenExclude;
    }

    if (clone) {
        return clone;    // some clauses rewrote
    } else {
        return shared_from_this();    // no clauses rewrote
    }
}

bool SpanNotQuery::equals(const LuceneObjectPtr& other) {
    if (LuceneObject::equals(other)) {
        return true;
    }

    SpanNotQueryPtr otherQuery(boost::dynamic_pointer_cast<SpanNotQuery>(other));
    if (!otherQuery) {
        return false;
    }

    return (include->equals(otherQuery->include) && exclude->equals(otherQuery->exclude) && getBoost() == otherQuery->getBoost());
}

int32_t SpanNotQuery::hashCode() {
    int32_t result = include->hashCode();
    result = (result << 1) | MiscUtils::unsignedShift(result, 31); // rotate left
    result ^= exclude->hashCode();
    result = (result << 1) | MiscUtils::unsignedShift(result, 31); // rotate left
    result ^= MiscUtils::doubleToRawIntBits(getBoost());
    return result;
}

NotSpans::NotSpans(const SpanNotQueryPtr& query, const SpansPtr& includeSpans, const SpansPtr& excludeSpans) {
    this->query = query;
    this->includeSpans = includeSpans;
    this->moreInclude = true;
    this->excludeSpans = excludeSpans;
    this->moreExclude = excludeSpans->next();
}

NotSpans::~NotSpans() {
}

bool NotSpans::next() {
    if (moreInclude) { // move to next include
        moreInclude = includeSpans->next();
    }

    while (moreInclude && moreExclude) {
        if (includeSpans->doc() > excludeSpans->doc()) { // skip exclude
            moreExclude = excludeSpans->skipTo(includeSpans->doc());
        }

        // while exclude is before
        while (moreExclude && includeSpans->doc() == excludeSpans->doc() && excludeSpans->end() <= includeSpans->start()) {
            moreExclude = excludeSpans->next();    // increment exclude
        }

        // if no intersection
        if (!moreExclude || includeSpans->doc() != excludeSpans->doc() || includeSpans->end() <= excludeSpans->start()) {
            break;    // we found a match
        }

        moreInclude = includeSpans->next(); // intersected: keep scanning
    }
    return moreInclude;
}

bool NotSpans::skipTo(int32_t target) {
    if (moreInclude) { // skip include
        moreInclude = includeSpans->skipTo(target);
    }

    if (!moreInclude) {
        return false;
    }

    // skip exclude
    if (moreExclude && includeSpans->doc() > excludeSpans->doc()) {
        moreExclude = excludeSpans->skipTo(includeSpans->doc());
    }

    // while exclude is before
    while (moreExclude && includeSpans->doc() == excludeSpans->doc() && excludeSpans->end() <= includeSpans->start()) {
        moreExclude = excludeSpans->next();    // increment exclude
    }

    // if no intersection
    if (!moreExclude || includeSpans->doc() != excludeSpans->doc() || includeSpans->end() <= excludeSpans->start()) {
        return true;    // we found a match
    }

    return next(); // scan to next match
}

int32_t NotSpans::doc() {
    return includeSpans->doc();
}

int32_t NotSpans::start() {
    return includeSpans->start();
}

int32_t NotSpans::end() {
    return includeSpans->end();
}

Collection<ByteArray> NotSpans::getPayload() {
    Collection<ByteArray> result;
    if (includeSpans->isPayloadAvailable()) {
        Collection<ByteArray> payload(includeSpans->getPayload());
        result = Collection<ByteArray>::newInstance(payload.begin(), payload.end());
    }
    return result;
}

bool NotSpans::isPayloadAvailable() {
    return includeSpans->isPayloadAvailable();
}

String NotSpans::toString() {
    return L"spans(" + query->toString() + L")";
}

}
