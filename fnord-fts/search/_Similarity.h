/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef _SIMILARITY_H
#define _SIMILARITY_H

#include "Explanation.h"

namespace Lucene {

class SimilarityIDFExplanation : public IDFExplanation {
public:
    SimilarityIDFExplanation(int32_t df, int32_t max, double idf);
    SimilarityIDFExplanation(const String& exp, double idf);

    virtual ~SimilarityIDFExplanation();

    LUCENE_CLASS(SimilarityIDFExplanation);

protected:
    String exp;
    int32_t df;
    int32_t max;
    double idf;

public:
    virtual String explain();
    virtual double getIdf();
};

}

#endif
