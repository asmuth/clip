/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef FILTER_H
#define FILTER_H

#include "LuceneObject.h"

namespace Lucene {

/// Abstract base class for restricting which documents may be returned during searching.
class Filter : public LuceneObject {
public:
    virtual ~Filter();
    LUCENE_CLASS(Filter);

public:
    /// Creates a {@link DocIdSet} enumerating the documents that should be permitted in search results.
    ///
    /// Note: null can be returned if no documents are accepted by this Filter.
    ///
    /// Note: This method will be called once per segment in the index during searching.  The returned
    /// {@link DocIdSet} must refer to document IDs for that segment, not for the top-level reader.
    ///
    /// @param reader a {@link IndexReader} instance opened on the index currently searched on. Note,
    /// it is likely that the provided reader does not represent the whole underlying index ie. if the
    /// index has more than one segment the given reader only represents a single segment.
    /// @return a DocIdSet that provides the documents which should be permitted or prohibited in search
    /// results. NOTE: null can be returned if no documents will be accepted by this Filter.
    ///
    /// @see DocIdBitSet
    virtual DocIdSetPtr getDocIdSet(const IndexReaderPtr& reader) = 0;
};

}

#endif
