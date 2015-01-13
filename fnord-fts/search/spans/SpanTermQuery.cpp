/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "SpanTermQuery.h"
#include "Term.h"
#include "TermSpans.h"
#include "IndexReader.h"
#include "MiscUtils.h"

namespace Lucene {

SpanTermQuery::SpanTermQuery(const TermPtr& term) {
    this->term = term;
}

SpanTermQuery::~SpanTermQuery() {
}

TermPtr SpanTermQuery::getTerm() {
    return term;
}

String SpanTermQuery::getField() {
    return term->field();
}

void SpanTermQuery::extractTerms(SetTerm terms) {
    terms.add(term);
}

String SpanTermQuery::toString(const String& field) {
    StringStream buffer;
    if (term->field() == field) {
        buffer << term->text();
    } else {
        buffer << term->toString();
    }
    buffer << boostString();
    return buffer.str();
}

int32_t SpanTermQuery::hashCode() {
    int32_t prime = 31;
    int32_t result = SpanQuery::hashCode();
    result = prime * result + (term ? term->hashCode() : 0);
    return result;
}

bool SpanTermQuery::equals(const LuceneObjectPtr& other) {
    if (LuceneObject::equals(other)) {
        return true;
    }
    if (!SpanQuery::equals(other)) {
        return false;
    }
    if (!MiscUtils::equalTypes(shared_from_this(), other)) {
        return false;
    }
    SpanTermQueryPtr otherQuery(boost::dynamic_pointer_cast<SpanTermQuery>(other));
    if (!otherQuery) {
        return false;
    }
    if (!term) {
        if (otherQuery->term) {
            return false;
        }
    } else if (!term->equals(otherQuery->term)) {
        return false;
    }
    return true;
}

LuceneObjectPtr SpanTermQuery::clone(const LuceneObjectPtr& other) {
    LuceneObjectPtr clone = SpanQuery::clone(other ? other : newLucene<SpanTermQuery>(term));
    SpanTermQueryPtr spanFirstQuery(boost::dynamic_pointer_cast<SpanTermQuery>(clone));
    spanFirstQuery->term = term;
    return spanFirstQuery;
}

SpansPtr SpanTermQuery::getSpans(const IndexReaderPtr& reader) {
    return newLucene<TermSpans>(reader->termPositions(term), term);
}

}
