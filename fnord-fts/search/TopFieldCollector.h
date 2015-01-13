/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef TOPFIELDCOLLECTOR_H
#define TOPFIELDCOLLECTOR_H

#include "fnord-fts/search/TopDocsCollector.h"

namespace Lucene {

/// A {@link Collector} that sorts by {@link SortField} using {@link FieldComparator}s.
///
/// See the {@link #create(SortPtr, int32_t, bool, bool, bool, bool)} method for instantiating a TopFieldCollector.
class TopFieldCollector : public TopDocsCollector {
public:
    TopFieldCollector(const HitQueueBasePtr& pq, int32_t numHits, bool fillFields);
    virtual ~TopFieldCollector();

    LUCENE_CLASS(TopFieldCollector);

protected:
    bool fillFields;

    /// Stores the maximum score value encountered, needed for normalizing.  If document scores are not tracked,
    /// this value is initialized to NaN.
    double maxScore;

    int32_t numHits;
    FieldValueHitQueueEntryPtr bottom;
    bool queueFull;
    int32_t docBase;

public:
    /// Creates a new {@link TopFieldCollector} from the given arguments.
    ///
    /// NOTE: The instances returned by this method pre-allocate a full array of length numHits.
    ///
    /// @param sort The sort criteria (SortFields).
    /// @param numHits The number of results to collect.
    /// @param fillFields Specifies whether the actual field values should be returned on the results (FieldDoc).
    /// @param trackDocScores Specifies whether document scores should be tracked and set on the results. Note
    /// that if set to false, then the results' scores will be set to NaN.  Setting this to true affects
    /// performance, as it incurs the score computation on each competitive result.  Therefore if document scores
    /// are not required by the application, it is recommended to set it to false.
    /// @param trackMaxScore Specifies whether the query's maxScore should be tracked and set on the resulting
    /// {@link TopDocs}.  Note that if set to false, {@link TopDocs#getMaxScore()} returns NaN. Setting this to
    /// true affects performance as it incurs the score computation on each result. Also, setting this true
    /// automatically sets trackDocScores to true as well.
    /// @param docsScoredInOrder Specifies whether documents are scored in doc Id order or not by the given
    /// {@link Scorer} in {@link #setScorer(ScorerPtr)}.
    /// @return a {@link TopFieldCollector} instance which will sort the results by the sort criteria.
    static TopFieldCollectorPtr create(const SortPtr& sort, int32_t numHits, bool fillFields, bool trackDocScores, bool trackMaxScore, bool docsScoredInOrder);

    virtual void add(int32_t slot, int32_t doc, double score);

    virtual bool acceptsDocsOutOfOrder();

protected:
    static const Collection<ScoreDocPtr> EMPTY_SCOREDOCS();

    /// Only the following callback methods need to be overridden since topDocs(int32_t, int32_t) calls them to
    /// return the results.
    virtual void populateResults(Collection<ScoreDocPtr> results, int32_t howMany);

    virtual TopDocsPtr newTopDocs(Collection<ScoreDocPtr> results, int32_t start);
};

}

#endif
