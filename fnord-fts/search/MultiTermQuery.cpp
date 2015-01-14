/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "fnord-fts/fts.h"
#include "fnord-fts/search/MultiTermQuery.h"
#include "fnord-fts/search/_MultiTermQuery.h"
#include "fnord-fts/search/ConstantScoreQuery.h"
#include "fnord-fts/search/MultiTermQueryWrapperFilter.h"
#include "fnord-fts/search/QueryWrapperFilter.h"
#include "fnord-fts/search/BooleanQuery.h"
#include "fnord-fts/index/Term.h"
#include "fnord-fts/search/TermQuery.h"
#include "fnord-fts/index/TermDocs.h"
#include "fnord-fts/search/FilteredTermEnum.h"
#include "fnord-fts/index/IndexReader.h"
#include "fnord-fts/util/MiscUtils.h"

namespace Lucene {

MultiTermQuery::MultiTermQuery() {
    numberOfTerms = 0;
    rewriteMethod = CONSTANT_SCORE_AUTO_REWRITE_DEFAULT();
}

MultiTermQuery::~MultiTermQuery() {
}

RewriteMethodPtr MultiTermQuery::CONSTANT_SCORE_FILTER_REWRITE() {
    static RewriteMethodPtr _CONSTANT_SCORE_FILTER_REWRITE;
    if (!_CONSTANT_SCORE_FILTER_REWRITE) {
        _CONSTANT_SCORE_FILTER_REWRITE = newLucene<ConstantScoreFilterRewrite>();
        CycleCheck::addStatic(_CONSTANT_SCORE_FILTER_REWRITE);
    }
    return _CONSTANT_SCORE_FILTER_REWRITE;
}

RewriteMethodPtr MultiTermQuery::SCORING_BOOLEAN_QUERY_REWRITE() {
    static RewriteMethodPtr _SCORING_BOOLEAN_QUERY_REWRITE;
    if (!_SCORING_BOOLEAN_QUERY_REWRITE) {
        _SCORING_BOOLEAN_QUERY_REWRITE = newLucene<ScoringBooleanQueryRewrite>();
        CycleCheck::addStatic(_SCORING_BOOLEAN_QUERY_REWRITE);
    }
    return _SCORING_BOOLEAN_QUERY_REWRITE;
}

RewriteMethodPtr MultiTermQuery::CONSTANT_SCORE_BOOLEAN_QUERY_REWRITE() {
    static RewriteMethodPtr _CONSTANT_SCORE_BOOLEAN_QUERY_REWRITE;
    if (!_CONSTANT_SCORE_BOOLEAN_QUERY_REWRITE) {
        _CONSTANT_SCORE_BOOLEAN_QUERY_REWRITE = newLucene<ConstantScoreBooleanQueryRewrite>();
        CycleCheck::addStatic(_CONSTANT_SCORE_BOOLEAN_QUERY_REWRITE);
    }
    return _CONSTANT_SCORE_BOOLEAN_QUERY_REWRITE;
}

RewriteMethodPtr MultiTermQuery::CONSTANT_SCORE_AUTO_REWRITE_DEFAULT() {
    static RewriteMethodPtr _CONSTANT_SCORE_AUTO_REWRITE_DEFAULT;
    if (!_CONSTANT_SCORE_AUTO_REWRITE_DEFAULT) {
        _CONSTANT_SCORE_AUTO_REWRITE_DEFAULT = newLucene<ConstantScoreAutoRewriteDefault>();
        CycleCheck::addStatic(_CONSTANT_SCORE_AUTO_REWRITE_DEFAULT);
    }
    return _CONSTANT_SCORE_AUTO_REWRITE_DEFAULT;
}

int32_t MultiTermQuery::getTotalNumberOfTerms() {
    return numberOfTerms;
}

void MultiTermQuery::clearTotalNumberOfTerms() {
    numberOfTerms = 0;
}

void MultiTermQuery::incTotalNumberOfTerms(int32_t inc) {
    numberOfTerms += inc;
}

QueryPtr MultiTermQuery::rewrite(const IndexReaderPtr& reader) {
    return rewriteMethod->rewrite(reader, shared_from_this());
}

RewriteMethodPtr MultiTermQuery::getRewriteMethod() {
    return rewriteMethod;
}

void MultiTermQuery::setRewriteMethod(const RewriteMethodPtr& method) {
    rewriteMethod = method;
}

LuceneObjectPtr MultiTermQuery::clone(const LuceneObjectPtr& other) {
    LuceneObjectPtr clone = Query::clone(other);
    MultiTermQueryPtr cloneQuery(std::dynamic_pointer_cast<MultiTermQuery>(clone));
    cloneQuery->rewriteMethod = rewriteMethod;
    cloneQuery->numberOfTerms = numberOfTerms;
    return cloneQuery;
}

int32_t MultiTermQuery::hashCode() {
    int32_t prime = 31;
    int32_t result = 1;
    result = prime * result + MiscUtils::doubleToIntBits(getBoost());
    result = prime * result;
    result += rewriteMethod->hashCode();
    return result;
}

bool MultiTermQuery::equals(const LuceneObjectPtr& other) {
    if (LuceneObject::equals(other)) {
        return true;
    }
    if (!other) {
        return false;
    }
    if (!MiscUtils::equalTypes(shared_from_this(), other)) {
        return false;
    }
    MultiTermQueryPtr otherMultiTermQuery(std::dynamic_pointer_cast<MultiTermQuery>(other));
    if (!otherMultiTermQuery) {
        return false;
    }
    if (MiscUtils::doubleToIntBits(getBoost()) != MiscUtils::doubleToIntBits(otherMultiTermQuery->getBoost())) {
        return false;
    }
    if (!rewriteMethod->equals(otherMultiTermQuery->rewriteMethod)) {
        return false;
    }
    return true;
}

RewriteMethod::~RewriteMethod() {
}

ConstantScoreFilterRewrite::~ConstantScoreFilterRewrite() {
}

QueryPtr ConstantScoreFilterRewrite::rewrite(const IndexReaderPtr& reader, const MultiTermQueryPtr& query) {
    QueryPtr result(newLucene<ConstantScoreQuery>(newLucene<MultiTermQueryWrapperFilter>(query)));
    result->setBoost(query->getBoost());
    return result;
}

ScoringBooleanQueryRewrite::~ScoringBooleanQueryRewrite() {
}

QueryPtr ScoringBooleanQueryRewrite::rewrite(const IndexReaderPtr& reader, const MultiTermQueryPtr& query) {
    FilteredTermEnumPtr enumerator(query->getEnum(reader));
    BooleanQueryPtr result(newLucene<BooleanQuery>(true));
    int32_t count = 0;
    LuceneException finally;
    try {
        do {
            TermPtr t(enumerator->term());
            if (t) {
                TermQueryPtr tq(newLucene<TermQuery>(t)); // found a match
                tq->setBoost(query->getBoost() * enumerator->difference()); // set the boost
                result->add(tq, BooleanClause::SHOULD); // add to query
                ++count;
            }
        } while (enumerator->next());
    } catch (LuceneException& e) {
        finally = e;
    }
    enumerator->close();
    finally.throwException();
    query->incTotalNumberOfTerms(count);
    return result;
}

ConstantScoreBooleanQueryRewrite::~ConstantScoreBooleanQueryRewrite() {
}

QueryPtr ConstantScoreBooleanQueryRewrite::rewrite(const IndexReaderPtr& reader, const MultiTermQueryPtr& query) {
    // strip the scores off
    QueryPtr result(newLucene<ConstantScoreQuery>(newLucene<QueryWrapperFilter>(ScoringBooleanQueryRewrite::rewrite(reader, query))));
    result->setBoost(query->getBoost());
    return result;
}

// Defaults derived from rough tests with a 20.0 million doc Wikipedia index.  With more than 350 terms
// in the query, the filter method is fastest
const int32_t ConstantScoreAutoRewrite::DEFAULT_TERM_COUNT_CUTOFF = 350;

// If the query will hit more than 1 in 1000 of the docs in the index (0.1%), the filter method is fastest
const double ConstantScoreAutoRewrite::DEFAULT_DOC_COUNT_PERCENT = 0.1;

ConstantScoreAutoRewrite::ConstantScoreAutoRewrite() {
    termCountCutoff = DEFAULT_TERM_COUNT_CUTOFF;
    docCountPercent = DEFAULT_DOC_COUNT_PERCENT;
}

ConstantScoreAutoRewrite::~ConstantScoreAutoRewrite() {
}

void ConstantScoreAutoRewrite::setTermCountCutoff(int32_t count) {
    termCountCutoff = count;
}

int32_t ConstantScoreAutoRewrite::getTermCountCutoff() {
    return termCountCutoff;
}

void ConstantScoreAutoRewrite::setDocCountPercent(double percent) {
    docCountPercent = percent;
}

double ConstantScoreAutoRewrite::getDocCountPercent() {
    return docCountPercent;
}

QueryPtr ConstantScoreAutoRewrite::rewrite(const IndexReaderPtr& reader, const MultiTermQueryPtr& query) {
    // Get the enum and start visiting terms.  If we exhaust the enum before hitting either of the
    // cutoffs, we use ConstantBooleanQueryRewrite; else ConstantFilterRewrite
    Collection<TermPtr> pendingTerms(Collection<TermPtr>::newInstance());
    int32_t docCountCutoff = (int32_t)((docCountPercent / 100.0) * (double)reader->maxDoc());
    int32_t termCountLimit = std::min(BooleanQuery::getMaxClauseCount(), termCountCutoff);
    int32_t docVisitCount = 0;

    FilteredTermEnumPtr enumerator(query->getEnum(reader));
    QueryPtr result;
    LuceneException finally;
    try {
        while (true) {
            TermPtr t(enumerator->term());
            if (t) {
                pendingTerms.add(t);
                // Loading the TermInfo from the terms dict here should not be costly, because 1) the
                // query/filter will load the TermInfo when it runs, and 2) the terms dict has a cache
                docVisitCount += reader->docFreq(t);
            }

            if (pendingTerms.size() >= termCountLimit || docVisitCount >= docCountCutoff) {
                // Too many terms -- make a filter.
                result = newLucene<ConstantScoreQuery>(newLucene<MultiTermQueryWrapperFilter>(query));
                result->setBoost(query->getBoost());
                break;
            } else if (!enumerator->next()) {
                // Enumeration is done, and we hit a small enough number of terms and docs -
                // just make a BooleanQuery, now
                BooleanQueryPtr bq(newLucene<BooleanQuery>(true));
                for (Collection<TermPtr>::iterator term = pendingTerms.begin(); term != pendingTerms.end(); ++ term) {
                    TermQueryPtr tq(newLucene<TermQuery>(*term));
                    bq->add(tq, BooleanClause::SHOULD);
                }
                // Strip scores
                result = newLucene<ConstantScoreQuery>(newLucene<QueryWrapperFilter>(bq));
                result->setBoost(query->getBoost());
                query->incTotalNumberOfTerms(pendingTerms.size());
                break;
            }
        }
    } catch (LuceneException& e) {
        finally = e;
    }
    enumerator->close();
    finally.throwException();
    return result;
}

int32_t ConstantScoreAutoRewrite::hashCode() {
    int32_t prime = 1279;
    return (int32_t)(prime * termCountCutoff + MiscUtils::doubleToLongBits(docCountPercent));
}

bool ConstantScoreAutoRewrite::equals(const LuceneObjectPtr& other) {
    if (RewriteMethod::equals(other)) {
        return true;
    }
    if (!other) {
        return false;
    }
    if (!MiscUtils::equalTypes(shared_from_this(), other)) {
        return false;
    }

    ConstantScoreAutoRewritePtr otherConstantScoreAutoRewrite(std::dynamic_pointer_cast<ConstantScoreAutoRewrite>(other));
    if (!otherConstantScoreAutoRewrite) {
        return false;
    }

    if (termCountCutoff != otherConstantScoreAutoRewrite->termCountCutoff) {
        return false;
    }

    if (MiscUtils::doubleToLongBits(docCountPercent) != MiscUtils::doubleToLongBits(otherConstantScoreAutoRewrite->docCountPercent)) {
        return false;
    }

    return true;
}

ConstantScoreAutoRewriteDefault::~ConstantScoreAutoRewriteDefault() {
}

void ConstantScoreAutoRewriteDefault::setTermCountCutoff(int32_t count) {
    boost::throw_exception(UnsupportedOperationException(L"Please create a private instance"));
}

void ConstantScoreAutoRewriteDefault::setDocCountPercent(double percent) {
    boost::throw_exception(UnsupportedOperationException(L"Please create a private instance"));
}

}
