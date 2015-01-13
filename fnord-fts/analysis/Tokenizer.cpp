/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "Tokenizer.h"
#include "CharReader.h"

namespace Lucene {

Tokenizer::Tokenizer() {
}

Tokenizer::Tokenizer(const ReaderPtr& input) {
    this->input = CharReader::get(input);
    this->charStream = boost::dynamic_pointer_cast<CharStream>(this->input);
}

Tokenizer::Tokenizer(const AttributeFactoryPtr& factory) : TokenStream(factory) {
}

Tokenizer::Tokenizer(const AttributeFactoryPtr& factory, const ReaderPtr& input) : TokenStream(factory) {
    this->input = CharReader::get(input);
    this->charStream = boost::dynamic_pointer_cast<CharStream>(this->input);
}

Tokenizer::Tokenizer(const AttributeSourcePtr& source) : TokenStream(source) {
}

Tokenizer::Tokenizer(const AttributeSourcePtr& source, const ReaderPtr& input) : TokenStream(source) {
    this->input = CharReader::get(input);
    this->charStream = boost::dynamic_pointer_cast<CharStream>(this->input);
}

Tokenizer::~Tokenizer() {
}

void Tokenizer::close() {
    if (input) {
        input->close();
        input.reset(); // don't hold onto Reader after close
    }
}

int32_t Tokenizer::correctOffset(int32_t currentOff) {
    return charStream ? charStream->correctOffset(currentOff) : currentOff;
}

void Tokenizer::reset(const ReaderPtr& input) {
    this->input = input;
}

}
