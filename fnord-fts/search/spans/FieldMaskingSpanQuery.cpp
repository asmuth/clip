/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "FieldMaskingSpanQuery.h"
#include "MiscUtils.h"
#include "StringUtils.h"

namespace Lucene {

FieldMaskingSpanQuery::FieldMaskingSpanQuery(const SpanQueryPtr& maskedQuery, const String& maskedField) {
    this->maskedQuery = maskedQuery;
    this->field = maskedField;
}

FieldMaskingSpanQuery::~FieldMaskingSpanQuery() {
}

String FieldMaskingSpanQuery::getField() {
    return field;
}

SpanQueryPtr FieldMaskingSpanQuery::getMaskedQuery() {
    return maskedQuery;
}

// :NOTE: getBoost and setBoost are not proxied to the maskedQuery
// ...this is done to be more consistent with things like SpanFirstQuery

SpansPtr FieldMaskingSpanQuery::getSpans(const IndexReaderPtr& reader) {
    return maskedQuery->getSpans(reader);
}

void FieldMaskingSpanQuery::extractTerms(SetTerm terms) {
    maskedQuery->extractTerms(terms);
}

WeightPtr FieldMaskingSpanQuery::createWeight(const SearcherPtr& searcher) {
    return maskedQuery->createWeight(searcher);
}

SimilarityPtr FieldMaskingSpanQuery::getSimilarity(const SearcherPtr& searcher) {
    return maskedQuery->getSimilarity(searcher);
}

QueryPtr FieldMaskingSpanQuery::rewrite(const IndexReaderPtr& reader) {
    FieldMaskingSpanQueryPtr clone;

    SpanQueryPtr rewritten(boost::dynamic_pointer_cast<SpanQuery>(maskedQuery->rewrite(reader)));
    if (rewritten != maskedQuery) {
        clone = boost::dynamic_pointer_cast<FieldMaskingSpanQuery>(this->clone());
        clone->maskedQuery = rewritten;
    }

    if (clone) {
        return clone;
    } else {
        return shared_from_this();
    }
}

String FieldMaskingSpanQuery::toString(const String& field) {
    StringStream buffer;
    buffer << L"mask(" << maskedQuery->toString(field) << L")";
    buffer << boostString() << L" as " << this->field;
    return buffer.str();
}

bool FieldMaskingSpanQuery::equals(const LuceneObjectPtr& other) {
    if (LuceneObject::equals(other)) {
        return true;
    }

    FieldMaskingSpanQueryPtr otherQuery(boost::dynamic_pointer_cast<FieldMaskingSpanQuery>(other));
    if (!otherQuery) {
        return false;
    }

    return (getField() == otherQuery->getField() && getBoost() == otherQuery->getBoost() &&
            getMaskedQuery()->equals(otherQuery->getMaskedQuery()));
}

int32_t FieldMaskingSpanQuery::hashCode() {
    return getMaskedQuery()->hashCode() ^ StringUtils::hashCode(getField()) ^ MiscUtils::doubleToRawIntBits(getBoost());
}

LuceneObjectPtr FieldMaskingSpanQuery::clone(const LuceneObjectPtr& other) {
    LuceneObjectPtr clone = SpanQuery::clone(other ? other : newLucene<FieldMaskingSpanQuery>(maskedQuery, field));
    FieldMaskingSpanQueryPtr cloneQuery(boost::dynamic_pointer_cast<FieldMaskingSpanQuery>(clone));
    cloneQuery->maskedQuery = maskedQuery;
    cloneQuery->field = field;
    return cloneQuery;
}

}
