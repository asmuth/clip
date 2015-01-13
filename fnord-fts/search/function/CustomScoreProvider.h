/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef CUSTOMSCOREPROVIDER_H
#define CUSTOMSCOREPROVIDER_H

#include "fnord-fts/util/LuceneObject.h"

namespace Lucene {

/// An instance of this subclass should be returned by {@link CustomScoreQuery#getCustomScoreProvider},
/// if you want to modify the custom score calculation of a {@link CustomScoreQuery}.
///
/// Since Lucene 2.9, queries operate on each segment of an Index separately, so overriding the similar
/// (now deprecated) methods in {@link CustomScoreQuery} is no longer suitable, as the supplied doc ID
/// is per-segment and without knowledge of the IndexReader you cannot access the document or {@link
/// FieldCache}.
class CustomScoreProvider : public LuceneObject {
public:
    /// Creates a new instance of the provider class for the given {@link IndexReader}.
    CustomScoreProvider(const IndexReaderPtr& reader);

    virtual ~CustomScoreProvider();

    LUCENE_CLASS(CustomScoreProvider);

protected:
    IndexReaderPtr reader;

public:
    /// Compute a custom score by the subQuery score and a number of ValueSourceQuery scores.
    ///
    /// Subclasses can override this method to modify the custom score.
    ///
    /// If your custom scoring is different than the default herein you should override at least one of
    /// the two customScore() methods.  If the number of ValueSourceQueries is always < 2 it is
    /// sufficient to override the other {@link #customScore(int32_t, double, double) customScore()}
    /// method, which is simpler.
    ///
    /// The default computation herein is a multiplication of given scores:
    /// <pre>
    /// ModifiedScore = valSrcScore * valSrcScores[0] * valSrcScores[1] * ...
    /// </pre>
    ///
    /// @param doc id of scored doc.
    /// @param subQueryScore score of that doc by the subQuery.
    /// @param valSrcScores scores of that doc by the ValueSourceQuery.
    /// @return custom score.
    virtual double customScore(int32_t doc, double subQueryScore, Collection<double> valSrcScores);

    /// Compute a custom score by the subQuery score and the ValueSourceQuery score.
    ///
    /// Subclasses can override this method to modify the custom score.
    ///
    /// If your custom scoring is different than the default herein you should override at least one of the
    /// two customScore() methods.  If the number of ValueSourceQueries is always < 2 it is sufficient to
    /// override this customScore() method, which is simpler.
    ///
    /// The default computation herein is a multiplication of the two scores:
    /// <pre>
    /// ModifiedScore = subQueryScore * valSrcScore
    /// </pre>
    ///
    /// @param doc id of scored doc.
    /// @param subQueryScore score of that doc by the subQuery.
    /// @param valSrcScore score of that doc by the ValueSourceQuery.
    /// @return custom score.
    virtual double customScore(int32_t doc, double subQueryScore, double valSrcScore);

    /// Explain the custom score.  Whenever overriding {@link #customScore(int32_t, double, Collection<double>)},
    /// this method should also be overridden to provide the correct explanation for the part of the custom scoring.
    ///
    /// @param doc doc being explained.
    /// @param subQueryExpl explanation for the sub-query part.
    /// @param valSrcExpls explanation for the value source part.
    /// @return an explanation for the custom score
    virtual ExplanationPtr customExplain(int32_t doc, const ExplanationPtr& subQueryExpl, Collection<ExplanationPtr> valSrcExpls);

    /// Explain the custom score.  Whenever overriding {@link #customScore(int32_t, double, double)},
    /// this method should also be overridden to provide the correct explanation for the part of the custom scoring.
    /// @param doc doc being explained.
    /// @param subQueryExpl explanation for the sub-query part.
    /// @param valSrcExpl explanation for the value source part.
    /// @return an explanation for the custom score
    virtual ExplanationPtr customExplain(int32_t doc, const ExplanationPtr& subQueryExpl, const ExplanationPtr& valSrcExpl);
};

}

#endif
