/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef CACHINGTOKENFILTER_H
#define CACHINGTOKENFILTER_H

#include "fnord-fts/analysis/TokenFilter.h"

namespace fnord {
namespace fts {

/// This class can be used if the token attributes of a TokenStream are intended to be consumed more than once.
/// It caches all token attribute states locally in a List.
///
/// CachingTokenFilter implements the optional method {@link TokenStream#reset()}, which repositions the stream
/// to the first Token.
class CachingTokenFilter : public TokenFilter {
public:
    CachingTokenFilter(const TokenStreamPtr& input);
    virtual ~CachingTokenFilter();

    LUCENE_CLASS(CachingTokenFilter);

protected:
    Collection<AttributeSourceStatePtr> cache;
    Collection<AttributeSourceStatePtr>::iterator iterator;
    AttributeSourceStatePtr finalState;

public:
    virtual bool incrementToken();
    virtual void end();
    virtual void reset();

protected:
    void fillCache();
};

}

}
#endif
