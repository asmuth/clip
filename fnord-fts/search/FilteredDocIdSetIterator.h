/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef FILTEREDDOCIDSETITERATOR_H
#define FILTEREDDOCIDSETITERATOR_H

#include "fnord-fts/search/DocIdSetIterator.h"

namespace Lucene {

/// Abstract decorator class of a DocIdSetIterator implementation that provides on-demand filter/validation
/// mechanism on an underlying DocIdSetIterator.  See {@link FilteredDocIdSet}.
class FilteredDocIdSetIterator : public DocIdSetIterator {
public:
    /// @param innerIter Underlying DocIdSetIterator.
    FilteredDocIdSetIterator(const DocIdSetIteratorPtr& innerIter);
    virtual ~FilteredDocIdSetIterator();

    LUCENE_CLASS(FilteredDocIdSetIterator);

protected:
    DocIdSetIteratorPtr innerIter;
    int32_t doc;

public:
    virtual int32_t docID();
    virtual int32_t nextDoc();
    virtual int32_t advance(int32_t target);

protected:
    /// Validation method to determine whether a docid should be in the result set.
    /// @param doc docid to be tested
    /// @return true if input docid should be in the result set, false otherwise.
    /// @see #FilteredDocIdSetIterator(DocIdSetIterator).
    virtual bool match(int32_t docid) = 0;
};

}

#endif
