/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "CustomScoreProvider.h"
#include "Explanation.h"

namespace Lucene {

CustomScoreProvider::CustomScoreProvider(const IndexReaderPtr& reader) {
    this->reader = reader;
}

CustomScoreProvider::~CustomScoreProvider() {
}

double CustomScoreProvider::customScore(int32_t doc, double subQueryScore, Collection<double> valSrcScores) {
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

double CustomScoreProvider::customScore(int32_t doc, double subQueryScore, double valSrcScore) {
    return subQueryScore * valSrcScore;
}

ExplanationPtr CustomScoreProvider::customExplain(int32_t doc, const ExplanationPtr& subQueryExpl, Collection<ExplanationPtr> valSrcExpls) {
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

ExplanationPtr CustomScoreProvider::customExplain(int32_t doc, const ExplanationPtr& subQueryExpl, const ExplanationPtr& valSrcExpl) {
    double valSrcScore = 1;
    if (valSrcExpl) {
        valSrcScore *= valSrcExpl->getValue();
    }
    ExplanationPtr exp(newLucene<Explanation>(valSrcScore * subQueryExpl->getValue(), L"custom score: product of:"));
    exp->addDetail(subQueryExpl);
    exp->addDetail(valSrcExpl);
    return exp;
}

}
