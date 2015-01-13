/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef SCOREDOC_H
#define SCOREDOC_H

#include "fnord-fts/util/LuceneObject.h"

namespace Lucene {

/// Expert: Returned by low-level search implementations.
/// @see TopDocs
class ScoreDoc : public LuceneObject {
public:
    ScoreDoc(int32_t doc, double score);
    virtual ~ScoreDoc();

    LUCENE_CLASS(ScoreDoc);

public:
    /// The score of this document for the query.
    double score;

    /// A hit document's number.
    /// @see Searcher#doc(int32_t)
    int32_t doc;

public:
    virtual String toString();
};

}

#endif
