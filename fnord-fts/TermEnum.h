/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef TERMENUM_H
#define TERMENUM_H

#include "LuceneObject.h"

namespace Lucene {

/// Abstract class for enumerating terms.
///
/// Term enumerations are always ordered by Term.compareTo().  Each term in the enumeration is greater
/// than all that precede it.
class TermEnum : public LuceneObject {
public:
    virtual ~TermEnum();
    LUCENE_CLASS(TermEnum);

public:
    /// Increments the enumeration to the next element.  True if one exists.
    virtual bool next() = 0;

    /// Returns the current Term in the enumeration.
    virtual TermPtr term() = 0;

    /// Returns the docFreq of the current Term in the enumeration.
    virtual int32_t docFreq() = 0;

    /// Closes the enumeration to further activity, freeing resources.
    virtual void close() = 0;
};

}

#endif
