/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef _SCORERDOCQUEUE_H
#define _SCORERDOCQUEUE_H

#include "LuceneObject.h"

namespace Lucene {

class HeapedScorerDoc : public LuceneObject {
public:
    HeapedScorerDoc(const ScorerPtr& scorer);
    HeapedScorerDoc(const ScorerPtr& scorer, int32_t doc);
    virtual ~HeapedScorerDoc();

    LUCENE_CLASS(HeapedScorerDoc);

public:
    ScorerPtr scorer;
    int32_t doc;

public:
    void adjust();
};

}

#endif
