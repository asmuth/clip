/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "Random.h"
#include "MiscUtils.h"

namespace Lucene {

Random::Random() {
    this->seed = (int64_t)MiscUtils::currentTimeMillis();
}

Random::Random(int64_t seed) {
    this->seed = seed;
}

Random::~Random() {
}

void Random::setSeed(int64_t seed) {
    this->seed = (seed ^ 0x5deece66dLL) & (((int64_t)1 << 48) - 1);
}

int32_t Random::nextInt(int32_t limit) {
    if ((limit & -limit) == limit) {
        return (int32_t)((limit * (int64_t)next(31)) >> 31);
    }

    int32_t bits = 0;
    int32_t val = 0;

    do {
        bits = next(31);
        val = bits % limit;
    } while (bits - val + (limit - 1) < 0);

    return val;
}

double Random::nextDouble() {
    return ((double)(((int64_t)next(26) << 27) + next(27)) / (double)((int64_t)1 << 53));
}

int32_t Random::next(int32_t bits) {
    seed = (seed * 0x5deece66dLL + 0xb) & (((int64_t)1 << 48) - 1);
    return (int32_t)(seed >> (48 - bits));
}

}
