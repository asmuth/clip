/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef SINGLETERMENUM_H
#define SINGLETERMENUM_H

#include "fnord-fts/search/FilteredTermEnum.h"

namespace Lucene {

/// Subclass of FilteredTermEnum for enumerating a single term.
///
/// This can be used by {@link MultiTermQuery}s that need only visit one term, but want to preserve
/// MultiTermQuery semantics such as {@link MultiTermQuery#rewriteMethod}.
class SingleTermEnum : public FilteredTermEnum {
public:
    SingleTermEnum(const IndexReaderPtr& reader, const TermPtr& singleTerm);
    virtual ~SingleTermEnum();

    LUCENE_CLASS(SingleTermEnum);

protected:
    TermPtr singleTerm;
    bool _endEnum;

public:
    virtual double difference();

protected:
    virtual bool endEnum();
    virtual bool termCompare(const TermPtr& term);
};

}

#endif
