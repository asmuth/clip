/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef SLOPPYPHRASESCORER_H
#define SLOPPYPHRASESCORER_H

#include "PhraseScorer.h"

namespace Lucene {

class SloppyPhraseScorer : public PhraseScorer {
public:
    SloppyPhraseScorer(const WeightPtr& weight, Collection<TermPositionsPtr> tps, Collection<int32_t> offsets, const SimilarityPtr& similarity, int32_t slop, ByteArray norms);
    virtual ~SloppyPhraseScorer();

    LUCENE_CLASS(SloppyPhraseScorer);

protected:
    int32_t slop;
    Collection<PhrasePositionsPtr> repeats;
    Collection<PhrasePositionsPtr> tmpPos; // for flipping repeating pps
    bool checkedRepeats;

public:
    /// Score a candidate doc for all slop-valid position-combinations (matches) encountered while
    /// traversing/hopping the PhrasePositions.  The score contribution of a match depends on the distance:
    /// - highest score for distance=0 (exact match).
    /// - score gets lower as distance gets higher.
    /// Example: for query "a b"~2, a document "x a b a y" can be scored twice: once for "a b" (distance=0),
    /// and once for "b a" (distance=2).
    /// Possibly not all valid combinations are encountered, because for efficiency we always propagate the
    /// least PhrasePosition. This allows to base on PriorityQueue and move forward faster.
    /// As result, for example, document "a b c b a" would score differently for queries "a b c"~4 and
    /// "c b a"~4, although they really are equivalent.  Similarly, for doc "a b c b a f g", query "c b"~2
    /// would get same score as "g f"~2, although "c b"~2 could be matched twice. We may want to fix this
    /// in the future (currently not, for performance reasons).
    virtual double phraseFreq();

protected:
    /// Flip pp2 and pp in the queue: pop until finding pp2, insert back all but pp2, insert pp back.
    /// Assumes: pp!=pp2, pp2 in pq, pp not in pq.  Called only when there are repeating pps.
    PhrasePositionsPtr flip(const PhrasePositionsPtr& pp, const PhrasePositionsPtr& pp2);

    /// Init PhrasePositions in place.
    /// There is a one time initialization for this scorer:
    /// - Put in repeats[] each pp that has another pp with same position in the doc.
    /// - Also mark each such pp by pp.repeats = true.
    /// Later can consult with repeats[] in termPositionsDiffer(pp), making that check efficient.
    /// In particular, this allows to score queries with no repetitions with no overhead due to this computation.
    /// - Example 1 - query with no repetitions: "ho my"~2
    /// - Example 2 - query with repetitions: "ho my my"~2
    /// - Example 3 - query with repetitions: "my ho my"~2
    /// Init per doc with repeats in query, includes propagating some repeating pp's to avoid false phrase detection.
    /// @return end (max position), or -1 if any term ran out (ie. done)
    int32_t initPhrasePositions();

    /// We disallow two pp's to have the same TermPosition, thereby verifying multiple occurrences in the query
    /// of the same word would go elsewhere in the matched doc.
    /// @return null if differ (i.e. valid) otherwise return the higher offset PhrasePositions out of the first
    /// two PPs found to not differ.
    PhrasePositionsPtr termPositionsDiffer(const PhrasePositionsPtr& pp);
};

}

#endif
