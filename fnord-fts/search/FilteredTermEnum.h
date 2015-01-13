/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef FILTEREDTERMENUM_H
#define FILTEREDTERMENUM_H

#include "fnord-fts/index/TermEnum.h"

namespace Lucene {

/// Abstract class for enumerating a subset of all terms.
///
/// Term enumerations are always ordered by Term.compareTo().  Each term in the enumeration is greater than
/// all that precede it.
class FilteredTermEnum : public TermEnum {
public:
    virtual ~FilteredTermEnum();
    LUCENE_CLASS(FilteredTermEnum);

protected:
    /// The current term
    TermPtr currentTerm;

    /// The delegate enum - to set this member use {@link #setEnum}
    TermEnumPtr actualEnum;

public:
    /// Equality measure on the term
    virtual double difference() = 0;

    /// Returns the docFreq of the current Term in the enumeration.
    /// Returns -1 if no Term matches or all terms have been enumerated.
    virtual int32_t docFreq();

    /// Increments the enumeration to the next element.  True if one exists.
    virtual bool next();

    /// Returns the current Term in the enumeration.
    /// Returns null if no Term matches or all terms have been enumerated.
    virtual TermPtr term();

    /// Closes the enumeration to further activity, freeing resources.
    virtual void close();

protected:
    /// Equality compare on the term
    virtual bool termCompare(const TermPtr& term) = 0;

    /// Indicates the end of the enumeration has been reached
    virtual bool endEnum() = 0;

    /// Use this method to set the actual TermEnum (eg. in ctor), it will be automatically positioned
    /// on the first matching term.
    virtual void setEnum(const TermEnumPtr& actualEnum);
};

}

#endif
