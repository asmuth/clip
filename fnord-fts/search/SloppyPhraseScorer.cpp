/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "fnord-fts/LuceneInc.h"
#include "fnord-fts/SloppyPhraseScorer.h"
#include "fnord-fts/PhrasePositions.h"
#include "fnord-fts/PhraseQueue.h"
#include "fnord-fts/Similarity.h"

namespace Lucene {

SloppyPhraseScorer::SloppyPhraseScorer(const WeightPtr& weight, Collection<TermPositionsPtr> tps, Collection<int32_t> offsets, const SimilarityPtr& similarity, int32_t slop, ByteArray norms) : PhraseScorer(weight, tps, offsets, similarity, norms) {
    this->slop = slop;
    this->checkedRepeats = false;
}

SloppyPhraseScorer::~SloppyPhraseScorer() {
}

double SloppyPhraseScorer::phraseFreq() {
    int32_t end = initPhrasePositions();

    double freq = 0.0;
    bool done = (end < 0);
    while (!done) {
        PhrasePositionsPtr pp(pq->pop());
        int32_t start = pp->position;
        int32_t next = pq->top()->position;

        bool tpsDiffer = true;
        for (int32_t pos = start; pos <= next || !tpsDiffer; pos = pp->position) {
            if (pos<=next && tpsDiffer) {
                start = pos;    // advance pp to min window
            }
            if (!pp->nextPosition()) {
                done = true; // ran out of a term - done
                break;
            }

            PhrasePositionsPtr pp2;
            tpsDiffer = (!pp->repeats || !(pp2 = termPositionsDiffer(pp)));
            if (pp2 && pp2 != pp) {
                pp = flip(pp, pp2);    // flip pp to pp2
            }
        }

        int32_t matchLength = end - start;
        if (matchLength <= slop) {
            freq += getSimilarity()->sloppyFreq(matchLength);    // score match
        }

        if (pp->position > end) {
            end = pp->position;
        }
        pq->add(pp); // restore pq
    }

    return freq;
}

PhrasePositionsPtr SloppyPhraseScorer::flip(const PhrasePositionsPtr& pp, const PhrasePositionsPtr& pp2) {
    int32_t n = 0;
    PhrasePositionsPtr pp3;
    // pop until finding pp2
    while ((pp3 = pq->pop()) != pp2) {
        tmpPos[n++] = pp3;
    }
    // insert back all but pp2
    for (n--; n >= 0; --n) {
        pq->addOverflow(tmpPos[n]);
    }
    // insert pp back
    pq->add(pp);
    return pp2;
}

int32_t SloppyPhraseScorer::initPhrasePositions() {
    int32_t end = 0;

    // no repeats at all (most common case is also the simplest one)
    if (checkedRepeats && !repeats) {
        // build queue from list
        pq->clear();
        for (PhrasePositionsPtr pp(first); pp; pp = pp->_next) {
            pp->firstPosition();
            if (pp->position > end) {
                end = pp->position;
            }
            pq->add(pp); // build pq from list
        }
        return end;
    }

    // position the pp's
    for (PhrasePositionsPtr pp(first); pp; pp = pp->_next) {
        pp->firstPosition();
    }

    // one time initialization for this scorer
    if (!checkedRepeats) {
        checkedRepeats = true;
        // check for repeats
        MapPhrasePositionsLuceneObject m;
        for (PhrasePositionsPtr pp(first); pp; pp = pp->_next) {
            int32_t tpPos = pp->position + pp->offset;
            for (PhrasePositionsPtr pp2(pp->_next); pp2; pp2 = pp2->_next) {
                int32_t tpPos2 = pp2->position + pp2->offset;
                if (tpPos2 == tpPos) {
                    if (!m) {
                        m = MapPhrasePositionsLuceneObject::newInstance();
                    }
                    pp->repeats = true;
                    pp2->repeats = true;
                    m.put(pp, LuceneObjectPtr());
                    m.put(pp2, LuceneObjectPtr());
                }
            }
        }
        if (m) {
            repeats = Collection<PhrasePositionsPtr>::newInstance();
            for (MapPhrasePositionsLuceneObject::iterator key = m.begin(); key != m.end(); ++key) {
                repeats.add(key->first);
            }
        }
    }

    // with repeats must advance some repeating pp's so they all start with differing tp's
    if (repeats) {
        for (Collection<PhrasePositionsPtr>::iterator pp = repeats.begin(); pp != repeats.end(); ++pp) {
            PhrasePositionsPtr pp2;
            while ((pp2 = termPositionsDiffer(*pp))) {
                if (!pp2->nextPosition()) { // out of pps that do not differ, advance the pp with higher offset
                    return -1;    // ran out of a term - done
                }
            }
        }
    }

    // build queue from list
    pq->clear();
    for (PhrasePositionsPtr pp(first); pp; pp = pp->_next) {
        if (pp->position > end) {
            end = pp->position;
        }
        pq->add(pp); // build pq from list
    }

    if (repeats) {
        tmpPos = Collection<PhrasePositionsPtr>::newInstance(pq->size());
    }

    return end;
}

PhrasePositionsPtr SloppyPhraseScorer::termPositionsDiffer(const PhrasePositionsPtr& pp) {
    // Efficiency note: a more efficient implementation could keep a map between repeating pp's, so that if
    // pp1a, pp1b, pp1c are repeats term1, and pp2a, pp2b are repeats of term2, pp2a would only be checked
    // against pp2b but not against pp1a, pp1b, pp1c.  However this would complicate code, for a rather rare
    // case, so choice is to compromise here.
    int32_t tpPos = pp->position + pp->offset;
    for (Collection<PhrasePositionsPtr>::iterator pp2 = repeats.begin(); pp2 != repeats.end(); ++pp2) {
        if (*pp2 == pp) {
            continue;
        }
        int32_t tpPos2 = (*pp2)->position + (*pp2)->offset;
        if (tpPos2 == tpPos) {
            return pp->offset > (*pp2)->offset ? pp : *pp2;    // do not differ: return the one with higher offset.
        }
    }
    return PhrasePositionsPtr();
}

}
