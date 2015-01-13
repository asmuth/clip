/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "CustomScoreQuery.h"
#include "_CustomScoreQuery.h"
#include "ValueSourceQuery.h"
#include "ComplexExplanation.h"
#include "MiscUtils.h"
#include "StringUtils.h"

namespace Lucene {

CustomScoreQuery::CustomScoreQuery(const QueryPtr& subQuery) {
    ConstructQuery(subQuery, Collection<ValueSourceQueryPtr>::newInstance());
}

CustomScoreQuery::CustomScoreQuery(const QueryPtr& subQuery, const ValueSourceQueryPtr& valSrcQuery) {
    Collection<ValueSourceQueryPtr> valSrcQueries(Collection<ValueSourceQueryPtr>::newInstance());
    if (valSrcQuery) {
        valSrcQueries.add(valSrcQuery);
    }
    ConstructQuery(subQuery, valSrcQueries);
}

CustomScoreQuery::CustomScoreQuery(const QueryPtr& subQuery, Collection<ValueSourceQueryPtr> valSrcQueries) {
    ConstructQuery(subQuery, valSrcQueries);
}

CustomScoreQuery::~CustomScoreQuery() {
}

void CustomScoreQuery::ConstructQuery(const QueryPtr& subQuery, Collection<ValueSourceQueryPtr> valSrcQueries) {
    this->strict = false;
    this->subQuery = subQuery;
    this->valSrcQueries = valSrcQueries ? valSrcQueries : Collection<ValueSourceQueryPtr>::newInstance();
    if (!subQuery) {
        boost::throw_exception(IllegalArgumentException(L"<subquery> must not be null!"));
    }
}

QueryPtr CustomScoreQuery::rewrite(const IndexReaderPtr& reader) {
    CustomScoreQueryPtr cloneQuery;

    QueryPtr sq = subQuery->rewrite(reader);
    if (sq != subQuery) {
        cloneQuery = boost::static_pointer_cast<CustomScoreQuery>(clone());
        cloneQuery->subQuery = sq;
    }

    for (int32_t i = 0; i < valSrcQueries.size(); ++i) {
        ValueSourceQueryPtr v = boost::dynamic_pointer_cast<ValueSourceQuery>(valSrcQueries[i]->rewrite(reader));
        if (v != valSrcQueries[i]) {
            if (!cloneQuery) {
                cloneQuery = boost::static_pointer_cast<CustomScoreQuery>(clone());
            }
            cloneQuery->valSrcQueries[i] = v;
        }
    }

    return cloneQuery ? cloneQuery : shared_from_this();
}

void CustomScoreQuery::extractTerms(SetTerm terms) {
    subQuery->extractTerms(terms);
    for (Collection<ValueSourceQueryPtr>::iterator srcQuery = valSrcQueries.begin(); srcQuery != valSrcQueries.end(); ++srcQuery) {
        (*srcQuery)->extractTerms(terms);
    }
}

LuceneObjectPtr CustomScoreQuery::clone(const LuceneObjectPtr& other) {
    LuceneObjectPtr clone = Query::clone(other ? other : newLucene<CustomScoreQuery>(subQuery));
    CustomScoreQueryPtr cloneQuery(boost::dynamic_pointer_cast<CustomScoreQuery>(clone));
    cloneQuery->strict = strict;
    cloneQuery->subQuery = boost::dynamic_pointer_cast<Query>(subQuery->clone());
    cloneQuery->valSrcQueries = Collection<ValueSourceQueryPtr>::newInstance(valSrcQueries.size());
    for (int32_t i = 0; i < valSrcQueries.size(); ++i) {
        cloneQuery->valSrcQueries[i] = boost::dynamic_pointer_cast<ValueSourceQuery>(valSrcQueries[i]->clone());
    }
    return cloneQuery;
}

String CustomScoreQuery::toString(const String& field) {
    StringStream buffer;
    buffer << name() << L"(" << subQuery->toString(field);
    for (Collection<ValueSourceQueryPtr>::iterator srcQuery = valSrcQueries.begin(); srcQuery != valSrcQueries.end(); ++srcQuery) {
        buffer << L", " << (*srcQuery)->toString(field);
    }
    buffer << L")" << (strict ? L" STRICT" : L"") << boostString();
    return buffer.str();
}

bool CustomScoreQuery::equals(const LuceneObjectPtr& other) {
    CustomScoreQueryPtr otherQuery(boost::dynamic_pointer_cast<CustomScoreQuery>(other));
    if (!otherQuery) {
        return false;
    }
    if (getBoost() != otherQuery->getBoost() || !subQuery->equals(otherQuery->subQuery) || strict != otherQuery->strict) {
        return false;
    }
    return valSrcQueries.equals(otherQuery->valSrcQueries, luceneEquals<ValueSourceQueryPtr>());
}

int32_t CustomScoreQuery::hashCode() {
    return (StringUtils::hashCode(CustomScoreQuery::_getClassName()) + StringUtils::hashCode(Query::_getClassName()) +
            MiscUtils::hashCode(valSrcQueries.begin(), valSrcQueries.end(), MiscUtils::hashLucene<ValueSourceQueryPtr>)) ^
           MiscUtils::doubleToIntBits(getBoost()) ^ (strict ? 1234 : 4321);
}

CustomScoreProviderPtr CustomScoreQuery::getCustomScoreProvider(const IndexReaderPtr& reader) {
    // when deprecated methods are removed, do not extend class here, just return new default CustomScoreProvider
    return newLucene<DefaultCustomScoreProvider>(shared_from_this(), reader);
}

double CustomScoreQuery::customScore(int32_t doc, double subQueryScore, Collection<double> valSrcScores) {
    if (valSrcScores.size() == 1) {
        return customScore(doc, subQueryScore, valSrcScores[0]);
    }
    if (valSrcScores.empty()) {
        return customScore(doc, subQueryScore, 1);
    }
    double score = subQueryScore;
    for (Collection<double>::iterator srcScore = valSrcScores.begin(); srcScore != valSrcScores.end(); ++srcScore) {
        score *= *srcScore;
    }
    return score;
}

double CustomScoreQuery::customScore(int32_t doc, double subQueryScore, double valSrcScore) {
    return subQueryScore * valSrcScore;
}

ExplanationPtr CustomScoreQuery::customExplain(int32_t doc, const ExplanationPtr& subQueryExpl, Collection<ExplanationPtr> valSrcExpls) {
    if (valSrcExpls.size() == 1) {
        return customExplain(doc, subQueryExpl, valSrcExpls[0]);
    }
    if (valSrcExpls.empty()) {
        return subQueryExpl;
    }
    double valSrcScore = 1;
    for (Collection<ExplanationPtr>::iterator srcExpl = valSrcExpls.begin(); srcExpl != valSrcExpls.end(); ++srcExpl) {
        valSrcScore *= (*srcExpl)->getValue();
    }
    ExplanationPtr exp(newLucene<Explanation>(valSrcScore * subQueryExpl->getValue(), L"custom score: product of:"));
    exp->addDetail(subQueryExpl);
    for (Collection<ExplanationPtr>::iterator srcExpl = valSrcExpls.begin(); srcExpl != valSrcExpls.end(); ++srcExpl) {
        exp->addDetail(*srcExpl);
    }
    return exp;
}

ExplanationPtr CustomScoreQuery::customExplain(int32_t doc, const ExplanationPtr& subQueryExpl, const ExplanationPtr& valSrcExpl) {
    double valSrcScore = 1;
    if (valSrcExpl) {
        valSrcScore *= valSrcExpl->getValue();
    }
    ExplanationPtr exp(newLucene<Explanation>(valSrcScore * subQueryExpl->getValue(), L"custom score: product of:"));
    exp->addDetail(subQueryExpl);
    exp->addDetail(valSrcExpl);
    return exp;
}

WeightPtr CustomScoreQuery::createWeight(const SearcherPtr& searcher) {
    return newLucene<CustomWeight>(shared_from_this(), searcher);
}

bool CustomScoreQuery::isStrict() {
    return strict;
}

void CustomScoreQuery::setStrict(bool strict) {
    this->strict = strict;
}

String CustomScoreQuery::name() {
    return L"custom";
}

DefaultCustomScoreProvider::DefaultCustomScoreProvider(const CustomScoreQueryPtr& customQuery, const IndexReaderPtr& reader) : CustomScoreProvider(reader) {
    _customQuery = customQuery;
}

DefaultCustomScoreProvider::~DefaultCustomScoreProvider() {
}

double DefaultCustomScoreProvider::customScore(int32_t doc, double subQueryScore, Collection<double> valSrcScores) {
    return CustomScoreQueryPtr(_customQuery)->customScore(doc, subQueryScore, valSrcScores);
}

double DefaultCustomScoreProvider::customScore(int32_t doc, double subQueryScore, double valSrcScore) {
    return CustomScoreQueryPtr(_customQuery)->customScore(doc, subQueryScore, valSrcScore);
}

ExplanationPtr DefaultCustomScoreProvider::customExplain(int32_t doc, const ExplanationPtr& subQueryExpl, Collection<ExplanationPtr> valSrcExpls) {
    return CustomScoreQueryPtr(_customQuery)->customExplain(doc, subQueryExpl, valSrcExpls);
}

ExplanationPtr DefaultCustomScoreProvider::customExplain(int32_t doc, const ExplanationPtr& subQueryExpl, const ExplanationPtr& valSrcExpl) {
    return CustomScoreQueryPtr(_customQuery)->customExplain(doc, subQueryExpl, valSrcExpl);
}

CustomWeight::CustomWeight(const CustomScoreQueryPtr& query, const SearcherPtr& searcher) {
    this->query = query;
    this->similarity = query->getSimilarity(searcher);
    this->subQueryWeight = query->subQuery->weight(searcher);
    this->valSrcWeights = Collection<WeightPtr>::newInstance(query->valSrcQueries.size());
    for (int32_t i = 0; i < query->valSrcQueries.size(); ++i) {
        this->valSrcWeights[i] = query->valSrcQueries[i]->createWeight(searcher);
    }
    this->qStrict = query->strict;
}

CustomWeight::~CustomWeight() {
}

QueryPtr CustomWeight::getQuery() {
    return query;
}

double CustomWeight::getValue() {
    return query->getBoost();
}

double CustomWeight::sumOfSquaredWeights() {
    double sum = subQueryWeight->sumOfSquaredWeights();
    for (int32_t i = 0; i < valSrcWeights.size(); ++i) {
        if (qStrict) {
            valSrcWeights[i]->sumOfSquaredWeights();    // do not include ValueSource part in the query normalization
        } else {
            sum += valSrcWeights[i]->sumOfSquaredWeights();
        }
    }
    sum *= query->getBoost() * query->getBoost(); // boost each sub-weight
    return sum;
}

void CustomWeight::normalize(double norm) {
    norm *= query->getBoost(); // incorporate boost
    subQueryWeight->normalize(norm);
    for (int32_t i = 0; i < valSrcWeights.size(); ++i) {
        if (qStrict) {
            valSrcWeights[i]->normalize(1.0);    // do not normalize the ValueSource part
        } else {
            valSrcWeights[i]->normalize(norm);
        }
    }
}

ScorerPtr CustomWeight::scorer(const IndexReaderPtr& reader, bool scoreDocsInOrder, bool topScorer) {
    // Pass true for "scoresDocsInOrder", because we require in-order scoring, even if caller does not,
    // since we call advance on the valSrcScorers.  Pass false for "topScorer" because we will not invoke
    // score(Collector) on these scorers
    ScorerPtr subQueryScorer(subQueryWeight->scorer(reader, true, false));
    if (!subQueryScorer) {
        return ScorerPtr();
    }
    Collection<ScorerPtr> valSrcScorers(Collection<ScorerPtr>::newInstance(valSrcWeights.size()));
    for (int32_t i = 0; i < valSrcScorers.size(); ++i) {
        valSrcScorers[i] = valSrcWeights[i]->scorer(reader, true, topScorer);
    }
    return newLucene<CustomScorer>(similarity, reader, shared_from_this(), subQueryScorer, valSrcScorers);
}

ExplanationPtr CustomWeight::explain(const IndexReaderPtr& reader, int32_t doc) {
    ExplanationPtr explain(doExplain(reader, doc));
    return explain ? explain : newLucene<Explanation>(0.0, L"no matching docs");
}

ExplanationPtr CustomWeight::doExplain(const IndexReaderPtr& reader, int32_t doc) {
    ExplanationPtr subQueryExpl(subQueryWeight->explain(reader, doc));
    if (!subQueryExpl->isMatch()) {
        return subQueryExpl;
    }
    // match
    Collection<ExplanationPtr> valSrcExpls(Collection<ExplanationPtr>::newInstance(valSrcWeights.size()));
    for (int32_t i = 0; i < valSrcWeights.size(); ++i) {
        valSrcExpls[i] = valSrcWeights[i]->explain(reader, doc);
    }
    ExplanationPtr customExp(query->getCustomScoreProvider(reader)->customExplain(doc, subQueryExpl, valSrcExpls));
    double sc = getValue() * customExp->getValue();
    ExplanationPtr res(newLucene<ComplexExplanation>(true, sc, query->toString() + L", product of:"));
    res->addDetail(customExp);
    res->addDetail(newLucene<Explanation>(getValue(), L"queryBoost")); // actually using the q boost as q weight (== weight value)
    return res;
}

bool CustomWeight::scoresDocsOutOfOrder() {
    return false;
}

CustomScorer::CustomScorer(const SimilarityPtr& similarity, const IndexReaderPtr& reader, const CustomWeightPtr& weight, const ScorerPtr& subQueryScorer, Collection<ScorerPtr> valSrcScorers) : Scorer(similarity) {
    this->qWeight = weight->getValue();
    this->subQueryScorer = subQueryScorer;
    this->valSrcScorers = valSrcScorers;
    this->reader = reader;
    this->vScores = Collection<double>::newInstance(valSrcScorers.size());
    this->provider = weight->query->getCustomScoreProvider(reader);
}

CustomScorer::~CustomScorer() {
}

int32_t CustomScorer::nextDoc() {
    int32_t doc = subQueryScorer->nextDoc();
    if (doc != NO_MORE_DOCS) {
        for (int32_t i = 0; i < valSrcScorers.size(); ++i) {
            valSrcScorers[i]->advance(doc);
        }
    }
    return doc;
}

int32_t CustomScorer::docID() {
    return subQueryScorer->docID();
}

double CustomScorer::score() {
    for (int32_t i = 0; i < valSrcScorers.size(); ++i) {
        vScores[i] = valSrcScorers[i]->score();
    }
    return qWeight * provider->customScore(subQueryScorer->docID(), subQueryScorer->score(), vScores);
}

int32_t CustomScorer::advance(int32_t target) {
    int32_t doc = subQueryScorer->advance(target);
    if (doc != NO_MORE_DOCS) {
        for (int32_t i = 0; i < valSrcScorers.size(); ++i) {
            valSrcScorers[i]->advance(doc);
        }
    }
    return doc;
}

}
