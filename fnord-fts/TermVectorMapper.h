/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef TERMVECTORMAPPER_H
#define TERMVECTORMAPPER_H

#include "LuceneObject.h"

namespace Lucene {

/// The TermVectorMapper can be used to map Term Vectors into your own structure instead of the parallel
/// array structure used by {@link IndexReader#getTermFreqVector(int,String)}.
///
/// It is up to the implementation to make sure it is thread-safe.
class TermVectorMapper : public LuceneObject {
public:
    /// @param ignoringPositions true if this mapper should tell Lucene to ignore positions even if
    /// they are stored.
    /// @param ignoringOffsets similar to ignoringPositions
    TermVectorMapper(bool ignoringPositions = false, bool ignoringOffsets = false);

    virtual ~TermVectorMapper();

    LUCENE_CLASS(TermVectorMapper);

protected:
    bool ignoringPositions;
    bool ignoringOffsets;

public:
    /// Tell the mapper what to expect in regards to field, number of terms, offset and position storage.
    /// This method will be called once before retrieving the vector for a field.
    ///
    /// This method will be called before {@link #map(String,int,TermVectorOffsetInfo[],int[])}.
    /// @param field The field the vector is for
    /// @param numTerms The number of terms that need to be mapped
    /// @param storeOffsets true if the mapper should expect offset information
    /// @param storePositions true if the mapper should expect positions info
    virtual void setExpectations(const String& field, int32_t numTerms, bool storeOffsets, bool storePositions) = 0;

    /// Map the Term Vector information into your own structure
    /// @param term The term to add to the vector
    /// @param frequency The frequency of the term in the document
    /// @param offsets null if the offset is not specified, otherwise the offset into the field of the term
    /// @param positions null if the position is not specified, otherwise the position in the field of the term
    virtual void map(const String& term, int32_t frequency, Collection<TermVectorOffsetInfoPtr> offsets, Collection<int32_t> positions) = 0;

    /// Indicate to Lucene that even if there are positions stored, this mapper is not interested in them and
    /// they can be skipped over.  Derived classes should set this to true if they want to ignore positions.
    /// The default is false, meaning positions will be loaded if they are stored.
    virtual bool isIgnoringPositions();

    /// @see #isIgnoringPositions() Same principal as {@link #isIgnoringPositions()}, but applied to offsets.
    virtual bool isIgnoringOffsets();

    /// Passes down the index of the document whose term vector is currently being mapped, once for each top
    /// level call to a term vector reader.
    ///
    /// Default implementation IGNORES the document number.  Override if your implementation needs the document
    /// number.
    ///
    /// NOTE: Document numbers are internal to Lucene and subject to change depending on indexing operations.
    ///
    /// @param documentNumber index of document currently being mapped
    virtual void setDocumentNumber(int32_t documentNumber);
};

}

#endif
