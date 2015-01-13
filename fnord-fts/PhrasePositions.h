/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef PHRASEPOSITIONS_H
#define PHRASEPOSITIONS_H

#include "LuceneObject.h"

namespace Lucene {

/// Position of a term in a document that takes into account the term offset within the phrase.
class PhrasePositions : public LuceneObject {
public:
    PhrasePositions(const TermPositionsPtr& t, int32_t o);
    virtual ~PhrasePositions();

    LUCENE_CLASS(PhrasePositions);

public:
    int32_t doc; // current doc
    int32_t position; // position in doc
    int32_t count; // remaining pos in this doc
    int32_t offset; // position in phrase
    TermPositionsPtr tp; // stream of positions
    PhrasePositionsPtr _next; // used to make lists
    bool repeats; // there's other pp for same term (eg. query="1st word 2nd word"~1)

public:
    bool next();
    bool skipTo(int32_t target);
    void firstPosition();
    bool nextPosition();
};

}

#endif
