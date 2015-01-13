/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef MAPPINGCHARFILTER_H
#define MAPPINGCHARFILTER_H

#include "fnord-fts/analysis/BaseCharFilter.h"

namespace Lucene {

/// Simplistic {@link CharFilter} that applies the mappings contained in a {@link NormalizeCharMap} to the character
/// stream, and correcting the resulting changes to the offsets.
class MappingCharFilter : public BaseCharFilter {
public:
    /// Default constructor that takes a {@link CharStream}.
    MappingCharFilter(const NormalizeCharMapPtr& normMap, const CharStreamPtr& in);

    /// Easy-use constructor that takes a {@link Reader}.
    MappingCharFilter(const NormalizeCharMapPtr& normMap, const ReaderPtr& in);

    virtual ~MappingCharFilter();

    LUCENE_CLASS(MappingCharFilter);

protected:
    NormalizeCharMapPtr normMap;
    Collection<wchar_t> buffer;
    String replacement;
    int32_t charPointer;
    int32_t nextCharCounter;

public:
    virtual int32_t read();
    virtual int32_t read(wchar_t* buffer, int32_t offset, int32_t length);

protected:
    int32_t nextChar();
    void pushChar(int32_t c);
    void pushLastChar(int32_t c);
    NormalizeCharMapPtr match(const NormalizeCharMapPtr& map);
};

}

#endif
