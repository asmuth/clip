/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "fnord-fts/LuceneInc.h"
#include "fnord-fts/search/PhrasePositions.h"
#include "fnord-fts/search/TermPositions.h"

namespace Lucene {

PhrasePositions::PhrasePositions(const TermPositionsPtr& t, int32_t o) {
    doc = 0;
    position = 0;
    count = 0;
    repeats = false;

    tp = t;
    offset = o;
}

PhrasePositions::~PhrasePositions() {
}

bool PhrasePositions::next() {
    if (!tp->next()) {
        tp->close(); // close stream
        doc = INT_MAX; // sentinel value
        return false;
    }
    doc = tp->doc();
    position = 0;
    return true;
}

bool PhrasePositions::skipTo(int32_t target) {
    if (!tp->skipTo(target)) {
        tp->close(); // close stream
        doc = INT_MAX; // sentinel value
        return false;
    }
    doc = tp->doc();
    position = 0;
    return true;
}

void PhrasePositions::firstPosition() {
    count = tp->freq(); // read first pos
    nextPosition();
}

bool PhrasePositions::nextPosition() {
    if (count-- > 0) { // read subsequent pos's
        position = tp->nextPosition() - offset;
        return true;
    } else {
        return false;
    }
}

}
