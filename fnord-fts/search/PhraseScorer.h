/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef PHRASESCORER_H
#define PHRASESCORER_H

#include "Scorer.h"

namespace Lucene {

/// Scoring functionality for phrase queries.  A document is considered matching if it contains the
/// phrase-query terms at "valid" positions. What "valid positions" are depends on the type of the
/// phrase query: for an exact phrase query terms are required  to appear in adjacent locations, while
/// for a sloppy phrase query some distance between the terms is allowed. The abstract method {@link
/// #phraseFreq()} of extending classes is invoked for each document containing all the phrase query
/// terms, in order to compute the frequency of the phrase query in that document. A non zero frequency
/// means a match.
class PhraseScorer : public Scorer {
public:
    PhraseScorer(const WeightPtr& weight, Collection<TermPositionsPtr> tps, Collection<int32_t> offsets, const SimilarityPtr& similarity, ByteArray norms);
    virtual ~PhraseScorer();

    LUCENE_CLASS(PhraseScorer);

protected:
    WeightPtr weight;
    ByteArray norms;
    double value;

    bool firstTime;
    bool more;
    PhraseQueuePtr pq;
    PhrasePositionsPtr first;
    PhrasePositionsPtr last;

    double freq; // phrase frequency in current doc as computed by phraseFreq().

public:
    virtual int32_t docID();
    virtual int32_t nextDoc();
    virtual double score();
    virtual int32_t advance(int32_t target);

    /// Phrase frequency in current doc as computed by phraseFreq().
    double currentFreq();

    virtual String toString();

protected:
    /// Next without initial increment
    bool doNext();

    /// For a document containing all the phrase query terms, compute the frequency of the phrase in
    /// that document.  A non zero frequency means a match.
    /// Note, that containing all phrase terms does not guarantee a match - they have to be found in
    /// matching locations.
    /// @return frequency of the phrase in current doc, 0 if not found.
    virtual double phraseFreq() = 0;

    void init();
    void sort();
    void pqToList();
    void firstToLast();
};

}

#endif
