/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef SCORECACHINGWRAPPINGSCORER_H
#define SCORECACHINGWRAPPINGSCORER_H

#include "fnord-fts/search/Scorer.h"

namespace fnord {
namespace fts {

/// A {@link Scorer} which wraps another scorer and caches the score of the current document. Successive
/// calls to {@link #score()} will return the same result and will not invoke the wrapped Scorer's score()
/// method, unless the current document has changed.
///
/// This class might be useful due to the changes done to the {@link Collector} interface, in which the
/// score is not computed for a document by default, only if the collector requests it. Some collectors
/// may need to use the score in several places, however all they have in hand is a {@link Scorer} object,
/// and might end up computing the score of a document more than once.
class ScoreCachingWrappingScorer : public Scorer {
public:
    /// Creates a new instance by wrapping the given scorer.
    ScoreCachingWrappingScorer(const ScorerPtr& scorer);
    virtual ~ScoreCachingWrappingScorer();

    LUCENE_CLASS(ScoreCachingWrappingScorer);

protected:
    ScorerWeakPtr _scorer;
    int32_t curDoc;
    double curScore;

public:
    SimilarityPtr getSimilarity();
    virtual double score();
    virtual int32_t docID();
    virtual int32_t nextDoc();
    virtual void score(const CollectorPtr& collector);
    virtual int32_t advance(int32_t target);

protected:
    virtual bool score(const CollectorPtr& collector, int32_t max, int32_t firstDocID);
};

}

}
#endif
