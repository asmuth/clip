/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "fnord-fts/LuceneInc.h"
#include "fnord-fts/search/PhraseQuery.h"
#include "fnord-fts/search/_PhraseQuery.h"
#include "fnord-fts/search/Similarity.h"
#include "fnord-fts/index/Term.h"
#include "fnord-fts/search/TermPositions.h"
#include "fnord-fts/search/TermQuery.h"
#include "fnord-fts/index/IndexReader.h"
#include "fnord-fts/search/ExactPhraseScorer.h"
#include "fnord-fts/search/SloppyPhraseScorer.h"
#include "fnord-fts/search/Explanation.h"
#include "fnord-fts/util/MiscUtils.h"
#include "fnord-fts/util/StringUtils.h"

namespace Lucene {

PhraseQuery::PhraseQuery() {
    terms = Collection<TermPtr>::newInstance();
    positions = Collection<int32_t>::newInstance();
    maxPosition = 0;
    slop = 0;
}

PhraseQuery::~PhraseQuery() {
}

void PhraseQuery::setSlop(int32_t slop) {
    this->slop = slop;
}

int32_t PhraseQuery::getSlop() {
    return slop;
}

void PhraseQuery::add(const TermPtr& term) {
    int32_t position = 0;
    if (!positions.empty()) {
        position = positions[positions.size() - 1] + 1;
    }
    add(term, position);
}

void PhraseQuery::add(const TermPtr& term, int32_t position) {
    if (terms.empty()) {
        field = term->field();
    } else if (term->field() != field) {
        boost::throw_exception(IllegalArgumentException(L"All phrase terms must be in the same field: " + term->toString()));
    }

    terms.add(term);
    positions.add(position);
    if (position > maxPosition) {
        maxPosition = position;
    }
}

Collection<TermPtr> PhraseQuery::getTerms() {
    return terms;
}

Collection<int32_t> PhraseQuery::getPositions() {
    return positions;
}

WeightPtr PhraseQuery::createWeight(const SearcherPtr& searcher) {
    if (terms.size() == 1) { // optimize one-term case
        QueryPtr termQuery(newLucene<TermQuery>(terms[0]));
        termQuery->setBoost(getBoost());
        return termQuery->createWeight(searcher);
    }
    return newLucene<PhraseWeight>(shared_from_this(), searcher);
}

void PhraseQuery::extractTerms(SetTerm terms) {
    terms.addAll(this->terms.begin(), this->terms.end());
}

String PhraseQuery::toString(const String& field) {
    StringStream buffer;
    if (this->field != field) {
        buffer << this->field << L":";
    }
    buffer << L"\"";
    Collection<String> pieces(Collection<String>::newInstance(maxPosition + 1));
    for (int32_t i = 0; i < terms.size(); ++i) {
        int32_t pos = positions[i];
        String s(pieces[pos]);
        if (!s.empty()) {
            s += L"|";
        }
        s += terms[i]->text();
        pieces[pos] = s;
    }
    for (int32_t i = 0; i < pieces.size(); ++i) {
        if (i > 0) {
            buffer << L" ";
        }
        String s(pieces[i]);
        buffer << (s.empty() ? L"?" : s);
    }
    buffer << L"\"";

    if (slop != 0) {
        buffer << L"~" << slop;
    }

    buffer << boostString();

    return buffer.str();
}

bool PhraseQuery::equals(const LuceneObjectPtr& other) {
    if (LuceneObject::equals(other)) {
        return true;
    }

    PhraseQueryPtr otherPhraseQuery(std::dynamic_pointer_cast<PhraseQuery>(other));
    if (!otherPhraseQuery) {
        return false;
    }

    return (getBoost() == otherPhraseQuery->getBoost() && slop == otherPhraseQuery->slop &&
            terms.equals(otherPhraseQuery->terms, luceneEquals<TermPtr>()) && positions.equals(otherPhraseQuery->positions));
}

int32_t PhraseQuery::hashCode() {
    return MiscUtils::doubleToIntBits(getBoost()) ^ slop ^
           MiscUtils::hashCode(terms.begin(), terms.end(), MiscUtils::hashLucene<TermPtr>) ^
           MiscUtils::hashCode(positions.begin(), positions.end(), MiscUtils::hashNumeric<int32_t>);
}

LuceneObjectPtr PhraseQuery::clone(const LuceneObjectPtr& other) {
    LuceneObjectPtr clone = other ? other : newLucene<PhraseQuery>();
    PhraseQueryPtr cloneQuery(std::dynamic_pointer_cast<PhraseQuery>(Query::clone(clone)));
    cloneQuery->field = field;
    cloneQuery->terms = terms;
    cloneQuery->positions = positions;
    cloneQuery->maxPosition = maxPosition;
    cloneQuery->slop = slop;
    return cloneQuery;
}

PhraseWeight::PhraseWeight(const PhraseQueryPtr& query, const SearcherPtr& searcher) {
    this->query = query;
    this->similarity = query->getSimilarity(searcher);
    this->value = 0.0;
    this->idf = 0.0;
    this->queryNorm = 0.0;
    this->queryWeight = 0.0;

    this->idfExp = similarity->idfExplain(query->terms, searcher);
    idf = idfExp->getIdf();
}

PhraseWeight::~PhraseWeight() {
}

String PhraseWeight::toString() {
    return L"weight(" + query->toString() + L")";
}

QueryPtr PhraseWeight::getQuery() {
    return query;
}

double PhraseWeight::getValue() {
    return value;
}

double PhraseWeight::sumOfSquaredWeights() {
    queryWeight = idf * getQuery()->getBoost(); // compute query weight
    return queryWeight * queryWeight; // square it
}

void PhraseWeight::normalize(double norm) {
    queryNorm = norm;
    queryWeight *= queryNorm; // normalize query weight
    value = queryWeight * idf; // idf for document
}

ScorerPtr PhraseWeight::scorer(const IndexReaderPtr& reader, bool scoreDocsInOrder, bool topScorer) {
    if (query->terms.empty()) { // optimize zero-term case
        return ScorerPtr();
    }

    Collection<TermPositionsPtr> tps(Collection<TermPositionsPtr>::newInstance(query->terms.size()));
    for (int32_t i = 0; i < tps.size(); ++i) {
        TermPositionsPtr p(reader->termPositions(query->terms[i]));
        if (!p) {
            return ScorerPtr();
        }
        tps[i] = p;
    }

    if (query->slop == 0) { // optimize exact case
        return newLucene<ExactPhraseScorer>(shared_from_this(), tps, query->getPositions(), similarity, reader->norms(query->field));
    } else {
        return newLucene<SloppyPhraseScorer>(shared_from_this(), tps, query->getPositions(), similarity, query->slop, reader->norms(query->field));
    }
}

ExplanationPtr PhraseWeight::explain(const IndexReaderPtr& reader, int32_t doc) {
    ExplanationPtr result(newLucene<Explanation>());
    result->setDescription(L"weight(" + query->toString() + L" in " + StringUtils::toString(doc) + L"), product of:");

    StringStream docFreqsBuffer;
    StringStream queryBuffer;
    queryBuffer << L"\"";
    docFreqsBuffer << idfExp->explain();
    for (Collection<TermPtr>::iterator term = query->terms.begin(); term != query->terms.end(); ++term) {
        if (term != query->terms.begin()) {
            queryBuffer << L" ";
        }
        queryBuffer << (*term)->text();
    }
    queryBuffer << L"\"";

    ExplanationPtr idfExpl(newLucene<Explanation>(idf, L"idf(" + query->field + L":" + docFreqsBuffer.str() + L")"));

    // explain query weight
    ExplanationPtr queryExpl(newLucene<Explanation>());
    queryExpl->setDescription(L"queryWeight(" + query->toString() + L"), product of:");

    ExplanationPtr boostExpl(newLucene<Explanation>(query->getBoost(), L"boost"));
    if (query->getBoost() != 1.0) {
        queryExpl->addDetail(boostExpl);
    }
    queryExpl->addDetail(idfExpl);

    ExplanationPtr queryNormExpl(newLucene<Explanation>(queryNorm, L"queryNorm"));
    queryExpl->addDetail(queryNormExpl);

    queryExpl->setValue(boostExpl->getValue() * idfExpl->getValue() * queryNormExpl->getValue());
    result->addDetail(queryExpl);

    // explain field weight
    ExplanationPtr fieldExpl(newLucene<Explanation>());
    fieldExpl->setDescription(L"fieldWeight(" +    query->field + L":" + query->toString() + L" in " + StringUtils::toString(doc) + L"), product of:");

    PhraseScorerPtr phraseScorer(std::dynamic_pointer_cast<PhraseScorer>(scorer(reader, true, false)));
    if (!phraseScorer) {
        return newLucene<Explanation>(0.0, L"no matching docs");
    }

    ExplanationPtr tfExplanation(newLucene<Explanation>());
    int32_t d = phraseScorer->advance(doc);
    double phraseFreq = d == doc ? phraseScorer->currentFreq() : 0.0;
    tfExplanation->setValue(similarity->tf(phraseFreq));
    tfExplanation->setDescription(L"tf(phraseFreq=" + StringUtils::toString(phraseFreq) + L")");

    fieldExpl->addDetail(tfExplanation);
    fieldExpl->addDetail(idfExpl);

    ExplanationPtr fieldNormExpl(newLucene<Explanation>());
    ByteArray fieldNorms(reader->norms(query->field));
    double fieldNorm = fieldNorms ? Similarity::decodeNorm(fieldNorms[doc]) : 1.0;
    fieldNormExpl->setValue(fieldNorm);
    fieldNormExpl->setDescription(L"fieldNorm(field=" + query->field + L", doc=" + StringUtils::toString(doc) + L")");
    fieldExpl->addDetail(fieldNormExpl);

    fieldExpl->setValue(tfExplanation->getValue() * idfExpl->getValue() * fieldNormExpl->getValue());

    result->addDetail(fieldExpl);

    // combine them
    result->setValue(queryExpl->getValue() * fieldExpl->getValue());

    if (queryExpl->getValue() == 1.0) {
        return fieldExpl;
    }

    return result;
}

}
