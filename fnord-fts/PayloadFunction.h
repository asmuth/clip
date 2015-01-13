/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef PAYLOADFUNCTION_H
#define PAYLOADFUNCTION_H

#include "fnord-fts/util/LuceneObject.h"

namespace Lucene {

/// An abstract class that defines a way for Payload*Query instances to transform the cumulative
/// effects of payload scores for a document.
///
/// @see PayloadTermQuery for more information
class PayloadFunction : public LuceneObject {
protected:
    PayloadFunction();

public:
    virtual ~PayloadFunction();
    LUCENE_CLASS(PayloadFunction);

public:
    /// Calculate the score up to this point for this doc and field
    /// @param docId The current doc
    /// @param field The field
    /// @param start The start position of the matching Span
    /// @param end The end position of the matching Span
    /// @param numPayloadsSeen The number of payloads seen so far
    /// @param currentScore The current score so far
    /// @param currentPayloadScore The score for the current payload
    /// @return The new current Score
    ///
    /// @see Spans
    virtual double currentScore(int32_t docId, const String& field, int32_t start, int32_t end, int32_t numPayloadsSeen,
                                double currentScore, double currentPayloadScore) = 0;

    /// Calculate the final score for all the payloads seen so far for this doc/field
    /// @param docId The current doc
    /// @param field The current field
    /// @param numPayloadsSeen The total number of payloads seen on this document
    /// @param payloadScore The raw score for those payloads
    /// @return The final score for the payloads
    virtual double docScore(int32_t docId, const String& field, int32_t numPayloadsSeen, double payloadScore) = 0;

    /// Return hash code for this object.
    virtual int32_t hashCode() = 0;

    /// Return whether two objects are equal
    virtual bool equals(const LuceneObjectPtr& other) = 0;
};

}

#endif
