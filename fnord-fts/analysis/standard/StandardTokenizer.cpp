/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "StandardTokenizer.h"
#include "StandardTokenizerImpl.h"
#include "StandardAnalyzer.h"
#include "TermAttribute.h"
#include "OffsetAttribute.h"
#include "PositionIncrementAttribute.h"
#include "TypeAttribute.h"

namespace Lucene {

const int32_t StandardTokenizer::ALPHANUM = 0;
const int32_t StandardTokenizer::APOSTROPHE = 1;
const int32_t StandardTokenizer::ACRONYM = 2;
const int32_t StandardTokenizer::COMPANY = 3;
const int32_t StandardTokenizer::EMAIL = 4;
const int32_t StandardTokenizer::HOST = 5;
const int32_t StandardTokenizer::NUM = 6;
const int32_t StandardTokenizer::CJ = 7;

/// @deprecated this solves a bug where HOSTs that end with '.' are identified as ACRONYMs.
const int32_t StandardTokenizer::ACRONYM_DEP = 8;

StandardTokenizer::StandardTokenizer(LuceneVersion::Version matchVersion, const ReaderPtr& input) {
    this->scanner = newLucene<StandardTokenizerImpl>(input);
    init(input, matchVersion);
}

StandardTokenizer::StandardTokenizer(LuceneVersion::Version matchVersion, const AttributeSourcePtr& source, const ReaderPtr& input) : Tokenizer(source) {
    this->scanner = newLucene<StandardTokenizerImpl>(input);
    init(input, matchVersion);
}

StandardTokenizer::StandardTokenizer(LuceneVersion::Version matchVersion, const AttributeFactoryPtr& factory, const ReaderPtr& input) : Tokenizer(factory) {
    this->scanner = newLucene<StandardTokenizerImpl>(input);
    init(input, matchVersion);
}

StandardTokenizer::~StandardTokenizer() {
}

const Collection<String> StandardTokenizer::TOKEN_TYPES() {
    static Collection<String> _TOKEN_TYPES;
    if (!_TOKEN_TYPES) {
        _TOKEN_TYPES = newCollection<String>(
                           L"<ALPHANUM>",
                           L"<APOSTROPHE>",
                           L"<ACRONYM>",
                           L"<COMPANY>",
                           L"<EMAIL>",
                           L"<HOST>",
                           L"<NUM>",
                           L"<CJ>",
                           L"<ACRONYM_DEP>"
                       );
    }
    return _TOKEN_TYPES;
}

void StandardTokenizer::init(const ReaderPtr& input, LuceneVersion::Version matchVersion) {
    replaceInvalidAcronym = LuceneVersion::onOrAfter(matchVersion, LuceneVersion::LUCENE_24);
    maxTokenLength = StandardAnalyzer::DEFAULT_MAX_TOKEN_LENGTH;
    this->input = input;
    termAtt = addAttribute<TermAttribute>();
    offsetAtt = addAttribute<OffsetAttribute>();
    posIncrAtt = addAttribute<PositionIncrementAttribute>();
    typeAtt = addAttribute<TypeAttribute>();
}

void StandardTokenizer::setMaxTokenLength(int32_t length) {
    this->maxTokenLength = length;
}

int32_t StandardTokenizer::getMaxTokenLength() {
    return maxTokenLength;
}

bool StandardTokenizer::incrementToken() {
    clearAttributes();
    int32_t posIncr = 1;

    while (true) {
        int32_t tokenType = scanner->getNextToken();

        if (tokenType == StandardTokenizerImpl::YYEOF) {
            return false;
        }

        if (scanner->yylength() <= maxTokenLength) {
            posIncrAtt->setPositionIncrement(posIncr);
            scanner->getText(termAtt);
            int32_t start = scanner->yychar();
            offsetAtt->setOffset(correctOffset(start), correctOffset(start + termAtt->termLength()));

            // This 'if' should be removed in the next release. For now, it converts invalid acronyms to HOST.
            /// When removed, only the 'else' part should remain.
            if (tokenType == ACRONYM_DEP) {
                if (replaceInvalidAcronym) {
                    typeAtt->setType(TOKEN_TYPES()[HOST]);
                    termAtt->setTermLength(termAtt->termLength() - 1); // remove extra '.'
                } else {
                    typeAtt->setType(TOKEN_TYPES()[ACRONYM]);
                }
            } else {
                typeAtt->setType(TOKEN_TYPES()[tokenType]);
            }
            return true;
        } else {
            // When we skip a too-long term, we still increment the position increment
            ++posIncr;
        }
    }
}

void StandardTokenizer::end() {
    // set final offset
    int32_t finalOffset = correctOffset(scanner->yychar() + scanner->yylength());
    offsetAtt->setOffset(finalOffset, finalOffset);
}

void StandardTokenizer::reset(const ReaderPtr& input) {
    Tokenizer::reset(input);
    scanner->reset(input);
}

bool StandardTokenizer::isReplaceInvalidAcronym() {
    return replaceInvalidAcronym;
}

void StandardTokenizer::setReplaceInvalidAcronym(bool replaceInvalidAcronym) {
    this->replaceInvalidAcronym = replaceInvalidAcronym;
}

}
