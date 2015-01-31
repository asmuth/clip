/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef TOPFIELDDOCS_H
#define TOPFIELDDOCS_H

#include "fnord-fts/search/TopDocs.h"

namespace fnord {
namespace fts {

/// Represents hits returned by {@link Searcher#search(QueryPtr, FilterPtr, int32_t, SortPtr)}.
class TopFieldDocs : public TopDocs {
public:
    /// @param totalHits Total number of hits for the query.
    /// @param scoreDocs The top hits for the query.
    /// @param fields The sort criteria used to find the top hits.
    /// @param maxScore The maximum score encountered.
    TopFieldDocs(int32_t totalHits, Collection<ScoreDocPtr> scoreDocs, Collection<SortFieldPtr> fields, double maxScore);

    virtual ~TopFieldDocs();

    LUCENE_CLASS(TopFieldDocs);

public:
    /// The fields which were used to sort results by.
    Collection<SortFieldPtr> fields;
};

}

}
#endif
