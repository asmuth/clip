/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef CHARFILTER_H
#define CHARFILTER_H

#include "CharStream.h"

namespace Lucene {

/// Subclasses of CharFilter can be chained to filter CharStream.  They can be used as {@link Reader} with
/// additional offset correction. {@link Tokenizer}s will automatically use {@link #correctOffset} if a
/// CharFilter/CharStream subclass is used.
class CharFilter : public CharStream {
protected:
    CharFilter(const CharStreamPtr& in);
public:
    virtual ~CharFilter();

    LUCENE_CLASS(CharFilter);

protected:
    CharStreamPtr input;

protected:
    /// Subclass may want to override to correct the current offset.
    /// @param currentOff current offset
    /// @return corrected offset
    virtual int32_t correct(int32_t currentOff);

    /// Chains the corrected offset through the input CharFilter.
    virtual int32_t correctOffset(int32_t currentOff);

    virtual void close();
    virtual int32_t read(wchar_t* buffer, int32_t offset, int32_t length);
    virtual bool markSupported();
    virtual void mark(int32_t readAheadLimit);
    virtual void reset();
};

}

#endif
