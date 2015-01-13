/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "NormalizeCharMap.h"

namespace Lucene {

NormalizeCharMap::NormalizeCharMap() {
    diff = 0;
}

NormalizeCharMap::~NormalizeCharMap() {
}

void NormalizeCharMap::add(const String& singleMatch, const String& replacement) {
    NormalizeCharMapPtr currMap(shared_from_this());
    for (String::const_iterator c = singleMatch.begin(); c != singleMatch.end(); ++c) {
        if (!currMap->submap) {
            currMap->submap = MapCharNormalizeCharMap::newInstance();
        }
        NormalizeCharMapPtr map(currMap->submap.get(*c));
        if (!map) {
            map = newLucene<NormalizeCharMap>();
            currMap->submap.put(*c, map);
        }
        currMap = map;
    }
    if (!currMap->normStr.empty()) {
        boost::throw_exception(RuntimeException(L"MappingCharFilter: there is already a mapping for " + singleMatch));
    }
    currMap->normStr = replacement;
    currMap->diff = (int32_t)(singleMatch.length() - replacement.length());
}

}
