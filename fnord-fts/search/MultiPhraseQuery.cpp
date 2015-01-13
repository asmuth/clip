/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "fnord-fts/LuceneInc.h"
#include "fnord-fts/search/MultiPhraseQuery.h"
#include "fnord-fts/search/_MultiPhraseQuery.h"
#include "fnord-fts/search/Searcher.h"
#include "fnord-fts/index/Term.h"
#include "fnord-fts/search/TermQuery.h"
#include "fnord-fts/index/MultipleTermPositions.h"
#include "fnord-fts/search/ExactPhraseScorer.h"
#include "fnord-fts/search/SloppyPhraseScorer.h"
#include "fnord-fts/search/Similarity.h"
#include "fnord-fts/index/IndexReader.h"
#include "fnord-fts/search/ComplexExplanation.h"
#include "fnord-fts/search/BooleanQuery.h"
#include "fnord-fts/util/MiscUtils.h"
#include "fnord-fts/util/StringUtils.h"

namespace Lucene {

MultiPhraseQuery::MultiPhraseQuery() {
    termArrays = Collection< Collection<TermPtr> >::newInstance();
    positions = Collection<int32_t>::newInstance();
    slop = 0;
}

MultiPhraseQuery::~MultiPhraseQuery() {
}

void MultiPhraseQuery::setSlop(int32_t s) {
    slop = s;
}

int32_t MultiPhraseQuery::getSlop() {
    return slop;
}

void MultiPhraseQuery::add(const TermPtr& term) {
    add(newCollection<TermPtr>(term));
}

void MultiPhraseQuery::add(Collection<TermPtr> terms) {
    int32_t position = 0;
    if (!positions.empty()) {
        position = positions[positions.size() - 1] + 1;
    }
    add(terms, position);
}

void MultiPhraseQuery::add(Collection<TermPtr> terms, int32_t position) {
    if (termArrays.empty()) {
        field = terms[0]->field();
    }
    for (Collection<TermPtr>::iterator term = terms.begin(); term != terms.end(); ++term) {
        if ((*term)->field() != field) {
            boost::throw_exception(IllegalArgumentException(L"All phrase terms must be in the same field (" + field + L"): " + (*term)->toString()));
        }
    }
    termArrays.add(terms);
    positions.add(position);
}

Collection< Collection<TermPtr> > MultiPhraseQuery::getTermArrays() {
    return termArrays;
}

Collection<int32_t> MultiPhraseQuery::getPositions() {
    return positions;
}

void MultiPhraseQuery::extractTerms(SetTerm terms) {
    for (Collection< Collection<TermPtr> >::iterator arr = termArrays.begin(); arr != termArrays.end(); ++arr) {
        for (Collection<TermPtr>::iterator term = arr->begin(); term != arr->end(); ++term) {
            terms.add(*term);
        }
    }
}

QueryPtr MultiPhraseQuery::rewrite(const IndexReaderPtr& reader) {
    if (termArrays.size() == 1) { // optimize one-term case
        Collection<TermPtr> terms(termArrays[0]);
        BooleanQueryPtr boq(newLucene<BooleanQuery>(true));
        for (Collection<TermPtr>::iterator term = terms.begin(); term != terms.end(); ++term) {
            boq->add(newLucene<TermQuery>(*term), BooleanClause::SHOULD);
        }
        boq->setBoost(getBoost());
        return boq;
    } else {
        return shared_from_this();
    }
}

WeightPtr MultiPhraseQuery::createWeight(const SearcherPtr& searcher) {
    return newLucene<MultiPhraseWeight>(shared_from_this(), searcher);
}

String MultiPhraseQuery::toString(const String& field) {
    StringStream buffer;
    if (this->field != field) {
        buffer << this->field << L":";
    }
    buffer << L"\"";
    for (Collection< Collection<TermPtr> >::iterator arr = termArrays.begin(); arr != termArrays.end(); ++arr) {
        if (arr != termArrays.begin()) {
            buffer << L" ";
        }
        if (arr->size() > 1) {
            buffer << L"(";
            for (Collection<TermPtr>::iterator term = arr->begin(); term != arr->end(); ++term) {
                if (term != arr->begin()) {
                    buffer << L" ";
                }
                buffer << (*term)->text();
            }
            buffer << L")";
        } else if (!arr->empty()) {
            buffer << (*arr)[0]->text();
        }
    }
    buffer << L"\"";

    if (slop != 0) {
        buffer << L"~" << slop;
    }

    buffer << boostString();

    return buffer.str();
}

bool MultiPhraseQuery::equals(const LuceneObjectPtr& other) {
    if (LuceneObject::equals(other)) {
        return true;
    }

    MultiPhraseQueryPtr otherMultiPhraseQuery(std::dynamic_pointer_cast<MultiPhraseQuery>(other));
    if (!otherMultiPhraseQuery) {
        return false;
    }

    return (getBoost() == otherMultiPhraseQuery->getBoost() && slop == otherMultiPhraseQuery->slop &&
            termArraysEquals(termArrays, otherMultiPhraseQuery->termArrays) &&
            positions.equals(otherMultiPhraseQuery->positions));
}

int32_t MultiPhraseQuery::hashCode() {
    return MiscUtils::doubleToIntBits(getBoost()) ^ slop ^  termArraysHashCode() ^ MiscUtils::hashCode(positions.begin(), positions.end(), MiscUtils::hashNumeric<int32_t>) ^ 0x4ac65113;
}

int32_t MultiPhraseQuery::termArraysHashCode() {
    int32_t hashCode = 1;
    for (Collection< Collection<TermPtr> >::iterator arr = termArrays.begin(); arr != termArrays.end(); ++arr) {
        hashCode = 31 * hashCode + MiscUtils::hashCode(arr->begin(), arr->end(), MiscUtils::hashLucene<TermPtr>);
    }
    return hashCode;
}

struct equalTermArrays {
    inline bool operator()(const Collection<TermPtr>& first, const Collection<TermPtr>& second) const {
        if (first.size() != second.size()) {
            return false;
        }
        return first.equals(second, luceneEquals<TermPtr>());
    }
};

bool MultiPhraseQuery::termArraysEquals(Collection< Collection<TermPtr> > first, Collection< Collection<TermPtr> > second) {
    return first.equals(second, equalTermArrays());
}

LuceneObjectPtr MultiPhraseQuery::clone(const LuceneObjectPtr& other) {
    LuceneObjectPtr clone = other ? other : newLucene<MultiPhraseQuery>();
    MultiPhraseQueryPtr cloneQuery(std::dynamic_pointer_cast<MultiPhraseQuery>(Query::clone(clone)));
    cloneQuery->field = field;
    cloneQuery->termArrays = termArrays;
    cloneQuery->positions = positions;
    cloneQuery->slop = slop;
    return cloneQuery;
}

MultiPhraseWeight::MultiPhraseWeight(const MultiPhraseQueryPtr& query, const SearcherPtr& searcher) {
    this->query = query;
    this->similarity = query->getSimilarity(searcher);
    this->value = 0.0;
    this->idf = 0.0;
    this->queryNorm = 0.0;
    this->queryWeight = 0.0;

    // compute idf
    int32_t maxDoc = searcher->maxDoc();
    for (Collection< Collection<TermPtr> >::iterator arr = query->termArrays.begin(); arr != query->termArrays.end(); ++arr) {
        for (Collection<TermPtr>::iterator term = arr->begin(); term != arr->end(); ++term) {
            idf += this->similarity->idf(searcher->docFreq(*term), maxDoc);
        }
    }
}

MultiPhraseWeight::~MultiPhraseWeight() {
}

QueryPtr MultiPhraseWeight::getQuery() {
    return query;
}

double MultiPhraseWeight::getValue() {
    return value;
}

double MultiPhraseWeight::sumOfSquaredWeights() {
    queryWeight = idf * getQuery()->getBoost(); // compute query weight
    return queryWeight * queryWeight; // square it
}

void MultiPhraseWeight::normalize(double norm) {
    queryNorm = norm;
    queryWeight *= queryNorm; // normalize query weight
    value = queryWeight * idf; // idf for document
}

ScorerPtr MultiPhraseWeight::scorer(const IndexReaderPtr& reader, bool scoreDocsInOrder, bool topScorer) {
    if (query->termArrays.empty()) { // optimize zero-term case
        return ScorerPtr();
    }

    Collection<TermPositionsPtr> tps(Collection<TermPositionsPtr>::newInstance(query->termArrays.size()));
    for (int32_t i = 0; i < tps.size(); ++i) {
        Collection<TermPtr> terms(query->termArrays[i]);

        TermPositionsPtr p;
        if (terms.size() > 1) {
            p = newLucene<MultipleTermPositions>(reader, terms);
        } else {
            p = reader->termPositions(terms[0]);
        }

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

ExplanationPtr MultiPhraseWeight::explain(const IndexReaderPtr& reader, int32_t doc) {
    ComplexExplanationPtr result(newLucene<ComplexExplanation>());
    result->setDescription(L"weight(" + query->toString() + L" in " + StringUtils::toString(doc) + L"), product of:");

    ExplanationPtr idfExpl(newLucene<Explanation>(idf, L"idf(" + query->toString() + L")"));

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
    ComplexExplanationPtr fieldExpl(newLucene<ComplexExplanation>());
    fieldExpl->setDescription(L"fieldWeight(" + query->toString() + L" in " + StringUtils::toString(doc) + L"), product of:");

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

    fieldExpl->setMatch(tfExplanation->isMatch());
    fieldExpl->setValue(tfExplanation->getValue() * idfExpl->getValue() * fieldNormExpl->getValue());

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
