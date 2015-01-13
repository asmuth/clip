/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "CharTokenizer.h"
#include "OffsetAttribute.h"
#include "TermAttribute.h"
#include "Reader.h"

namespace Lucene {

const int32_t CharTokenizer::MAX_WORD_LEN = 255;
const int32_t CharTokenizer::IO_BUFFER_SIZE = 4096;

CharTokenizer::CharTokenizer(const ReaderPtr& input) : Tokenizer(input) {
    offset = 0;
    bufferIndex = 0;
    dataLen = 0;
    ioBuffer = CharArray::newInstance(IO_BUFFER_SIZE);

    offsetAtt = addAttribute<OffsetAttribute>();
    termAtt = addAttribute<TermAttribute>();
}

CharTokenizer::CharTokenizer(const AttributeSourcePtr& source, const ReaderPtr& input) : Tokenizer(source, input) {
    offset = 0;
    bufferIndex = 0;
    dataLen = 0;
    ioBuffer = CharArray::newInstance(IO_BUFFER_SIZE);

    offsetAtt = addAttribute<OffsetAttribute>();
    termAtt = addAttribute<TermAttribute>();
}

CharTokenizer::CharTokenizer(const AttributeFactoryPtr& factory, const ReaderPtr& input) : Tokenizer(factory, input) {
    offset = 0;
    bufferIndex = 0;
    dataLen = 0;
    ioBuffer = CharArray::newInstance(IO_BUFFER_SIZE);

    offsetAtt = addAttribute<OffsetAttribute>();
    termAtt = addAttribute<TermAttribute>();
}

CharTokenizer::~CharTokenizer() {
}

wchar_t CharTokenizer::normalize(wchar_t c) {
    return c;
}

bool CharTokenizer::incrementToken() {
    clearAttributes();
    int32_t length = 0;
    int32_t start = bufferIndex;
    CharArray buffer(termAtt->termBuffer());
    while (true) {
        if (bufferIndex >= dataLen) {
            offset += dataLen;
            dataLen = input->read(ioBuffer.get(), 0, ioBuffer.size());
            if (dataLen == -1) {
                dataLen = 0; // so next offset += dataLen won't decrement offset
                if (length > 0) {
                    break;
                } else {
                    return false;
                }
            }
            bufferIndex = 0;
        }

        wchar_t c = ioBuffer[bufferIndex++];

        if (isTokenChar(c)) { // if it's a token char
            if (length == 0) {
                start = offset + bufferIndex - 1;
            } else if (length == buffer.size()) {
                buffer = termAtt->resizeTermBuffer(1 + length);
            }

            buffer[length++] = normalize(c); // buffer it, normalized

            if (length == MAX_WORD_LEN) { // buffer overflow!
                break;
            }
        } else if (length > 0) { // at non-Letter with chars
            break;    // return them
        }
    }

    termAtt->setTermLength(length);
    offsetAtt->setOffset(correctOffset(start), correctOffset(start + length));

    return true;
}

void CharTokenizer::end() {
    // set final offset
    int32_t finalOffset = correctOffset(offset);
    offsetAtt->setOffset(finalOffset, finalOffset);
}

void CharTokenizer::reset(const ReaderPtr& input) {
    Tokenizer::reset(input);
    bufferIndex = 0;
    offset = 0;
    dataLen = 0;
}

}
