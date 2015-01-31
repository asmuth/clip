/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef PREFIXFILTER_H
#define PREFIXFILTER_H

#include "fnord-fts/search/MultiTermQueryWrapperFilter.h"

namespace fnord {
namespace fts {

/// A Filter that restricts search results to values that have a matching prefix in a given field.
class PrefixFilter : public MultiTermQueryWrapperFilter {
public:
    PrefixFilter(const TermPtr& prefix);
    virtual ~PrefixFilter();

    LUCENE_CLASS(PrefixFilter);

public:
    TermPtr getPrefix();

    virtual String toString();
};

}

}
#endif
