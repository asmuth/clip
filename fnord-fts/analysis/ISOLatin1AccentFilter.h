/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef ISOLATIN1ACCENTFILTER_H
#define ISOLATIN1ACCENTFILTER_H

#include "fnord-fts/analysis/TokenFilter.h"

namespace fnord {
namespace fts {

/// A filter that replaces accented characters in the ISO Latin 1 character set (ISO-8859-1) by their unaccented
/// equivalent. The case will not be altered.
///
/// For instance, '&agrave;' will be replaced by 'a'.
///
/// @deprecated If you build a new index, use {@link ASCIIFoldingFilter} which covers a superset of Latin 1.
/// This class is included for use with existing indexes and will be removed in a future release (possibly Lucene 4.0).
class ISOLatin1AccentFilter : public TokenFilter {
public:
    ISOLatin1AccentFilter(const TokenStreamPtr& input);
    virtual ~ISOLatin1AccentFilter();

    LUCENE_CLASS(ISOLatin1AccentFilter);

protected:
    CharArray output;
    int32_t outputPos;
    TermAttributePtr termAtt;

public:
    virtual bool incrementToken();

    /// To replace accented characters in a String by unaccented equivalents.
    void removeAccents(const wchar_t* input, int32_t length);
};

}

}
#endif
