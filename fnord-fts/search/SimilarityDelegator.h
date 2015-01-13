/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef SIMILARITYDELEGATOR_H
#define SIMILARITYDELEGATOR_H

#include "fnord-fts/search/Similarity.h"

namespace Lucene {

/// Delegating scoring implementation.  Useful in {@link Query#getSimilarity(Searcher)} implementations,
/// to override only certain methods of a Searcher's Similarity implementation.
class SimilarityDelegator : public Similarity {
public:
    SimilarityDelegator(const SimilarityPtr& delegee);
    virtual ~SimilarityDelegator();

    LUCENE_CLASS(SimilarityDelegator);

protected:
    SimilarityPtr delegee;

public:
    virtual double computeNorm(const String& field, const FieldInvertStatePtr& state);
    virtual double lengthNorm(const String& fieldName, int32_t numTokens);
    virtual double queryNorm(double sumOfSquaredWeights);
    virtual double tf(double freq);
    virtual double sloppyFreq(int32_t distance);
    virtual double idf(int32_t docFreq, int32_t numDocs);
    virtual double coord(int32_t overlap, int32_t maxOverlap);
    virtual double scorePayload(int32_t docId, const String& fieldName, int32_t start, int32_t end, ByteArray payload, int32_t offset, int32_t length);
};

}

#endif
