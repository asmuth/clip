/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef NORMALIZECHARMAP_H
#define NORMALIZECHARMAP_H

#include "fnord-fts/util/LuceneObject.h"

namespace fnord {
namespace fts {

/// Holds a map of String input to String output, to be used with {@link MappingCharFilter}.
class NormalizeCharMap : public LuceneObject {
public:
    NormalizeCharMap();
    virtual ~NormalizeCharMap();

    LUCENE_CLASS(NormalizeCharMap);

public:
    MapCharNormalizeCharMap submap;
    String normStr;
    int32_t diff;

public:
    /// Records a replacement to be applied to the inputs stream.  Whenever singleMatch occurs in the input, it
    /// will be replaced with replacement.
    ///
    /// @param singleMatch input String to be replaced
    /// @param replacement output String
    void add(const String& singleMatch, const String& replacement);
};

}

}
#endif
