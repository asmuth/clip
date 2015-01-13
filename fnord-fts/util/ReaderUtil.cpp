/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "ReaderUtil.h"
#include "IndexReader.h"

namespace Lucene {

ReaderUtil::~ReaderUtil() {
}

void ReaderUtil::gatherSubReaders(Collection<IndexReaderPtr> allSubReaders, const IndexReaderPtr& reader) {
    Collection<IndexReaderPtr> subReaders(reader->getSequentialSubReaders());
    if (!subReaders) {
        // Add the reader itself, and do not recurse
        allSubReaders.add(reader);
    } else {
        for (Collection<IndexReaderPtr>::iterator subReader = subReaders.begin(); subReader != subReaders.end(); ++subReader) {
            gatherSubReaders(allSubReaders, *subReader);
        }
    }
}

IndexReaderPtr ReaderUtil::subReader(int32_t doc, const IndexReaderPtr& reader) {
    Collection<IndexReaderPtr> subReaders(Collection<IndexReaderPtr>::newInstance());
    ReaderUtil::gatherSubReaders(subReaders, reader);
    Collection<int32_t> docStarts(Collection<int32_t>::newInstance(subReaders.size()));
    int32_t maxDoc = 0;
    for (int32_t i = 0; i < subReaders.size(); ++i) {
        docStarts[i] = maxDoc;
        maxDoc += subReaders[i]->maxDoc();
    }
    return subReaders[ReaderUtil::subIndex(doc, docStarts)];
}

IndexReaderPtr ReaderUtil::subReader(const IndexReaderPtr& reader, int32_t subIndex) {
    Collection<IndexReaderPtr> subReaders(Collection<IndexReaderPtr>::newInstance());
    ReaderUtil::gatherSubReaders(subReaders, reader);
    return subReaders[subIndex];
}

int32_t ReaderUtil::subIndex(int32_t n, Collection<int32_t> docStarts) {
    // Binary search to locate reader
    Collection<int32_t>::iterator index = std::upper_bound(docStarts.begin(), docStarts.end(), n);
    return (std::distance(docStarts.begin(), index) - 1);
}

}
