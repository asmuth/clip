/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "fnord-fts/LuceneInc.h"
#include "fnord-fts/PhraseQueue.h"
#include "fnord-fts/PhrasePositions.h"

namespace Lucene {

PhraseQueue::PhraseQueue(int32_t size) : PriorityQueue<PhrasePositionsPtr>(size) {
}

PhraseQueue::~PhraseQueue() {
}

bool PhraseQueue::lessThan(const PhrasePositionsPtr& first, const PhrasePositionsPtr& second) {
    if (first->doc == second->doc) {
        if (first->position == second->position) {
            // same doc and pp.position, so decide by actual term positions.
            // rely on: pp.position == tp.position - offset.
            return first->offset < second->offset;
        } else {
            return first->position < second->position;
        }
    } else {
        return first->doc < second->doc;
    }
}

}
