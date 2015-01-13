/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef COLLECTOR_H
#define COLLECTOR_H

#include "fnord-fts/util/LuceneObject.h"

namespace Lucene {

/// Collectors are primarily meant to be used to gather raw results from a search, and implement sorting
/// or custom result filtering, collation, etc.
///
/// Lucene's core collectors are derived from Collector.  Likely your application can use one of these
/// classes, or subclass {@link TopDocsCollector}, instead of implementing Collector directly:
///
/// <ul>
/// <li>{@link TopDocsCollector} is an abstract base class that assumes you will retrieve the top N docs,
/// according to some criteria, after collection is done.
///
/// <li>{@link TopScoreDocCollector} is a concrete subclass {@link TopDocsCollector} and sorts according
/// to score + docID.  This is used internally by the {@link IndexSearcher} search methods that do not take
/// an explicit {@link Sort}. It is likely the most frequently used collector.
///
/// <li>{@link TopFieldCollector} subclasses {@link TopDocsCollector} and sorts according to a specified
/// {@link Sort} object (sort by field).  This is used internally by the {@link IndexSearcher} search methods
/// that take an explicit {@link Sort}.
///
/// <li>{@link TimeLimitingCollector}, which wraps any other Collector and aborts the search if it's taken too
/// much time.
///
/// <li>{@link PositiveScoresOnlyCollector} wraps any other Collector and prevents collection of hits whose
/// score is <= 0.0
///
/// </ul>
///
/// Collector decouples the score from the collected doc: the score computation is skipped entirely if it's not
/// needed.  Collectors that do need the score should implement the {@link #setScorer} method, to hold onto the
/// passed {@link Scorer} instance, and call {@link Scorer#score()} within the collect method to compute the
/// current hit's score.  If your collector may request the score for a single hit multiple times, you should use
/// {@link ScoreCachingWrappingScorer}.
///
/// NOTE: The doc that is passed to the collect method is relative to the current reader. If your collector needs
/// to resolve this to the docID space of the Multi*Reader, you must re-base it by recording the docBase from the
/// most recent setNextReader call.  Here's a simple example showing how to collect docIDs into a BitSet:
///
/// <pre>
/// class MyCollector : public Collector
/// {
/// public:
///     MyCollector(const BitSetPtr& bits)
///     {
///         this->bits = bits;
///         this->docBase = 0;
///     }
///
/// protected:
///     BitSetPtr bits;
///     int32_t docBase;
///
/// public:
///     virtual void setScorer(const ScorerPtr& scorer)
///     {
///         // ignore scorer
///     }
///
///     virtual void collect(int32_t doc)
///     {
///         bits->set(doc + docBase);
///     }
///
///     virtual void setNextReader(const IndexReaderPtr& reader, int32_t docBase)
///     {
///         this->docBase = docBase;
///     }
///
///     virtual bool acceptsDocsOutOfOrder()
///     {
///         return true; // accept docs out of order (for a BitSet it doesn't matter)
///     }
/// };
///
/// ...
///
/// SearcherPtr searcher = newLucene<IndexSearcher>(indexReader);
/// BitSetPtr bits = newLucene<BitSet>(indexReader->maxDoc());
/// searcher->search(query, newLucene<MyCollector>(bits));
///
/// </pre>
/// Not all collectors will need to rebase the docID.  For example, a collector that simply counts the
/// total number of hits would skip it.
///
/// NOTE: Prior to 2.9, Lucene silently filtered out hits with score <= 0.  As of 2.9, the core Collectors
/// no longer do that.  It's very unusual to have such hits (a negative query boost, or function query
/// returning negative custom scores, could cause it to happen).  If you need that behavior, use {@link
/// PositiveScoresOnlyCollector}.
class Collector : public LuceneObject {
public:
    virtual ~Collector();
    LUCENE_CLASS(Collector);

public:
    /// Called before successive calls to {@link #collect(int32_t)}. Implementations that need the score
    /// of the current document (passed-in to {@link #collect(int32_t)}), should save the passed-in Scorer
    /// and call scorer.score() when needed.
    virtual void setScorer(const ScorerPtr& scorer) = 0;

    /// Called once for every document matching a query, with the unbased document number.
    ///
    /// Note: This is called in an inner search loop. For good search performance, implementations of this
    /// method should not call {@link Searcher#doc(int32_t)} or {@link IndexReader#document(int32_t)} on
    /// every hit.  Doing so can slow searches by an order of magnitude or more.
    virtual void collect(int32_t doc) = 0;

    /// Called before collecting from each IndexReader. All doc ids in {@link #collect(int32_t)} will
    /// correspond to reader.  Add docBase to the current IndexReaders internal document id to re-base ids
    /// in {@link #collect(int32_t)}.
    /// @param reader next IndexReader
    /// @param docBase
    virtual void setNextReader(const IndexReaderPtr& reader, int32_t docBase) = 0;

    /// Return true if this collector does not require the matching docIDs to be delivered in int sort
    /// order (smallest to largest) to {@link #collect}.
    ///
    /// Most Lucene Query implementations will visit matching docIDs in order.  However, some queries
    /// (currently limited to certain cases of {@link BooleanQuery}) can achieve faster searching if the
    /// Collector allows them to deliver the docIDs out of order.
    ///
    /// Many collectors don't mind getting docIDs out of order, so it's important to return true here.
    virtual bool acceptsDocsOutOfOrder() = 0;
};

}

#endif
