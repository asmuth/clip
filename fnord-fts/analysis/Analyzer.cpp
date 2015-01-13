/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "Analyzer.h"
#include "Fieldable.h"

namespace Lucene {

Analyzer::~Analyzer() {
}

TokenStreamPtr Analyzer::reusableTokenStream(const String& fieldName, const ReaderPtr& reader) {
    return tokenStream(fieldName, reader);
}

LuceneObjectPtr Analyzer::getPreviousTokenStream() {
    return tokenStreams.get();
}

void Analyzer::setPreviousTokenStream(const LuceneObjectPtr& stream) {
    tokenStreams.set(stream);
}

int32_t Analyzer::getPositionIncrementGap(const String& fieldName) {
    return 0;
}

int32_t Analyzer::getOffsetGap(const FieldablePtr& field) {
    return field->isTokenized() ? 1 : 0;
}

void Analyzer::close() {
    tokenStreams.close();
}

}
