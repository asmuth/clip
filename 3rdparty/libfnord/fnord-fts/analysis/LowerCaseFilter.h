/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef LOWERCASEFILTER_H
#define LOWERCASEFILTER_H

#include "fnord-fts/analysis/TokenFilter.h"

namespace fnord {
namespace fts {

/// Normalizes token text to lower case.
class LowerCaseFilter : public TokenFilter {
public:
    LowerCaseFilter(const TokenStreamPtr& input);
    virtual ~LowerCaseFilter();

    LUCENE_CLASS(LowerCaseFilter);

protected:
    TermAttributePtr termAtt;

public:
    virtual bool incrementToken();
};

}

}
#endif
