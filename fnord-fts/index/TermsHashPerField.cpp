/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "TermsHashPerField.h"
#include "TermsHashPerThread.h"
#include "TermsHashConsumerPerThread.h"
#include "TermsHashConsumerPerField.h"
#include "TermsHash.h"
#include "TermAttribute.h"
#include "AttributeSource.h"
#include "DocInverterPerField.h"
#include "DocumentsWriter.h"
#include "IntBlockPool.h"
#include "CharBlockPool.h"
#include "ByteSliceReader.h"
#include "RawPostingList.h"
#include "FieldInvertState.h"
#include "UTF8Stream.h"
#include "MiscUtils.h"

namespace Lucene {

TermsHashPerField::TermsHashPerField(const DocInverterPerFieldPtr& docInverterPerField, const TermsHashPerThreadPtr& perThread, const TermsHashPerThreadPtr& nextPerThread, const FieldInfoPtr& fieldInfo) {
    this->_docInverterPerField = docInverterPerField;
    this->_perThread = perThread;
    this->nextPerThread = nextPerThread;
    this->fieldInfo = fieldInfo;
}

TermsHashPerField::~TermsHashPerField() {
}

void TermsHashPerField::initialize() {
    this->postingsCompacted = false;
    this->numPostings = 0;
    this->postingsHashSize = 4;
    this->postingsHashHalfSize = this->postingsHashSize / 2;
    this->postingsHashMask = this->postingsHashSize - 1;
    this->postingsHash = Collection<RawPostingListPtr>::newInstance(postingsHashSize);
    this->doCall = false;
    this->doNextCall = false;
    this->intUptoStart = 0;

    TermsHashPerThreadPtr perThread(_perThread);
    intPool = perThread->intPool;
    charPool = perThread->charPool;
    bytePool = perThread->bytePool;
    docState = perThread->docState;
    DocInverterPerFieldPtr docInverterPerField(_docInverterPerField);
    fieldState = docInverterPerField->fieldState;
    this->consumer = perThread->consumer->addField(shared_from_this(), fieldInfo);
    streamCount = consumer->getStreamCount();
    numPostingInt = 2 * streamCount;
    if (nextPerThread) {
        nextPerField = boost::dynamic_pointer_cast<TermsHashPerField>(nextPerThread->addField(docInverterPerField, fieldInfo));
    }
}

void TermsHashPerField::shrinkHash(int32_t targetSize) {
    BOOST_ASSERT(postingsCompacted || numPostings == 0);

    int32_t newSize = 4;
    if (newSize != postingsHash.size()) {
        postingsHash.resize(newSize);
        postingsHashSize = newSize;
        postingsHashHalfSize = newSize / 2;
        postingsHashMask = newSize - 1;
    }
    MiscUtils::arrayFill(postingsHash.begin(), 0, postingsHash.size(), RawPostingListPtr());
}

void TermsHashPerField::reset() {
    if (!postingsCompacted) {
        compactPostings();
    }
    BOOST_ASSERT(numPostings <= postingsHash.size());
    if (numPostings > 0) {
        TermsHashPtr(TermsHashPerThreadPtr(_perThread)->_termsHash)->recyclePostings(postingsHash, numPostings);
        MiscUtils::arrayFill(postingsHash.begin(), 0, numPostings, RawPostingListPtr());
        numPostings = 0;
    }
    postingsCompacted = false;
    if (nextPerField) {
        nextPerField->reset();
    }
}

void TermsHashPerField::abort() {
    SyncLock syncLock(this);
    reset();
    if (nextPerField) {
        nextPerField->abort();
    }
}

void TermsHashPerField::initReader(const ByteSliceReaderPtr& reader, const RawPostingListPtr& p, int32_t stream) {
    BOOST_ASSERT(stream < streamCount);
    IntArray ints(intPool->buffers[p->intStart >> DocumentsWriter::INT_BLOCK_SHIFT]);
    int32_t upto = (p->intStart & DocumentsWriter::INT_BLOCK_MASK);
    reader->init(bytePool, p->byteStart + stream * ByteBlockPool::FIRST_LEVEL_SIZE(), ints[upto + stream]);
}

void TermsHashPerField::compactPostings() {
    SyncLock syncLock(this);
    int32_t upto = 0;
    for (int32_t i = 0; i < postingsHashSize; ++i) {
        if (postingsHash[i]) {
            if (upto < i) {
                postingsHash[upto] = postingsHash[i];
                postingsHash[i].reset();
            }
            ++upto;
        }
    }

    BOOST_ASSERT(upto == numPostings);
    postingsCompacted = true;
}

struct comparePostings {
    comparePostings(Collection<CharArray> buffers) {
        this->buffers = buffers;
    }

