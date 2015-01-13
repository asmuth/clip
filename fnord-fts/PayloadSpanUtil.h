/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef PAYLOADSPANUTIL_H
#define PAYLOADSPANUTIL_H

#include "fnord-fts/util/LuceneObject.h"

namespace Lucene {

/// Experimental class to get set of payloads for most standard Lucene queries.  Operates like Highlighter -
/// IndexReader should only contain doc of interest, best to use MemoryIndex.
class PayloadSpanUtil : public LuceneObject {
public:
    /// @param reader That contains doc with payloads to extract
    PayloadSpanUtil(const IndexReaderPtr& reader);

    virtual ~PayloadSpanUtil();

    LUCENE_CLASS(PayloadSpanUtil);

protected:
    IndexReaderPtr reader;

public:
    /// Query should be rewritten for wild/fuzzy support.
    /// @return payloads Collection
    Collection<ByteArray> getPayloadsForQuery(const QueryPtr& query);

protected:
    void queryToSpanQuery(const QueryPtr& query, Collection<ByteArray> payloads);
    void getPayloads(Collection<ByteArray> payloads, const SpanQueryPtr& query);
};

}

#endif
