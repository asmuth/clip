/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "OpenBitSetIterator.h"
#include "OpenBitSet.h"
#include "MiscUtils.h"

namespace Lucene {

/// The General Idea: instead of having an array per byte that has the offsets of the
/// next set bit, that array could be packed inside a 32 bit integer (8 4 bit numbers).
/// That should be faster than accessing an array for each index, and the total array
/// size is kept smaller (256*sizeof(int32_t))=1K
const int32_t OpenBitSetIterator::bitlist[] = {
    0x0, 0x1, 0x2, 0x21, 0x3, 0x31, 0x32, 0x321, 0x4, 0x41, 0x42, 0x421, 0x43,
    0x431, 0x432, 0x4321, 0x5, 0x51, 0x52, 0x521, 0x53, 0x531, 0x532, 0x5321,
    0x54, 0x541, 0x542, 0x5421, 0x543, 0x5431, 0x5432, 0x54321, 0x6, 0x61, 0x62,
    0x621, 0x63, 0x631, 0x632, 0x6321, 0x64, 0x641, 0x642, 0x6421, 0x643, 0x6431,
    0x6432, 0x64321, 0x65, 0x651, 0x652, 0x6521, 0x653, 0x6531, 0x6532, 0x65321,
    0x654, 0x6541, 0x6542, 0x65421, 0x6543, 0x65431, 0x65432, 0x654321, 0x7,
    0x71, 0x72, 0x721, 0x73, 0x731, 0x732, 0x7321, 0x74, 0x741, 0x742, 0x7421,
    0x743, 0x7431, 0x7432, 0x74321, 0x75, 0x751, 0x752, 0x7521, 0x753, 0x7531,
    0x7532, 0x75321, 0x754, 0x7541, 0x7542, 0x75421, 0x7543, 0x75431, 0x75432,
    0x754321, 0x76, 0x761, 0x762, 0x7621, 0x763, 0x7631, 0x7632, 0x76321, 0x764,
    0x7641, 0x7642, 0x76421, 0x7643, 0x76431, 0x76432, 0x764321, 0x765, 0x7651,
    0x7652, 0x76521, 0x7653, 0x76531, 0x76532, 0x765321, 0x7654, 0x76541, 0x76542,
    0x765421, 0x76543, 0x765431, 0x765432, 0x7654321, 0x8, 0x81, 0x82, 0x821,
    0x83, 0x831, 0x832, 0x8321, 0x84, 0x841, 0x842, 0x8421, 0x843, 0x8431, 0x8432,
    0x84321, 0x85, 0x851, 0x852, 0x8521, 0x853, 0x8531, 0x8532, 0x85321, 0x854,
    0x8541, 0x8542, 0x85421, 0x8543, 0x85431, 0x85432, 0x854321, 0x86, 0x861,
    0x862, 0x8621, 0x863, 0x8631, 0x8632, 0x86321, 0x864, 0x8641, 0x8642, 0x86421,
    0x8643, 0x86431, 0x86432, 0x864321, 0x865, 0x8651, 0x8652, 0x86521, 0x8653,
    0x86531, 0x86532, 0x865321, 0x8654, 0x86541, 0x86542, 0x865421, 0x86543,
    0x865431, 0x865432, 0x8654321, 0x87, 0x871, 0x872, 0x8721, 0x873, 0x8731,
    0x8732, 0x87321, 0x874, 0x8741, 0x8742, 0x87421, 0x8743, 0x87431, 0x87432,
    0x874321, 0x875, 0x8751, 0x8752, 0x87521, 0x8753, 0x87531, 0x87532, 0x875321,
    0x8754, 0x87541, 0x87542, 0x875421, 0x87543, 0x875431, 0x875432, 0x8754321,
    0x876, 0x8761, 0x8762, 0x87621, 0x8763, 0x87631, 0x87632, 0x876321, 0x8764,
    0x87641, 0x87642, 0x876421, 0x87643, 0x876431, 0x876432, 0x8764321, 0x8765,
    0x87651, 0x87652, 0x876521, 0x87653, 0x876531, 0x876532, 0x8765321, 0x87654,
    0x876541, 0x876542, 0x8765421, 0x876543, 0x8765431, 0x8765432, static_cast<int32_t>(0x87654321)
};

OpenBitSetIterator::OpenBitSetIterator(const OpenBitSetPtr& bitSet) {
    arr = bitSet->getBits();
    words = bitSet->getNumWords();
    i = -1;
    word = 0;
    wordShift = 0;
    indexArray = 0;
    curDocId = -1;
}

OpenBitSetIterator::OpenBitSetIterator(LongArray bits, int32_t numWords) {
    arr = bits;
    words = numWords;
    i = -1;
    word = 0;
    wordShift = 0;
    indexArray = 0;
    curDocId = -1;
}

OpenBitSetIterator::~OpenBitSetIterator() {
}

void OpenBitSetIterator::shift() {
    if ((int32_t)word == 0) {
        wordShift += 32;
        word = MiscUtils::unsignedShift(word, (int64_t)32);
    }
    if ((word & 0x0000ffff) == 0) {
        wordShift += 16;
        word = MiscUtils::unsignedShift(word, (int64_t)16);
    }
    if ((word & 0x000000ff) == 0) {
        wordShift += 8;
        word = MiscUtils::unsignedShift(word, (int64_t)8);
    }
    indexArray = bitlist[(int32_t)word & 0xff];
}

int32_t OpenBitSetIterator::nextDoc() {
    if (indexArray == 0) {
        if (word != 0) {
            word = MiscUtils::unsignedShift(word, (int64_t)8);
            wordShift += 8;
        }

        while (word == 0) {
            if (++i >= words) {
                curDocId = NO_MORE_DOCS;
                return curDocId;
            }
            word = arr[i];
            wordShift = -1; // loop invariant code motion should move this
        }

        // after the first time, should I go with a linear search, or stick with the binary search in shift?
        shift();
    }

    int32_t bitIndex = (indexArray & 0x0f) + wordShift;
    indexArray = MiscUtils::unsignedShift(indexArray, 4);
    curDocId = (i << 6) + bitIndex;
    return curDocId;
}

int32_t OpenBitSetIterator::advance(int32_t target) {
    indexArray = 0;
    i = target >> 6;
    if (i >= words) {
        word = 0; // setup so next() will also return -1
        curDocId = NO_MORE_DOCS;
        return curDocId;
    }
    wordShift = target & 0x3f;
    word = MiscUtils::unsignedShift(arr[i], (int64_t)wordShift);
    if (word != 0) {
        --wordShift;    // compensate for 1 based arrIndex
    } else {
        while (word == 0) {
            if (++i >= words) {
                curDocId = NO_MORE_DOCS;
                return curDocId;
            }
            word = arr[i];
        }
        wordShift = -1;
    }

    shift();

    int32_t bitIndex = (indexArray & 0x0f) + wordShift;
    indexArray = MiscUtils::unsignedShift(indexArray, 4);
    curDocId = (i << 6) + bitIndex;
    return curDocId;
}

int32_t OpenBitSetIterator::docID() {
    return curDocId;
}

}
