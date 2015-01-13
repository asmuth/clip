/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef DEFAULTSIMILARITY_H
#define DEFAULTSIMILARITY_H

#include "Similarity.h"

namespace Lucene {

/// Default scoring implementation.
class DefaultSimilarity : public Similarity {
public:
    DefaultSimilarity();
    virtual ~DefaultSimilarity();

    LUCENE_CLASS(DefaultSimilarity);

protected:
    bool discountOverlaps; // Default false

public:
    /// Implemented as state->getBoost() * lengthNorm(numTerms), where numTerms is {@link
    /// FieldInvertState#getLength()} if {@link #setDiscountOverlaps} is false, else it's {@link
    /// FieldInvertState#getLength()} - {@link FieldInvertState#getNumOverlap()}.
    virtual double computeNorm(const String& fieldName, const FieldInvertStatePtr& state);

    /// Implemented as 1 / sqrt(numTerms).
    virtual double lengthNorm(const String& fieldName, int32_t numTokens);

    /// Implemented as 1 / sqrt(sumOfSquaredWeights).
    virtual double queryNorm(double sumOfSquaredWeights);

    /// Implemented as sqrt(freq).
    virtual double tf(double freq);

    /// Implemented as 1 / (distance + 1).
    virtual double sloppyFreq(int32_t distance);

    /// Implemented as log(numDocs / (docFreq + 1)) + 1.
    virtual double idf(int32_t docFreq, int32_t numDocs);

    /// Implemented as overlap / maxOverlap.
    virtual double coord(int32_t overlap, int32_t maxOverlap);

    /// Determines whether overlap tokens (Tokens with 0 position increment) are ignored when computing
    /// norm.  By default this is false, meaning overlap tokens are counted just like non-overlap tokens.
    /// @see #computeNorm
    void setDiscountOverlaps(bool v);

    /// @see #setDiscountOverlaps
    bool getDiscountOverlaps();
};

}

#endif
