/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef DOCIDSET_H
#define DOCIDSET_H

#include "fnord-fts/search/DocIdSetIterator.h"

namespace Lucene {

/// A DocIdSet contains a set of doc ids. Implementing classes must only implement {@link #iterator} to
/// provide access to the set.
class DocIdSet : public LuceneObject {
public:
    virtual ~DocIdSet();
    LUCENE_CLASS(DocIdSet);

public:
    /// Provides a {@link DocIdSetIterator} to access the set.  This implementation can return null or
    /// {@link #EmptyDocIdSet}.iterator() if there are no docs that match.
    virtual DocIdSetIteratorPtr iterator() = 0;

    /// This method is a hint for {@link CachingWrapperFilter}, if this DocIdSet should be cached without
    /// copying it into a BitSet.  The default is to return false.  If you have an own DocIdSet implementation
    /// that does its iteration very effective and fast without doing disk I/O, override this method and
    /// return true.
    virtual bool isCacheable();

    /// An empty {@code DocIdSet} instance for easy use, eg. in Filters that hit no documents.
    static DocIdSetPtr EMPTY_DOCIDSET();
};

}

#endif
