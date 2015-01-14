/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "fnord-fts/fts.h"
#include "fnord-fts/search/SpanQueryFilter.h"
#include "fnord-fts/search/spans/SpanQuery.h"
#include "fnord-fts/search/SpanFilterResult.h"
#include "fnord-fts/search/spans/Spans.h"
#include "fnord-fts/util/OpenBitSet.h"
#include "fnord-fts/index/IndexReader.h"

namespace Lucene {

SpanQueryFilter::SpanQueryFilter(const SpanQueryPtr& query) {
    this->query = query;
}

SpanQueryFilter::~SpanQueryFilter() {
}

DocIdSetPtr SpanQueryFilter::getDocIdSet(const IndexReaderPtr& reader) {
    SpanFilterResultPtr result(bitSpans(reader));
    return result->getDocIdSet();
}

SpanFilterResultPtr SpanQueryFilter::bitSpans(const IndexReaderPtr& reader) {
    OpenBitSetPtr bits(newLucene<OpenBitSet>(reader->maxDoc()));
    SpansPtr spans(query->getSpans(reader));
    Collection<PositionInfoPtr> tmp(Collection<PositionInfoPtr>::newInstance());
    int32_t currentDoc = -1;
    PositionInfoPtr currentInfo;
    while (spans->next()) {
        int32_t doc = spans->doc();
        bits->set(doc);
        if (currentDoc != doc) {
            currentInfo = newLucene<PositionInfo>(doc);
            tmp.add(currentInfo);
            currentDoc = doc;
        }
        currentInfo->addPosition(spans->start(), spans->end());
    }
    return newLucene<SpanFilterResult>(bits, tmp);
}

SpanQueryPtr SpanQueryFilter::getQuery() {
    return query;
}

String SpanQueryFilter::toString() {
    return L"SpanQueryFilter(" + query->toString() + L")";
}

bool SpanQueryFilter::equals(const LuceneObjectPtr& other) {
    if (LuceneObject::equals(other)) {
        return true;
    }

    SpanQueryFilterPtr otherSpanQueryFilter(std::dynamic_pointer_cast<SpanQueryFilter>(other));
    if (!otherSpanQueryFilter) {
        return false;
    }
    return query->equals(otherSpanQueryFilter->query);
}

int32_t SpanQueryFilter::hashCode() {
    return query->hashCode() ^ 0x923f64b9;
}

}
