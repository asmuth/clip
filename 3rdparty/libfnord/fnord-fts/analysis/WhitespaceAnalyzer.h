/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef WHITESPACEANALYZER_H
#define WHITESPACEANALYZER_H

#include "fnord-fts/analysis/Analyzer.h"

namespace fnord {
namespace fts {

/// An Analyzer that uses {@link WhitespaceTokenizer}.
class WhitespaceAnalyzer : public Analyzer {
public:
    virtual ~WhitespaceAnalyzer();

    LUCENE_CLASS(WhitespaceAnalyzer);

public:
    virtual TokenStreamPtr tokenStream(const String& fieldName, const ReaderPtr& reader);
    virtual TokenStreamPtr reusableTokenStream(const String& fieldName, const ReaderPtr& reader);
};

}

}
#endif
