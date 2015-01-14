/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef LENGTHFILTER_H
#define LENGTHFILTER_H

#include "fnord-fts/analysis/TokenFilter.h"

namespace fnord {
namespace fts {

/// Removes words that are too long or too short from the stream.
class LengthFilter : public TokenFilter {
public:
    /// Build a filter that removes words that are too long or too short from the text.
    LengthFilter(const TokenStreamPtr& input, int32_t min, int32_t max);
    virtual ~LengthFilter();

    LUCENE_CLASS(LengthFilter);

public:
    int32_t min;
    int32_t max;

protected:
    TermAttributePtr termAtt;

public:
    /// Returns the next input Token whose term() is the right len
    virtual bool incrementToken();
};

}

}
#endif
