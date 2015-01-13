/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef CHARSTREAM_H
#define CHARSTREAM_H

#include "fnord-fts/util/Reader.h"

namespace Lucene {

/// CharStream adds {@link #correctOffset} functionality over {@link Reader}.  All Tokenizers accept a CharStream
/// instead of {@link Reader} as input, which enables arbitrary character based filtering before tokenization.
/// The {@link #correctOffset} method fixed offsets to account for removal or insertion of characters, so that the
/// offsets reported in the tokens match the character offsets of the original Reader.
class CharStream : public Reader {
public:
    virtual ~CharStream();
    LUCENE_CLASS(CharStream);

public:
    /// Called by CharFilter(s) and Tokenizer to correct token offset.
    ///
    /// @param currentOff offset as seen in the output
    /// @return corrected offset based on the input
    virtual int32_t correctOffset(int32_t currentOff) = 0;
};

}

#endif
