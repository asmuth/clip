/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "SmallDouble.h"
#include "MiscUtils.h"

namespace Lucene {

SmallDouble::~SmallDouble() {
}

uint8_t SmallDouble::doubleToByte(double f) {
    if (f < 0.0) { // round negatives up to zero
        f = 0.0;
    }
    if (f == 0.0) { // zero is a special case
        return 0;
    }

    int32_t bits = MiscUtils::doubleToIntBits(f);
    int32_t mantissa = (bits & 0xffffff) >> 21;
    int32_t exponent = (((bits >> 24) & 0x7f) - 63) + 15;

    if (exponent > 31) { // overflow: use max value
        exponent = 31;
        mantissa = 7;
    }
    if (exponent < 0) { // underflow: use min value
        exponent = 0;
        mantissa = 1;
    }

    return (uint8_t)((exponent << 3) | mantissa); // pack into a uint8_t
}

double SmallDouble::byteToDouble(uint8_t b) {
    if (b == 0) { // zero is a special case
        return 0.0;
    }
    int32_t mantissa = b & 7;
    int32_t exponent = (b >> 3) & 31;
    int32_t bits = ((exponent + (63 - 15)) << 24) | (mantissa << 21);
    return MiscUtils::intBitsToDouble(bits);
}

}
