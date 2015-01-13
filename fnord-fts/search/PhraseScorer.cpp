/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "fnord-fts/LuceneInc.h"
#include "fnord-fts/PhraseScorer.h"
#include "fnord-fts/PhrasePositions.h"
#include "fnord-fts/PhraseQueue.h"
#include "fnord-fts/Weight.h"
#include "fnord-fts/Similarity.h"

namespace Lucene {

PhraseScorer::PhraseScorer(const WeightPtr& weight, Collection<TermPositionsPtr> tps, Collection<int32_t> offsets, const SimilarityPtr& similarity, ByteArray norms) : Scorer(similarity) {
    this->firstTime = true;
    this->more = true;
    this->freq = 0.0;

    this->norms = norms;
    this->weight = weight;
    this->value = weight->getValue();

    // convert tps to a list of phrase positions.
    // Note: phrase-position differs from term-position in that its position reflects the phrase offset: pp.pos = tp.pos - offset.
    // This allows to easily identify a matching (exact) phrase when all PhrasePositions have exactly the same position.
    for (int32_t i = 0; i < tps.size(); ++i) {
        PhrasePositionsPtr pp(newLucene<PhrasePositions>(tps[i], offsets[i]));
        if (last) { // add next to end of list
            last->_next = pp;
        } else {
            first = pp;
        }
        last = pp;
    }

    pq = newLucene<PhraseQueue>(tps.size()); // construct empty pq
    first->doc = -1;
}

PhraseScorer::~PhraseScorer() {
}

int32_t PhraseScorer::docID() {
    return first->doc;
}

int32_t PhraseScorer::nextDoc() {
    if (firstTime) {
        init();
        firstTime = false;
    } else if (more) {
        more = last->next();    // trigger further scanning
    }
    if (!doNext()) {
        first->doc = NO_MORE_DOCS;
    }
    return first->doc;
}

bool PhraseScorer::doNext() {
    while (more) {
        while (more && first->doc < last->doc) { // find doc with all the terms
            more = first->skipTo(last->doc); // skip first upto last and move it to the end
            firstToLast();
        }

        if (more) {
            // found a doc with all of the terms
            freq = phraseFreq(); // check for phrase
            if (freq == 0.0) { // no match
                more = last->next();    // trigger further scanning
            } else {
                return true;
            }
        }
    }
    return false; // no more matches
}

double PhraseScorer::score() {
    double raw = getSimilarity()->tf(freq) * value; // raw score
    return !norms ? raw : raw * Similarity::decodeNorm(norms[first->doc]); // normalize
}

int32_t PhraseScorer::advance(int32_t target) {
    firstTime = false;
    for (PhrasePositionsPtr pp(first); more && pp; pp = pp->_next) {
        more = pp->skipTo(target);
    }
    if (more) {
        sort();    // re-sort
    }
    if (!doNext()) {
        first->doc = NO_MORE_DOCS;
    }
    return first->doc;
}

double PhraseScorer::currentFreq() {
    return freq;
}

void PhraseScorer::init() {
    for (PhrasePositionsPtr pp(first); more && pp; pp = pp->_next) {
        more = pp->next();
    }
    if (more) {
        sort();
    }
}

void PhraseScorer::sort() {
    pq->clear();
    for (PhrasePositionsPtr pp(first); more && pp; pp = pp->_next) {
        pq->add(pp);
    }
    pqToList();
}

void PhraseScorer::pqToList() {
    last.reset();
    first.reset();
    while (pq->top()) {
        PhrasePositionsPtr pp(pq->pop());
        if (last) { // add next to end of list
            last->_next = pp;
        } else {
            first = pp;
        }
        last = pp;
        pp->_next.reset();
    }
}

void PhraseScorer::firstToLast() {
    last->_next = first; // move first to end of list
    last = first;
    first = first->_next;
    last->_next.reset();
}

String PhraseScorer::toString() {
    return L"scorer(" + weight->toString() + L")";
}

}
