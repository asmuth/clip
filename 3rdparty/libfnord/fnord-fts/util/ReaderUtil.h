/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef READERUTIL_H
#define READERUTIL_H

#include "fnord-fts/util/LuceneObject.h"

namespace fnord {
namespace fts {

/// Common util methods for dealing with {@link IndexReader}s.
class ReaderUtil : public LuceneObject {
public:
    virtual ~ReaderUtil();
    LUCENE_CLASS(ReaderUtil);

public:
    /// Gathers sub-readers from reader into a List.
    static void gatherSubReaders(Collection<IndexReaderPtr> allSubReaders, const IndexReaderPtr& reader);

    /// Returns sub IndexReader that contains the given document id.
    ///
    /// @param doc Id of document
    /// @param reader Parent reader
    /// @return Sub reader of parent which contains the specified doc id
    static IndexReaderPtr subReader(int32_t doc, const IndexReaderPtr& reader);

    /// Returns sub-reader subIndex from reader.
    ///
    /// @param reader Parent reader
    /// @param subIndex Index of desired sub reader
    /// @return The subreader at subIndex
    static IndexReaderPtr subReader(const IndexReaderPtr& reader, int32_t subIndex);

    /// Returns index of the searcher/reader for document n in the array used to construct this
    /// searcher/reader.
    static int32_t subIndex(int32_t n, Collection<int32_t> docStarts);
};

}

}
#endif
