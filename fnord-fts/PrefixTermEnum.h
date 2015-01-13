/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef PREFIXTERMENUM_H
#define PREFIXTERMENUM_H

#include "FilteredTermEnum.h"

namespace Lucene {

/// Subclass of FilteredTermEnum for enumerating all terms that match the specified prefix filter term.
///
/// Term enumerations are always ordered by Term.compareTo().  Each term in the enumeration is greater than
/// all that precede it.
class PrefixTermEnum : public FilteredTermEnum {
public:
    PrefixTermEnum(const IndexReaderPtr& reader, const TermPtr& prefix);
    virtual ~PrefixTermEnum();

    LUCENE_CLASS(PrefixTermEnum);

protected:
    TermPtr prefix;
    bool _endEnum;

public:
    virtual double difference();

protected:
    virtual bool endEnum();
    virtual bool termCompare(const TermPtr& term);

    TermPtr getPrefixTerm();
};

}

#endif
