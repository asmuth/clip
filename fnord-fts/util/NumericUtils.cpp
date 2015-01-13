/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "NumericUtils.h"
#include "MiscUtils.h"
#include "StringUtils.h"

namespace Lucene {

/// The default precision step used by {@link NumericField}, {@link NumericTokenStream}, {@link NumericRangeQuery},
/// and {@link NumericRangeFilter} as default.
const int32_t NumericUtils::PRECISION_STEP_DEFAULT = 4;

/// Longs are stored at lower precision by shifting off lower bits. The shift count is stored as SHIFT_START_LONG +
/// shift in the first character.
const wchar_t NumericUtils::SHIFT_START_LONG = (wchar_t)0x20;

/// The maximum term length (used for char[] buffer size) for encoding long values.
/// @see #longToPrefixCoded(long,int,char[])
const int32_t NumericUtils::BUF_SIZE_LONG = 63 / 7 + 2;

/// Integers are stored at lower precision by shifting off lower bits. The shift count is stored as SHIFT_START_INT +
/// shift in the first character.
const wchar_t NumericUtils::SHIFT_START_INT = (wchar_t)0x60;

/// The maximum term length (used for char[] buffer size) for encoding int values.
/// @see #intToPrefixCoded(int,int,char[])
const int32_t NumericUtils::BUF_SIZE_INT = 31 / 7 + 2;

NumericUtils::~NumericUtils() {
}

int32_t NumericUtils::longToPrefixCoded(int64_t val, int32_t shift, CharArray buffer) {
    if (shift > 63 || shift < 0) {
        boost::throw_exception(IllegalArgumentException(L"Illegal shift value, must be 0..63"));
    }
    int32_t nChars = (63 - shift) / 7 + 1;
    int32_t len = nChars + 1;
    buffer[0] = (wchar_t)(SHIFT_START_LONG + shift);
    int64_t sortableBits = val ^ 0x8000000000000000LL;
    sortableBits = MiscUtils::unsignedShift(sortableBits, (int64_t)shift);
    while (nChars >= 1) {
        // Store 7 bits per character for good efficiency when UTF-8 encoding.  The whole number is
        // right-justified so that lucene can prefix-encode the terms more efficiently.
        buffer[nChars--] = (wchar_t)(sortableBits & 0x7f);
        sortableBits = MiscUtils::unsignedShift(sortableBits, (int64_t)7);
    }
    return len;
}

String NumericUtils::longToPrefixCoded(int64_t val, int32_t shift) {
    CharArray buffer(CharArray::newInstance(BUF_SIZE_LONG));
    int32_t len = longToPrefixCoded(val, shift, buffer);
    return String(buffer.get(), len);
}

String NumericUtils::longToPrefixCoded(int64_t val) {
    return longToPrefixCoded(val, 0);
}

int32_t NumericUtils::intToPrefixCoded(int32_t val, int32_t shift, CharArray buffer) {
    if (shift > 31 || shift < 0) {
        boost::throw_exception(IllegalArgumentException(L"Illegal shift value, must be 0..31"));
    }
    int32_t nChars = (31 - shift) / 7 + 1;
    int32_t len = nChars + 1;
    buffer[0] = (wchar_t)(SHIFT_START_INT + shift);
    int32_t sortableBits = val ^ 0x80000000;
    sortableBits = MiscUtils::unsignedShift(sortableBits, shift);
    while (nChars >= 1) {
        // Store 7 bits per character for good efficiency when UTF-8 encoding.  The whole number is
        // right-justified so that lucene can prefix-encode the terms more efficiently.
        buffer[nChars--] = (wchar_t)(sortableBits & 0x7f);
        sortableBits = MiscUtils::unsignedShift(sortableBits, 7);
    }
    return len;
}

String NumericUtils::intToPrefixCoded(int32_t val, int32_t shift) {
    CharArray buffer(CharArray::newInstance(BUF_SIZE_INT));
    int32_t len = intToPrefixCoded(val, shift, buffer);
    return String(buffer.get(), len);
}

String NumericUtils::intToPrefixCoded(int32_t val) {
    return intToPrefixCoded(val, 0);
}

int64_t NumericUtils::prefixCodedToLong(const String& prefixCoded) {
    int32_t shift = prefixCoded[0] - SHIFT_START_LONG;
    if (shift > 63 || shift < 0) {
        boost::throw_exception(NumberFormatException(L"Invalid shift value in prefixCoded string (is encoded value really a LONG?)"));
    }
    int64_t sortableBits = 0;
    for (int32_t i = 1, len = prefixCoded.length(); i < len; ++i) {
        sortableBits <<= 7;
        wchar_t ch = prefixCoded[i];
        if (ch > 0x7f) {
            boost::throw_exception(NumberFormatException(L"Invalid prefixCoded numerical value representation (char " + StringUtils::toString(ch, 16) +
                                   L" at position " + StringUtils::toString(i) + L" is invalid)"));
        }
        sortableBits |= (int64_t)ch;
    }
    return (sortableBits << shift) ^ 0x8000000000000000LL;
}

int32_t NumericUtils::prefixCodedToInt(const String& prefixCoded) {
    int32_t shift = prefixCoded[0] - SHIFT_START_INT;
    if (shift > 31 || shift < 0) {
        boost::throw_exception(NumberFormatException(L"Invalid shift value in prefixCoded string (is encoded value really a INT?)"));
    }
    int32_t sortableBits = 0;
    for (int32_t i = 1, len = prefixCoded.length(); i < len; ++i) {
        sortableBits <<= 7;
        wchar_t ch = prefixCoded[i];
        if (ch > 0x7f) {
            boost::throw_exception(NumberFormatException(L"Invalid prefixCoded numerical value representation (char " + StringUtils::toString(ch, 16) +
                                   L" at position " + StringUtils::toString(i) + L" is invalid)"));
        }
        sortableBits |= (int32_t)ch;
    }
    return (sortableBits << shift) ^ 0x80000000;
}

int64_t NumericUtils::doubleToSortableLong(double val) {
    int64_t f = MiscUtils::doubleToRawLongBits(val);
    if (f < 0) {
        f ^= 0x7fffffffffffffffLL;
    }
    return f;
}

String NumericUtils::doubleToPrefixCoded(double val) {
    return longToPrefixCoded(doubleToSortableLong(val));
}

double NumericUtils::sortableLongToDouble(int64_t val) {
    if (val < 0) {
        val ^= 0x7fffffffffffffffLL;
    }
    return MiscUtils::longBitsToDouble(val);
}

double NumericUtils::prefixCodedToDouble(const String& val) {
    return sortableLongToDouble(prefixCodedToLong(val));
}

void NumericUtils::splitLongRange(const LongRangeBuilderPtr& builder, int32_t precisionStep, int64_t minBound, int64_t maxBound) {
    splitRange(builder, 64, precisionStep, minBound, maxBound);
}

void NumericUtils::splitIntRange(const IntRangeBuilderPtr& builder, int32_t precisionStep, int32_t minBound, int32_t maxBound) {
    splitRange(builder, 32, precisionStep, (int64_t)minBound, (int64_t)maxBound);
}

void NumericUtils::splitRange(const LuceneObjectPtr& builder, int32_t valSize, int32_t precisionStep, int64_t minBound, int64_t maxBound) {
    if (precisionStep < 1) {
        boost::throw_exception(IllegalArgumentException(L"precisionStep must be >=1"));
    }
    if (minBound > maxBound) {
        return;
    }
    for (int32_t shift = 0; ; shift += precisionStep) {
        // calculate new bounds for inner precision
        int64_t diff = (int64_t)1 << (shift + precisionStep);
        int64_t mask = (((int64_t)1 << precisionStep) - (int64_t)1) << shift;
        bool hasLower = ((minBound & mask) != 0);
        bool hasUpper = ((maxBound & mask) != mask);
        int64_t nextMinBound = ((hasLower ? (minBound + diff) : minBound) & ~mask);
        int64_t nextMaxBound = ((hasUpper ? (maxBound - diff) : maxBound) & ~mask);
        bool lowerWrapped = nextMinBound < minBound;
        bool upperWrapped = nextMaxBound > maxBound;

        if (shift + precisionStep >= valSize || nextMinBound>nextMaxBound || lowerWrapped || upperWrapped) {
            // We are in the lowest precision or the next precision is not available.
            addRange(builder, valSize, minBound, maxBound, shift);
            break; // exit the split recursion loop
        }

        if (hasLower) {
            addRange(builder, valSize, minBound, minBound | mask, shift);
        }
        if (hasUpper) {
            addRange(builder, valSize, maxBound & ~mask, maxBound, shift);
        }

        // recurse to next precision
        minBound = nextMinBound;
        maxBound = nextMaxBound;
    }
}

void NumericUtils::addRange(const LuceneObjectPtr& builder, int32_t valSize, int64_t minBound, int64_t maxBound, int32_t shift) {
    // for the max bound set all lower bits (that were shifted away): this is important for testing or other
    // usages of the splitted range (eg. to reconstruct the full range). The prefixEncoding will remove the
    // bits anyway, so they do not hurt!
    maxBound |= ((int64_t)1 << shift) - (int64_t)1;
    // delegate to correct range builder
    switch (valSize) {
    case 64:
        boost::dynamic_pointer_cast<LongRangeBuilder>(builder)->addRange(minBound, maxBound, shift);
        break;
    case 32:
        boost::dynamic_pointer_cast<IntRangeBuilder>(builder)->addRange((int32_t)minBound, (int32_t)maxBound, shift);
        break;
    default:
        boost::throw_exception(IllegalArgumentException(L"valSize must be 32 or 64."));
    }
}

LongRangeBuilder::~LongRangeBuilder() {
}

void LongRangeBuilder::addRange(const String& minPrefixCoded, const String& maxPrefixCoded) {
    boost::throw_exception(UnsupportedOperationException());
}

void LongRangeBuilder::addRange(int64_t min, int64_t max, int32_t shift) {
    addRange(NumericUtils::longToPrefixCoded(min, shift), NumericUtils::longToPrefixCoded(max, shift));
}

IntRangeBuilder::~IntRangeBuilder() {
}

void IntRangeBuilder::addRange(const String& minPrefixCoded, const String& maxPrefixCoded) {
    boost::throw_exception(UnsupportedOperationException());
}

void IntRangeBuilder::addRange(int32_t min, int32_t max, int32_t shift) {
    addRange(NumericUtils::intToPrefixCoded(min, shift), NumericUtils::intToPrefixCoded(max, shift));
}

}
