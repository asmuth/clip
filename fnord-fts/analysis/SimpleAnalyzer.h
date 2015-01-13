/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef SIMPLEANALYZER_H
#define SIMPLEANALYZER_H

#include "Analyzer.h"

namespace Lucene {

/// An {@link Analyzer} that filters {@link LetterTokenizer} with {@link LowerCaseFilter}
class SimpleAnalyzer : public Analyzer {
public:
    virtual ~SimpleAnalyzer();

    LUCENE_CLASS(SimpleAnalyzer);

public:
    virtual TokenStreamPtr tokenStream(const String& fieldName, const ReaderPtr& reader);
    virtual TokenStreamPtr reusableTokenStream(const String& fieldName, const ReaderPtr& reader);
};

}

#endif
