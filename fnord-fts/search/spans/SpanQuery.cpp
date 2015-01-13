/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "fnord-fts/LuceneInc.h"
#include "SpanQuery.h"
#include "fnord-fts/search/spans/SpanWeight.h"

namespace Lucene {

SpanQuery::~SpanQuery() {
}

WeightPtr SpanQuery::createWeight(const SearcherPtr& searcher) {
    return newLucene<SpanWeight>(shared_from_this(), searcher);
}

}
