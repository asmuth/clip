/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef OPENBITSETDISI_H
#define OPENBITSETDISI_H

#include "OpenBitSet.h"

namespace Lucene {

class OpenBitSetDISI : public OpenBitSet {
public:
    /// Construct an OpenBitSetDISI with its bits set from the doc ids of the given DocIdSetIterator.
    /// Also give a maximum size one larger than the largest doc id for which a bit may ever be set on
    /// this OpenBitSetDISI.
    OpenBitSetDISI(const DocIdSetIteratorPtr& disi, int32_t maxSize);

    /// Construct an OpenBitSetDISI with no bits set, and a given maximum size one larger than the largest
    /// doc id for which a bit may ever be set on this OpenBitSetDISI.
    OpenBitSetDISI(int32_t maxSize);

    virtual ~OpenBitSetDISI();

    LUCENE_CLASS(OpenBitSetDISI);

public:
    /// Perform an in-place OR with the doc ids from a given DocIdSetIterator, setting the bit for each
    /// such doc id.  These doc ids should be smaller than the maximum size passed to the constructor.
    void inPlaceOr(const DocIdSetIteratorPtr& disi);

    /// Perform an in-place AND with the doc ids from a given DocIdSetIterator, leaving only the bits set
    /// for which the doc ids are in common.  These doc ids should be smaller than the maximum size passed
    /// to the constructor.
    void inPlaceAnd(const DocIdSetIteratorPtr& disi);

    /// Perform an in-place NOT with the doc ids from a given DocIdSetIterator, clearing all the bits for
    /// each such doc id.  These doc ids should be smaller than the maximum size passed to the constructor.
    void inPlaceNot(const DocIdSetIteratorPtr& disi);

    /// Perform an inplace XOR with the doc ids from a given DocIdSetIterator, flipping all the bits for
    /// each such doc id.  These doc ids should be smaller than the maximum size passed to the constructor.
    void inPlaceXor(const DocIdSetIteratorPtr& disi);
};

}

#endif
