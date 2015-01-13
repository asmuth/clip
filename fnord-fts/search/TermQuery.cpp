/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "fnord-fts/LuceneInc.h"
#include "fnord-fts/search/TermQuery.h"
#include "fnord-fts/search/_TermQuery.h"
#include "fnord-fts/search/TermScorer.h"
#include "fnord-fts/index/IndexReader.h"
#include "fnord-fts/search/ComplexExplanation.h"
#include "fnord-fts/search/Term.h"
#include "fnord-fts/search/TermDocs.h"
#include "fnord-fts/search/Similarity.h"
#include "fnord-fts/util/MiscUtils.h"
#include "fnord-fts/util/StringUtils.h"

namespace Lucene {

TermQuery::TermQuery(const TermPtr& term) {
    this->term = term;
}

TermQuery::~TermQuery() {
}

TermPtr TermQuery::getTerm() {
    return term;
}

WeightPtr TermQuery::createWeight(const SearcherPtr& searcher) {
    return newLucene<TermWeight>(shared_from_this(), searcher);
}

void TermQuery::extractTerms(SetTerm terms) {
    terms.add(getTerm());
}

String TermQuery::toString(const String& field) {
    StringStream buffer;
    if (term->field() != field) {
        buffer << term->field() << L":";
    }
    buffer << term->text() << boostString();
    return buffer.str();
}

bool TermQuery::equals(const LuceneObjectPtr& other) {
    if (LuceneObject::equals(other)) {
        return true;
    }

    TermQueryPtr otherTermQuery(std::dynamic_pointer_cast<TermQuery>(other));
    if (!otherTermQuery) {
        return false;
    }

    return (getBoost() == otherTermQuery->getBoost() && term->equals(otherTermQuery->term));
}

int32_t TermQuery::hashCode() {
    return MiscUtils::doubleToIntBits(getBoost()) ^ term->hashCode();
}

LuceneObjectPtr TermQuery::clone(const LuceneObjectPtr& other) {
    LuceneObjectPtr clone = other ? other : newLucene<TermQuery>(term);
    TermQueryPtr cloneQuery(std::dynamic_pointer_cast<TermQuery>(Query::clone(clone)));
    cloneQuery->term = term;
    return cloneQuery;
}

TermWeight::TermWeight(const TermQueryPtr& query, const SearcherPtr& searcher) {
    this->query = query;
    this->similarity = query->getSimilarity(searcher);
    this->value = 0.0;
    this->idf = 0.0;
    this->queryNorm = 0.0;
    this->queryWeight = 0.0;

    this->idfExp = similarity->idfExplain(query->term, searcher);
    idf = idfExp->getIdf();
}

TermWeight::~TermWeight() {
}

String TermWeight::toString() {
    return L"weight(" + query->toString() + L")";
}

QueryPtr TermWeight::getQuery() {
    return query;
}

double TermWeight::getValue() {
    return value;
}

double TermWeight::sumOfSquaredWeights() {
    queryWeight = idf * getQuery()->getBoost(); // compute query weight
    return queryWeight * queryWeight; // square it
}

void TermWeight::normalize(double norm) {
    queryNorm = norm;
    queryWeight *= queryNorm; // normalize query weight
    value = queryWeight * idf; // idf for document
}

ScorerPtr TermWeight::scorer(const IndexReaderPtr& reader, bool scoreDocsInOrder, bool topScorer) {
    TermDocsPtr termDocs(reader->termDocs(query->term));
    return termDocs ? newLucene<TermScorer>(shared_from_this(), termDocs, similarity, reader->norms(query->term->field())) : ScorerPtr();
}

ExplanationPtr TermWeight::explain(const IndexReaderPtr& reader, int32_t doc) {
    ComplexExplanationPtr result(newLucene<ComplexExplanation>());
    result->setDescription(L"weight(" + query->toString() + L" in " + StringUtils::toString(doc) + L"), product of:");

    ExplanationPtr expl(newLucene<Explanation>(idf, idfExp->explain()));

    // explain query weight
    ExplanationPtr queryExpl(newLucene<Explanation>());
    queryExpl->setDescription(L"queryWeight(" + query->toString() + L"), product of:");

    ExplanationPtr boostExpl(newLucene<Explanation>(query->getBoost(), L"boost"));
    if (query->getBoost() != 1.0) {
        queryExpl->addDetail(boostExpl);
    }
    queryExpl->addDetail(expl);

    ExplanationPtr queryNormExpl(newLucene<Explanation>(queryNorm, L"queryNorm"));
    queryExpl->addDetail(queryNormExpl);

    queryExpl->setValue(boostExpl->getValue() * expl->getValue() * queryNormExpl->getValue());
    result->addDetail(queryExpl);

    // explain field weight
    String field(query->term->field());
    ComplexExplanationPtr fieldExpl(newLucene<ComplexExplanation>());
    fieldExpl->setDescription(L"fieldWeight(" +    query->term->toString() + L" in " + StringUtils::toString(doc) + L"), product of:");

    ExplanationPtr tfExplanation(newLucene<Explanation>());
    int32_t tf = 0;
    TermDocsPtr termDocs(reader->termDocs(query->term));
    if (termDocs) {
        LuceneException finally;
        try {
            if (termDocs->skipTo(doc) && termDocs->doc() == doc) {
                tf = termDocs->freq();
            }
        } catch (LuceneException& e) {
            finally = e;
        }
        termDocs->close();
        finally.throwException();
        tfExplanation->setValue(similarity->tf(tf));
        tfExplanation->setDescription(L"tf(termFreq(" + query->term->toString() + L")=" + StringUtils::toString(tf) + L")");
    } else {
        tfExplanation->setValue(0.0);
        tfExplanation->setDescription(L"no matching term");
    }

    fieldExpl->addDetail(tfExplanation);
    fieldExpl->addDetail(expl);

    ExplanationPtr fieldNormExpl(newLucene<Explanation>());
    ByteArray fieldNorms(reader->norms(field));
    double fieldNorm = fieldNorms ? Similarity::decodeNorm(fieldNorms[doc]) : 1.0;
    fieldNormExpl->setValue(fieldNorm);
    fieldNormExpl->setDescription(L"fieldNorm(field=" + field + L", doc=" + StringUtils::toString(doc) + L")");
    fieldExpl->addDetail(fieldNormExpl);

    fieldExpl->setMatch(tfExplanation->isMatch());
    fieldExpl->setValue(tfExplanation->getValue() * expl->getValue() * fieldNormExpl->getValue());

    result->addDetail(fieldExpl);
    result->setMatch(fieldExpl->getMatch());

    // combine them
    result->setValue(queryExpl->getValue() * fieldExpl->getValue());

    if (queryExpl->getValue() == 1.0) {
        return fieldExpl;
    }

    return result;
}

}
