/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef KEYWORDTOKENIZER_H
#define KEYWORDTOKENIZER_H

#include "fnord-fts/analysis/Tokenizer.h"

namespace Lucene {

/// Emits the entire input as a single token.
class KeywordTokenizer : public Tokenizer {
public:
    KeywordTokenizer(const ReaderPtr& input);
    KeywordTokenizer(const ReaderPtr& input, int32_t bufferSize);
    KeywordTokenizer(const AttributeSourcePtr& source, const ReaderPtr& input, int32_t bufferSize);
    KeywordTokenizer(const AttributeFactoryPtr& factory, const ReaderPtr& input, int32_t bufferSize);

    virtual ~KeywordTokenizer();

    LUCENE_CLASS(KeywordTokenizer);

protected:
    static const int32_t DEFAULT_BUFFER_SIZE;

    bool done;
    int32_t finalOffset;
    TermAttributePtr termAtt;
    OffsetAttributePtr offsetAtt;

protected:
    void init(int32_t bufferSize);

public:
    virtual bool incrementToken();
    virtual void end();
    virtual void reset();
};

}

#endif
