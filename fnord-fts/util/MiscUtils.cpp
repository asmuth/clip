/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "MiscUtils.h"
#include "LuceneObject.h"

namespace Lucene {

const uint32_t MiscUtils::SINGLE_EXPONENT_MASK = 0x7f800000;
const uint32_t MiscUtils::SINGLE_MANTISSA_MASK = 0x007fffff;
const uint32_t MiscUtils::SINGLE_NAN_BITS = (MiscUtils::SINGLE_EXPONENT_MASK | 0x00400000);

const uint64_t MiscUtils::DOUBLE_SIGN_MASK = 0x8000000000000000LL;
const uint64_t MiscUtils::DOUBLE_EXPONENT_MASK = 0x7ff0000000000000LL;
const uint64_t MiscUtils::DOUBLE_MANTISSA_MASK = 0x000fffffffffffffLL;
const uint64_t MiscUtils::DOUBLE_NAN_BITS = DOUBLE_EXPONENT_MASK | 0x0008000000000000LL;

uint64_t MiscUtils::getTimeMillis(boost::posix_time::ptime time) {
    return boost::posix_time::time_duration(time - boost::posix_time::ptime(boost::gregorian::date(1970, 1, 1))).total_milliseconds();
}

uint64_t MiscUtils::currentTimeMillis() {
    return getTimeMillis(boost::posix_time::microsec_clock::universal_time());
}

int32_t MiscUtils::getNextSize(int32_t targetSize) {
    return (targetSize >> 3) + (targetSize < 9 ? 3 : 6) + targetSize;
}

int32_t MiscUtils::getShrinkSize(int32_t currentSize, int32_t targetSize) {
    int32_t newSize = getNextSize(targetSize);
    return (newSize < currentSize / 2) ? newSize : currentSize;
}

int32_t MiscUtils::bytesDifference(uint8_t* bytes1, int32_t len1, uint8_t* bytes2, int32_t len2) {
    int32_t len = std::min(len1, len2);
    for (int32_t i = 0; i < len; ++i) {
        if (bytes1[i] != bytes2[i]) {
            return i;
        }
    }
    return len;
}

int32_t MiscUtils::hashCode(const wchar_t* array, int32_t start, int32_t end) {
    return hashCode(array + start, array + end, hashNumeric<wchar_t>);
}

int32_t MiscUtils::hashCode(const uint8_t* array, int32_t start, int32_t end) {
    return hashCode(array + start, array + end, hashNumeric<uint8_t>);
}

int32_t MiscUtils::hashCode(bool value) {
    return value ? 1231 : 1237;
}

int32_t MiscUtils::doubleToIntBits(double value) {
    int32_t intValue = 0;
    float floatValue = (float)value;
    std::memcpy(&intValue, &floatValue, sizeof(float));

    if ((intValue & SINGLE_EXPONENT_MASK) == SINGLE_EXPONENT_MASK) {
        if (intValue & SINGLE_MANTISSA_MASK) {
            return SINGLE_NAN_BITS;
        }
    }

    return intValue;
}

int32_t MiscUtils::doubleToRawIntBits(double value) {
    int32_t intValue = 0;
    float floatValue = (float)value;
    std::memcpy(&intValue, &floatValue, sizeof(float));
    return intValue;
}

double MiscUtils::intBitsToDouble(int32_t bits) {
    float floatValue = 0;
    std::memcpy(&floatValue, &bits, sizeof(int32_t));
    return (double)floatValue;
}

int64_t MiscUtils::doubleToLongBits(double value) {
    int64_t longValue = 0;
    std::memcpy(&longValue, &value, sizeof(double));

    if ((longValue & DOUBLE_EXPONENT_MASK) == DOUBLE_EXPONENT_MASK) {
        if (longValue & DOUBLE_MANTISSA_MASK) {
            return DOUBLE_NAN_BITS;
        }
    }

    return longValue;
}

int64_t MiscUtils::doubleToRawLongBits(double value) {
    int64_t longValue = 0;
    std::memcpy(&longValue, &value, sizeof(double));
    return longValue;
}

double MiscUtils::longBitsToDouble(int64_t bits) {
    double doubleValue = 0;
    std::memcpy(&doubleValue, &bits, sizeof(int64_t));
    return doubleValue;
}

bool MiscUtils::isInfinite(double value) {
    return (value == std::numeric_limits<double>::infinity() || value == -std::numeric_limits<double>::infinity());
}

bool MiscUtils::isNaN(double value) {
    return (value != value);
}

bool MiscUtils::equalTypes(const LuceneObjectPtr& first, const LuceneObjectPtr& second) {
    return (typeid(*first) == typeid(*second));
}

int64_t MiscUtils::unsignedShift(int64_t num, int64_t shift) {
    return (shift & 0x3f) == 0 ? num : (((uint64_t)num >> 1) & 0x7fffffffffffffffLL) >> ((shift & 0x3f) - 1);
}

int32_t MiscUtils::unsignedShift(int32_t num, int32_t shift) {
    return (shift & 0x1f) == 0 ? num : (((uint32_t)num >> 1) & 0x7fffffff) >> ((shift & 0x1f) - 1);
}

}
