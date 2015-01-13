/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "fnord-fts/LuceneInc.h"
#include "fnord-fts/search/TopFieldDocs.h"

namespace Lucene {

TopFieldDocs::TopFieldDocs(int32_t totalHits, Collection<ScoreDocPtr> scoreDocs, Collection<SortFieldPtr> fields, double maxScore) : TopDocs(totalHits, scoreDocs, maxScore) {
    this->fields = fields;
}

TopFieldDocs::~TopFieldDocs() {
}

}
