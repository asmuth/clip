/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef STANDARDFILTER_H
#define STANDARDFILTER_H

#include "TokenFilter.h"

namespace Lucene {

/// Normalizes tokens extracted with {@link StandardTokenizer}.
class StandardFilter : public TokenFilter {
public:
    /// Construct filtering input.
    StandardFilter(const TokenStreamPtr& input);
    virtual ~StandardFilter();

    LUCENE_CLASS(StandardFilter);

protected:
    TypeAttributePtr typeAtt;
    TermAttributePtr termAtt;

protected:
    static const String& APOSTROPHE_TYPE();
    static const String& ACRONYM_TYPE();

public:
    /// Returns the next token in the stream, or null at EOS.
    ///
    /// Removes <tt>'s</tt> from the end of words.
    /// Removes dots from acronyms.
    virtual bool incrementToken();
};

}

#endif
