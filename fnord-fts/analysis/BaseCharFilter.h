/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef BASECHARFILTER_H
#define BASECHARFILTER_H

#include "fnord-fts/analysis/CharFilter.h"

namespace fnord {
namespace fts {

/// Base utility class for implementing a {@link CharFilter}.  You subclass this, and then record mappings by
/// calling {@link #addOffCorrectMap}, and then invoke the correct method to correct an offset.
class BaseCharFilter : public CharFilter {
public:
    BaseCharFilter(const CharStreamPtr& in);
    virtual ~BaseCharFilter();

    LUCENE_CLASS(BaseCharFilter);

protected:
    IntArray offsets;
    IntArray diffs;
    int32_t size;

protected:
    /// Retrieve the corrected offset.
    virtual int32_t correct(int32_t currentOff);

    int32_t getLastCumulativeDiff();
    void addOffCorrectMap(int32_t off, int32_t cumulativeDiff);
};

}

}
#endif
