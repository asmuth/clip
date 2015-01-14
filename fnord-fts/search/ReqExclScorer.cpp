/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "fnord-fts/fts.h"
#include "fnord-fts/search/ReqExclScorer.h"

namespace Lucene {

ReqExclScorer::ReqExclScorer(const ScorerPtr& reqScorer, const DocIdSetIteratorPtr& exclDisi) : Scorer(SimilarityPtr()) { // No similarity used.
    this->reqScorer = reqScorer;
    this->exclDisi = exclDisi;
    this->doc = -1;
}

ReqExclScorer::~ReqExclScorer() {
}

int32_t ReqExclScorer::nextDoc() {
    if (!reqScorer) {
        return doc;
    }
    doc = reqScorer->nextDoc();
    if (doc == NO_MORE_DOCS) {
        reqScorer.reset(); // exhausted, nothing left
        return doc;
    }
    if (!exclDisi) {
        return doc;
    }
    doc = toNonExcluded();
    return doc;
}

int32_t ReqExclScorer::toNonExcluded() {
    int32_t exclDoc = exclDisi->docID();
    int32_t reqDoc = reqScorer->docID(); // may be excluded
    do {
        if (reqDoc < exclDoc) {
            return reqDoc;    // reqScorer advanced to before exclScorer, ie. not excluded
        } else if (reqDoc > exclDoc) {
            exclDoc = exclDisi->advance(reqDoc);
            if (exclDoc == NO_MORE_DOCS) {
                exclDisi.reset(); // exhausted, no more exclusions
                return reqDoc;
            }
            if (exclDoc > reqDoc) {
                return reqDoc;    // not excluded
            }
        }
    } while ((reqDoc = reqScorer->nextDoc()) != NO_MORE_DOCS);
    reqScorer.reset(); // exhausted, nothing left
    return NO_MORE_DOCS;
}

int32_t ReqExclScorer::docID() {
    return doc;
}

double ReqExclScorer::score() {
    return reqScorer->score(); // reqScorer may be null when next() or skipTo() already return false
}

int32_t ReqExclScorer::advance(int32_t target) {
    if (!reqScorer) {
        doc = NO_MORE_DOCS;
        return doc;
    }
    if (!exclDisi) {
        doc = reqScorer->advance(target);
        return doc;
    }
    if (reqScorer->advance(target) == NO_MORE_DOCS) {
        reqScorer.reset();
        doc = NO_MORE_DOCS;
        return doc;
    }
    doc = toNonExcluded();
    return doc;
}

}
