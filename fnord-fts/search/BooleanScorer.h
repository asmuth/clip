/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef BOOLEANSCORER_H
#define BOOLEANSCORER_H

#include "Scorer.h"
#include "Collector.h"

namespace Lucene {

/// BooleanScorer uses a ~16k array to score windows of docs. So it scores docs 0-16k first, then docs 16-32k,
/// etc. For each window it iterates through all query terms and accumulates a score in table[doc%16k]. It also
/// stores in the table a bitmask representing which terms contributed to the score.  Non-zero scores are chained
/// in a linked list. At the end of scoring each window it then iterates through the linked list and, if the
/// bitmask matches the boolean constraints, collects a hit.  For boolean queries with lots of frequent terms this
/// can be much faster, since it does not need to update a priority queue for each posting, instead performing
/// constant-time operations per posting.  The only downside is that it results in hits being delivered out-of-order
/// within the window, which means it cannot be nested within other scorers.  But it works well as a top-level scorer.
///
/// The new BooleanScorer2 implementation instead works by merging priority queues of postings, albeit with some
/// clever tricks.  For example, a pure conjunction (all terms required) does not require a priority queue. Instead it
/// sorts the posting streams at the start, then repeatedly skips the first to to the last.  If the first ever equals
/// the last, then there's a hit.  When some terms are required and some terms are optional, the conjunction can
/// be evaluated first, then the optional terms can all skip to the match and be added to the score. Thus the
/// conjunction can reduce the number of priority queue updates for the optional terms.
class BooleanScorer : public Scorer {
public:
    BooleanScorer(const SimilarityPtr& similarity, int32_t minNrShouldMatch, Collection<ScorerPtr> optionalScorers, Collection<ScorerPtr> prohibitedScorers);
    virtual ~BooleanScorer();

    LUCENE_CLASS(BooleanScorer);

protected:
    SubScorerPtr scorers;
    BucketTablePtr bucketTable;
    int32_t maxCoord;
    Collection<double> coordFactors;
    int32_t requiredMask;
    int32_t prohibitedMask;
    int32_t nextMask;
    int32_t minNrShouldMatch;
    int32_t end;
    BucketPtr current;
    int32_t doc;

protected:
    // firstDocID is ignored since nextDoc() initializes 'current'
    virtual bool score(const CollectorPtr& collector, int32_t max, int32_t firstDocID);

public:
    virtual int32_t advance(int32_t target);
    virtual int32_t docID();
    virtual int32_t nextDoc();
    virtual double score();
    virtual void score(const CollectorPtr& collector);
    virtual String toString();
};

class BooleanScorerCollector : public Collector {
public:
    BooleanScorerCollector(int32_t mask, const BucketTablePtr& bucketTable);
    virtual ~BooleanScorerCollector();

    LUCENE_CLASS(BooleanScorerCollector);

protected:
    BucketTableWeakPtr _bucketTable;
    int32_t mask;
    ScorerWeakPtr _scorer;

public:
    virtual void collect(int32_t doc);
    virtual void setNextReader(const IndexReaderPtr& reader, int32_t docBase);
    virtual void setScorer(const ScorerPtr& scorer);
    virtual bool acceptsDocsOutOfOrder();
};

// An internal class which is used in score(Collector, int32_t) for setting the current score. This is required
// since Collector exposes a setScorer method and implementations that need the score will call scorer->score().
// Therefore the only methods that are implemented are score() and doc().
class BucketScorer : public Scorer {
public:
    BucketScorer();
    virtual ~BucketScorer();

    LUCENE_CLASS(BucketScorer);

public:
    double _score;
    int32_t doc;

public:
    virtual int32_t advance(int32_t target);
    virtual int32_t docID();
    virtual int32_t nextDoc();
    virtual double score();
};

class Bucket : public LuceneObject {
public:
    Bucket();
    virtual ~Bucket();

    LUCENE_CLASS(Bucket);

public:
    int32_t doc; // tells if bucket is valid
    double score; // incremental score
    int32_t bits; // used for bool constraints
    int32_t coord; // count of terms in score
    BucketWeakPtr _next; // next valid bucket
};

/// A simple hash table of document scores within a range.
class BucketTable : public LuceneObject {
public:
    BucketTable();
    virtual ~BucketTable();

    LUCENE_CLASS(BucketTable);

public:
    static const int32_t SIZE;
    static const int32_t MASK;

    Collection<BucketPtr> buckets;
    BucketPtr first; // head of valid list

public:
    CollectorPtr newCollector(int32_t mask);
    int32_t size();
};

class SubScorer : public LuceneObject {
public:
    SubScorer(const ScorerPtr& scorer, bool required, bool prohibited, const CollectorPtr& collector, const SubScorerPtr& next);
    virtual ~SubScorer();

    LUCENE_CLASS(SubScorer);

public:
    ScorerPtr scorer;
    bool required;
    bool prohibited;
    CollectorPtr collector;
    SubScorerPtr next;
};

}

#endif
