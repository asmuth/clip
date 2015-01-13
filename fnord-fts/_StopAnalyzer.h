/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef _STOPANALYZER_H
#define _STOPANALYZER_H

#include "LuceneObject.h"

namespace Lucene {

/// Filters LowerCaseTokenizer with StopFilter.
class StopAnalyzerSavedStreams : public LuceneObject {
public:
    virtual ~StopAnalyzerSavedStreams();

    LUCENE_CLASS(StopAnalyzerSavedStreams);

public:
    TokenizerPtr source;
    TokenStreamPtr result;
};

}

#endif
