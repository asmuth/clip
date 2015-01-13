/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef _STANDARDANALYZER_H
#define _STANDARDANALYZER_H

#include "LuceneObject.h"

namespace Lucene {

class StandardAnalyzerSavedStreams : public LuceneObject {
public:
    virtual ~StandardAnalyzerSavedStreams();

public:
    StandardTokenizerPtr tokenStream;
    TokenStreamPtr filteredTokenStream;
};

}

#endif
