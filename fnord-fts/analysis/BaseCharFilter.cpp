/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "BaseCharFilter.h"
#include "MiscUtils.h"

namespace Lucene {

BaseCharFilter::BaseCharFilter(const CharStreamPtr& in) : CharFilter(in) {
    size = 0;
}

BaseCharFilter::~BaseCharFilter() {
}

int32_t BaseCharFilter::correct(int32_t currentOff) {
    if (!offsets || currentOff < offsets[0]) {
        return currentOff;
    }

    int32_t hi = size - 1;
    if (currentOff >= offsets[hi]) {
        return currentOff + diffs[hi];
    }

    int32_t lo = 0;
    int32_t mid = -1;

    while (hi >= lo) {
        mid = MiscUtils::unsignedShift(lo + hi, 1);
        if (currentOff < offsets[mid]) {
            hi = mid - 1;
        } else if (currentOff > offsets[mid]) {
            lo = mid + 1;
        } else {
            return currentOff + diffs[mid];
        }
    }

    if (currentOff < offsets[mid]) {
        return mid == 0 ? currentOff : currentOff + diffs[mid - 1];
    } else {
        return currentOff + diffs[mid];
    }
}

int32_t BaseCharFilter::getLastCumulativeDiff() {
    return !offsets ? 0 : diffs[size - 1];
}

void BaseCharFilter::addOffCorrectMap(int32_t off, int32_t cumulativeDiff) {
    if (!offsets) {
        offsets = IntArray::newInstance(64);
        diffs = IntArray::newInstance(64);
    } else if (size == offsets.size()) {
        offsets.resize(MiscUtils::getNextSize(offsets.size()));
        diffs.resize(MiscUtils::getNextSize(diffs.size()));
    }

    offsets[size] = off;
    diffs[size++] = cumulativeDiff;
}

}