    /// Compares term text for two Posting instance
    inline bool operator()(const RawPostingListPtr& first, const RawPostingListPtr& second) const {
        if (first == second) {
            return false;
        }

        wchar_t* text1 = buffers[first->textStart >> DocumentsWriter::CHAR_BLOCK_SHIFT].get();
        int32_t pos1 = (first->textStart & DocumentsWriter::CHAR_BLOCK_MASK);
        wchar_t* text2 = buffers[second->textStart >> DocumentsWriter::CHAR_BLOCK_SHIFT].get();
        int32_t pos2 = (second->textStart & DocumentsWriter::CHAR_BLOCK_MASK);

        BOOST_ASSERT(text1 != text2 || pos1 != pos2);

        while (true) {
            wchar_t c1 = text1[pos1++];
            wchar_t c2 = text2[pos2++];
            if (c1 != c2) {
                if (c2 == UTF8Base::UNICODE_TERMINATOR) {
                    return false;
                } else if (c1 == UTF8Base::UNICODE_TERMINATOR) {
                    return true;
                } else {
                    return (c1 < c2);
                }
            } else {
                // This method should never compare equal postings unless first == second
                BOOST_ASSERT(c1 != UTF8Base::UNICODE_TERMINATOR);
            }
        }
    }

    Collection<CharArray> buffers;
};

Collection<RawPostingListPtr> TermsHashPerField::sortPostings() {
    compactPostings();
    std::sort(postingsHash.begin(), postingsHash.begin() + numPostings, comparePostings(charPool->buffers));
    return postingsHash;
}

bool TermsHashPerField::postingEquals(const wchar_t* tokenText, int32_t tokenTextLen) {
    wchar_t* text = TermsHashPerThreadPtr(_perThread)->charPool->buffers[p->textStart >> DocumentsWriter::CHAR_BLOCK_SHIFT].get();
    BOOST_ASSERT(text);
    int32_t pos = (p->textStart & DocumentsWriter::CHAR_BLOCK_MASK);
    int32_t tokenPos = 0;
    for (; tokenPos < tokenTextLen; ++pos, ++tokenPos) {
        if (tokenText[tokenPos] != text[pos]) {
            return false;
        }
    }
    return (text[pos] == UTF8Base::UNICODE_TERMINATOR);
}

void TermsHashPerField::start(const FieldablePtr& field) {
    termAtt = fieldState->attributeSource->addAttribute<TermAttribute>();
    consumer->start(field);
    if (nextPerField) {
        nextPerField->start(field);
    }
}

bool TermsHashPerField::start(Collection<FieldablePtr> fields, int32_t count) {
    doCall = consumer->start(fields, count);
    if (nextPerField) {
        doNextCall = nextPerField->start(fields, count);
    }
    return (doCall || doNextCall);
}

void TermsHashPerField::add(int32_t textStart) {
    // Secondary entry point (for 2nd and subsequent TermsHash), we hash by textStart
    int32_t code = textStart;

    int32_t hashPos = (code & postingsHashMask);

    BOOST_ASSERT(!postingsCompacted);

    // Locate RawPostingList in hash
    p = postingsHash[hashPos];

    if (p && p->textStart != textStart) {
        // Conflict: keep searching different locations in the hash table.
        int32_t inc = (((code >> 8) + code) | 1);
        do {
            code += inc;
            hashPos = (code & postingsHashMask);
            p = postingsHash[hashPos];
        } while (p && p->textStart != textStart);
    }

    if (!p) {
        // First time we are seeing this token since we last flushed the hash.
        TermsHashPerThreadPtr perThread(_perThread);

        // Refill?
        if (perThread->freePostingsCount == 0) {
            perThread->morePostings();
        }

        // Pull next free RawPostingList from free list
        p = perThread->freePostings[--perThread->freePostingsCount];
        BOOST_ASSERT(p);

        p->textStart = textStart;

        BOOST_ASSERT(!postingsHash[hashPos]);
        postingsHash[hashPos] = p;
        ++numPostings;

        if (numPostings == postingsHashHalfSize) {
            rehashPostings(2 * postingsHashSize);
        }

        // Init stream slices
        if (numPostingInt + intPool->intUpto > DocumentsWriter::INT_BLOCK_SIZE) {
            intPool->nextBuffer();
        }

        if (DocumentsWriter::BYTE_BLOCK_SIZE - bytePool->byteUpto < numPostingInt * ByteBlockPool::FIRST_LEVEL_SIZE()) {
            bytePool->nextBuffer();
        }

        intUptos = intPool->buffer;
        intUptoStart = intPool->intUpto;
        intPool->intUpto += streamCount;

        p->intStart = intUptoStart + intPool->intOffset;

        for (int32_t i = 0; i < streamCount; ++i) {
            int32_t upto = bytePool->newSlice(ByteBlockPool::FIRST_LEVEL_SIZE());
            intUptos[intUptoStart + i] = upto + bytePool->byteOffset;
        }
        p->byteStart = intUptos[intUptoStart];

        consumer->newTerm(p);
    } else {
        intUptos = intPool->buffers[p->intStart >> DocumentsWriter::INT_BLOCK_SHIFT];
        intUptoStart = (p->intStart & DocumentsWriter::INT_BLOCK_MASK);
        consumer->addTerm(p);
    }
}

void TermsHashPerField::add() {
    BOOST_ASSERT(!postingsCompacted);

    // Get the text of this term.
    wchar_t* tokenText = termAtt->termBufferArray();
    int32_t tokenTextLen = termAtt->termLength();

    // Compute hashcode and replace any invalid UTF16 sequences
    int32_t downto = tokenTextLen;
    int32_t code = 0;

    while (downto > 0) {
        wchar_t ch = tokenText[--downto];

#ifdef LPP_UNICODE_CHAR_SIZE_2
        if (ch >= UTF8Base::TRAIL_SURROGATE_MIN && ch <= UTF8Base::TRAIL_SURROGATE_MAX) {
            if (downto == 0) {
                // Unpaired
                ch = UTF8Base::UNICODE_REPLACEMENT_CHAR;
                tokenText[downto] = ch;
            } else {
                wchar_t ch2 = tokenText[downto - 1];
                if (ch2 >= UTF8Base::LEAD_SURROGATE_MIN && ch2 <= UTF8Base::LEAD_SURROGATE_MAX) {
                    // OK: high followed by low.  This is a valid surrogate pair.
                    code = ((code * 31) + ch) * 31 + ch2;
                    --downto;
                    continue;
                } else {
                    // Unpaired
                    ch = UTF8Base::UNICODE_REPLACEMENT_CHAR;
                    tokenText[downto] = ch;
                }
            }
        } else if (ch >= UTF8Base::LEAD_SURROGATE_MIN && (ch <= UTF8Base::LEAD_SURROGATE_MAX || ch == UTF8Base::UNICODE_TERMINATOR)) {
            // Unpaired or UTF8Base::UNICODE_TERMINATOR
            ch = UTF8Base::UNICODE_REPLACEMENT_CHAR;
            tokenText[downto] = ch;
        }
#else
        if (ch == UTF8Base::UNICODE_TERMINATOR) {
            // Unpaired or UTF8Base::UNICODE_TERMINATOR
            ch = UTF8Base::UNICODE_REPLACEMENT_CHAR;
            tokenText[downto] = ch;
        }
#endif

        code = (code * 31) + ch;
    }

    int32_t hashPos = (code & postingsHashMask);

    // Locate RawPostingList in hash
    p = postingsHash[hashPos];

    if (p && !postingEquals(tokenText, tokenTextLen)) {
        // Conflict: keep searching different locations in the hash table.
        int32_t inc = (((code >> 8) + code) | 1);
        do {
            code += inc;
            hashPos = (code & postingsHashMask);
            p = postingsHash[hashPos];
        } while (p && !postingEquals(tokenText, tokenTextLen));
    }

    if (!p) {
        // First time we are seeing this token since we last flushed the hash.
        int32_t textLen1 = 1 + tokenTextLen;
        if (textLen1 + charPool->charUpto > DocumentsWriter::CHAR_BLOCK_SIZE) {
            if (textLen1 > DocumentsWriter::CHAR_BLOCK_SIZE) {
                // Just skip this term, to remain as robust as possible during indexing.  A TokenFilter
                // can be inserted into the analyzer chain if other behavior is wanted (pruning the term
                // to a prefix, throwing an exception, etc).

                if (docState->maxTermPrefix.empty()) {
                    docState->maxTermPrefix.append(tokenText, std::min((int32_t)30, tokenTextLen));
                }

                consumer->skippingLongTerm();
                return;
            }
            charPool->nextBuffer();
        }

        TermsHashPerThreadPtr perThread(_perThread);

        // Refill?
        if (perThread->freePostingsCount == 0) {
            perThread->morePostings();
        }

        // Pull next free RawPostingList from free list
        p = perThread->freePostings[--perThread->freePostingsCount];
        BOOST_ASSERT(p);

        wchar_t* text = charPool->buffer.get();
        int32_t textUpto = charPool->charUpto;

        p->textStart = textUpto + charPool->charOffset;
        charPool->charUpto += textLen1;

        MiscUtils::arrayCopy(tokenText, 0, text, textUpto, tokenTextLen);
        text[textUpto + tokenTextLen] = UTF8Base::UNICODE_TERMINATOR;

        BOOST_ASSERT(!postingsHash[hashPos]);
        postingsHash[hashPos] = p;
        ++numPostings;

        if (numPostings == postingsHashHalfSize) {
            rehashPostings(2 * postingsHashSize);
        }

        // Init stream slices
        if (numPostingInt + intPool->intUpto > DocumentsWriter::INT_BLOCK_SIZE) {
            intPool->nextBuffer();
        }

        if (DocumentsWriter::BYTE_BLOCK_SIZE - bytePool->byteUpto < numPostingInt * ByteBlockPool::FIRST_LEVEL_SIZE()) {
            bytePool->nextBuffer();
        }

        intUptos = intPool->buffer;
        intUptoStart = intPool->intUpto;
        intPool->intUpto += streamCount;

        p->intStart = intUptoStart + intPool->intOffset;

        for (int32_t i = 0; i < streamCount; ++i) {
            int32_t upto = bytePool->newSlice(ByteBlockPool::FIRST_LEVEL_SIZE());
            intUptos[intUptoStart + i] = upto + bytePool->byteOffset;
        }
        p->byteStart = intUptos[intUptoStart];

        consumer->newTerm(p);
    } else {
        intUptos = intPool->buffers[p->intStart >> DocumentsWriter::INT_BLOCK_SHIFT];
        intUptoStart = (p->intStart & DocumentsWriter::INT_BLOCK_MASK);
        consumer->addTerm(p);
    }

    if (doNextCall) {
        nextPerField->add(p->textStart);
    }
}

void TermsHashPerField::writeByte(int32_t stream, int8_t b) {
    int32_t upto = intUptos[intUptoStart + stream];
    ByteArray bytes(bytePool->buffers[upto >> DocumentsWriter::BYTE_BLOCK_SHIFT]);
    BOOST_ASSERT(bytes);
    int32_t offset = (upto & DocumentsWriter::BYTE_BLOCK_MASK);
    if (bytes[offset] != 0) {
        // End of slice; allocate a new one
        offset = bytePool->allocSlice(bytes, offset);
        bytes = bytePool->buffer;
        intUptos[intUptoStart + stream] = offset + bytePool->byteOffset;
    }
    bytes[offset] = b;
    intUptos[intUptoStart + stream]++;
}

void TermsHashPerField::writeBytes(int32_t stream, const uint8_t* b, int32_t offset, int32_t length) {
    int32_t end = offset + length;
    for (int32_t i = offset; i < end; ++i) {
        writeByte(stream, b[i]);
    }
}

void TermsHashPerField::writeVInt(int32_t stream, int32_t i) {
    BOOST_ASSERT(stream < streamCount);
    while ((i & ~0x7f) != 0) {
        writeByte(stream, (uint8_t)((i & 0x7f) | 0x80));
        i = MiscUtils::unsignedShift(i, 7);
    }
    writeByte(stream, (uint8_t)i);
}

void TermsHashPerField::finish() {
    consumer->finish();
    if (nextPerField) {
        nextPerField->finish();
    }
}

void TermsHashPerField::rehashPostings(int32_t newSize) {
    int32_t newMask = newSize - 1;

    Collection<RawPostingListPtr> newHash(Collection<RawPostingListPtr>::newInstance(newSize));
    TermsHashPerThreadPtr perThread(_perThread);

    for (int32_t i = 0; i < postingsHashSize; ++i) {
        RawPostingListPtr p0(postingsHash[i]);
        if (p0) {
            int32_t code;
            if (perThread->primary) {
                int32_t start = (p0->textStart & DocumentsWriter::CHAR_BLOCK_MASK);
                CharArray text = charPool->buffers[p0->textStart >> DocumentsWriter::CHAR_BLOCK_SHIFT];
                int32_t pos = start;
                while (text[pos] != UTF8Base::UNICODE_TERMINATOR) {
                    ++pos;
                }
                code = 0;
                while (pos > start) {
                    code = (code * 31) + text[--pos];
                }
            } else {
                code = p0->textStart;
            }

            int32_t hashPos = (code & newMask);
            BOOST_ASSERT(hashPos >= 0);
            if (newHash[hashPos]) {
                int32_t inc = (((code >> 8) + code) | 1);
                do {
                    code += inc;
                    hashPos = (code & newMask);
                } while (newHash[hashPos]);
            }
            newHash[hashPos] = p0;
        }
    }

    postingsHashMask = newMask;
    postingsHash = newHash;
    postingsHashSize = newSize;
    postingsHashHalfSize = (newSize >> 1);
}

}
