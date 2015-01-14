/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "fnord-fts/fts.h"
#include "fnord-fts/search/TermScorer.h"
#include "fnord-fts/index/TermDocs.h"
#include "fnord-fts/search/Similarity.h"
#include "fnord-fts/search/Weight.h"
#include "fnord-fts/search/Collector.h"

namespace Lucene {

const int32_t TermScorer::SCORE_CACHE_SIZE = 32;

TermScorer::TermScorer(const WeightPtr& weight, const TermDocsPtr& td, const SimilarityPtr& similarity, ByteArray norms) : Scorer(similarity) {
    this->weight = weight;
    this->termDocs = td;
    this->norms = norms;
    this->weightValue = weight->getValue();
    this->doc = -1;
    this->docs = Collection<int32_t>::newInstance(32);
    this->freqs = Collection<int32_t>::newInstance(32);
    this->pointer = 0;
    this->pointerMax = 0;
    this->scoreCache = Collection<double>::newInstance(SCORE_CACHE_SIZE);

    for (int32_t i = 0; i < SCORE_CACHE_SIZE; ++i) {
        scoreCache[i] = getSimilarity()->tf(i) * weightValue;
    }
}

TermScorer::~TermScorer() {
}

const Collection<double> TermScorer::SIM_NORM_DECODER() {
    return Similarity::getNormDecoder();
}

void TermScorer::score(const CollectorPtr& collector) {
    score(collector, INT_MAX, nextDoc());
}

bool TermScorer::score(const CollectorPtr& collector, int32_t max, int32_t firstDocID) {
    // firstDocID is ignored since nextDoc() sets 'doc'
    collector->setScorer(shared_from_this());
    while (doc < max) { // for docs in window
        collector->collect(doc);

        if (++pointer >= pointerMax) {
            pointerMax = termDocs->read(docs, freqs); // refill buffers
            if (pointerMax != 0) {
                pointer = 0;
            } else {
                termDocs->close(); // close stream
                doc = INT_MAX; // set to sentinel value
                return false;
            }
        }
        doc = docs[pointer];
    }
    return true;
}

int32_t TermScorer::docID() {
    return doc;
}

int32_t TermScorer::nextDoc() {
    ++pointer;
    if (pointer >= pointerMax) {
        pointerMax = termDocs->read(docs, freqs); // refill buffer
        if (pointerMax != 0) {
            pointer = 0;
        } else {
            termDocs->close(); // close stream
            doc = NO_MORE_DOCS;
            return doc;
        }
    }
    doc = docs[pointer];
    return doc;
}

double TermScorer::score() {
    BOOST_ASSERT(doc != -1);
    int32_t f = freqs[pointer];
    double raw = f < SCORE_CACHE_SIZE ? scoreCache[f] : getSimilarity()->tf(f) * weightValue; // compute tf(f) * weight
    return norms ? raw * SIM_NORM_DECODER()[norms[doc] & 0xff] : raw; // normalize for field
}

int32_t TermScorer::advance(int32_t target) {
    // first scan in cache
    for (++pointer; pointer < pointerMax; ++pointer) {
        if (docs[pointer] >= target) {
            doc = docs[pointer];
            return doc;
        }
    }

    // not found in cache, seek underlying stream
    bool result = termDocs->skipTo(target);
    if (result) {
        pointerMax = 1;
        pointer = 0;
        doc = termDocs->doc();
        docs[pointer] = doc;
        freqs[pointer] = termDocs->freq();
    } else {
        doc = NO_MORE_DOCS;
    }
    return doc;
}

String TermScorer::toString() {
    return L"scorer(" + weight->toString() + L")";
}

}
