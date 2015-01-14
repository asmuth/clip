/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef TOPSCOREDOCCOLLECTOR_H
#define TOPSCOREDOCCOLLECTOR_H

#include "fnord-fts/search/TopDocsCollector.h"

namespace fnord {
namespace fts {

/// A {@link Collector} implementation that collects the top-scoring hits, returning them as a {@link TopDocs}.
/// This is used by {@link IndexSearcher} to implement {@link TopDocs}-based search. Hits are sorted by score
/// descending and then (when the scores are tied) docID ascending.  When you create an instance of this
/// collector you should know in advance whether documents are going to be collected in doc Id order or not.
///
/// NOTE: The values Nan, NEGATIVE_INFINITY and POSITIVE_INFINITY are not valid scores.  This collector will
/// not properly collect hits with such scores.
class TopScoreDocCollector : public TopDocsCollector {
public:
    TopScoreDocCollector(int32_t numHits);
    virtual ~TopScoreDocCollector();

    LUCENE_CLASS(TopScoreDocCollector);

public:
    ScoreDocPtr pqTop;
    int32_t docBase;
    ScorerWeakPtr _scorer;

public:
    /// Creates a new {@link TopScoreDocCollector} given the number of hits to collect and whether documents
    /// are scored in order by the input {@link Scorer} to {@link #setScorer(ScorerPtr)}.
    ///
    /// NOTE: The instances returned by this method pre-allocate a full array of length numHits.
    static TopScoreDocCollectorPtr create(int32_t numHits, bool docsScoredInOrder);

    virtual void setNextReader(const IndexReaderPtr& reader, int32_t docBase);
    virtual void setScorer(const ScorerPtr& scorer);

protected:
    virtual TopDocsPtr newTopDocs(Collection<ScoreDocPtr> results, int32_t start);
};

}

}
#endif
