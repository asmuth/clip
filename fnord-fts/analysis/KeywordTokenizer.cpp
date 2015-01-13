/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "KeywordTokenizer.h"
#include "TermAttribute.h"
#include "OffsetAttribute.h"
#include "Reader.h"

namespace Lucene {

const int32_t KeywordTokenizer::DEFAULT_BUFFER_SIZE = 256;

KeywordTokenizer::KeywordTokenizer(const ReaderPtr& input) : Tokenizer(input) {
    init(DEFAULT_BUFFER_SIZE);
}

KeywordTokenizer::KeywordTokenizer(const ReaderPtr& input, int32_t bufferSize) : Tokenizer(input) {
    init(bufferSize);
}

KeywordTokenizer::KeywordTokenizer(const AttributeSourcePtr& source, const ReaderPtr& input, int32_t bufferSize) : Tokenizer(source, input) {
    init(bufferSize);
}

KeywordTokenizer::KeywordTokenizer(const AttributeFactoryPtr& factory, const ReaderPtr& input, int32_t bufferSize) : Tokenizer(factory, input) {
    init(bufferSize);
}

KeywordTokenizer::~KeywordTokenizer() {
}

void KeywordTokenizer::init(int32_t bufferSize) {
    this->done = false;
    this->finalOffset = 0;
    this->termAtt = addAttribute<TermAttribute>();
    this->offsetAtt = addAttribute<OffsetAttribute>();
    this->termAtt->resizeTermBuffer(bufferSize);
}

bool KeywordTokenizer::incrementToken() {
    if (!done) {
        clearAttributes();
        done = true;
        int32_t upto = 0;
        CharArray buffer(termAtt->termBuffer());
        while (true) {
            int32_t length = input->read(buffer.get(), upto, buffer.size() - upto);
            if (length == -1) {
                break;
            }
            upto += length;
            if (upto == buffer.size()) {
                buffer = termAtt->resizeTermBuffer(buffer.size() + 1);
            }
        }
        termAtt->setTermLength(upto);
        finalOffset = correctOffset(upto);
        offsetAtt->setOffset(correctOffset(0), finalOffset);
        return true;
    }
    return false;
}

void KeywordTokenizer::end() {
    // set final offset
    offsetAtt->setOffset(finalOffset, finalOffset);
}

void KeywordTokenizer::reset() {
    Tokenizer::reset(input);
    done = false;
}

}
