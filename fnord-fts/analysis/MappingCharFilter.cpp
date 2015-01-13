/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "MappingCharFilter.h"
#include "NormalizeCharMap.h"
#include "CharReader.h"

namespace Lucene {

MappingCharFilter::MappingCharFilter(const NormalizeCharMapPtr& normMap, const CharStreamPtr& in) : BaseCharFilter(in) {
    this->normMap = normMap;
    this->charPointer = 0;
    this->nextCharCounter = 0;
}

MappingCharFilter::MappingCharFilter(const NormalizeCharMapPtr& normMap, const ReaderPtr& in) : BaseCharFilter(CharReader::get(in)) {
    this->normMap = normMap;
    this->charPointer = 0;
    this->nextCharCounter = 0;
}

MappingCharFilter::~MappingCharFilter() {
}

int32_t MappingCharFilter::read() {
    while (true) {
        if (charPointer < (int32_t)replacement.length()) {
            return (int32_t)replacement[charPointer++];
        }

        int32_t firstChar = nextChar();
        if (firstChar == -1) {
            return -1;
        }
        NormalizeCharMapPtr nm(normMap->submap ? normMap->submap.get((wchar_t)firstChar) : NormalizeCharMapPtr());
        if (!nm) {
            return firstChar;
        }
        NormalizeCharMapPtr result(match(nm));
        if (!result) {
            return firstChar;
        }
        replacement = result->normStr;
        charPointer = 0;
        if (result->diff != 0) {
            int32_t prevCumulativeDiff = getLastCumulativeDiff();
            if (result->diff < 0) {
                for (int32_t i = 0; i < -result->diff; ++i) {
                    addOffCorrectMap(nextCharCounter + i - prevCumulativeDiff, prevCumulativeDiff - 1 - i);
                }
            } else {
                addOffCorrectMap(nextCharCounter - result->diff - prevCumulativeDiff, prevCumulativeDiff + result->diff);
            }
        }

    }
}

int32_t MappingCharFilter::nextChar() {
    ++nextCharCounter;
    if (buffer && !buffer.empty()) {
        return buffer.removeFirst();
    }
    return input->read();
}

void MappingCharFilter::pushChar(int32_t c) {
    --nextCharCounter;
    if (!buffer) {
        buffer = Collection<wchar_t>::newInstance();
    }
    buffer.add(0, (wchar_t)c);
}

void MappingCharFilter::pushLastChar(int32_t c) {
    if (!buffer) {
        buffer = Collection<wchar_t>::newInstance();
    }
    buffer.add((wchar_t)c);
}

NormalizeCharMapPtr MappingCharFilter::match(const NormalizeCharMapPtr& map) {
    NormalizeCharMapPtr result;
    if (map->submap) {
        int32_t chr = nextChar();
        if (chr != -1) {
            NormalizeCharMapPtr subMap(map->submap.get((wchar_t)chr));
            if (subMap) {
                result = match(subMap);
            }
            if (!result) {
                pushChar(chr);
            }
        }
    }
    if (!result) {
        result = map;
    }
    return result;
}

int32_t MappingCharFilter::read(wchar_t* buffer, int32_t offset, int32_t length) {
    CharArray tmp(CharArray::newInstance(length));
    int32_t l = input->read(tmp.get(), 0, length);
    if (l != -1) {
        for (int32_t i = 0; i < l; ++i) {
            pushLastChar(tmp[i]);
        }
    }
    l = 0;
    for (int32_t i = offset; i < offset + length; ++i) {
        int32_t c = read();
        if (c == -1) {
            break;
        }
        buffer[i] = (wchar_t)c;
        ++l;
    }
    return l == 0 ? -1 : l;
}

}
