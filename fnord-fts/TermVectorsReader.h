/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef TERMVECTORSREADER_H
#define TERMVECTORSREADER_H

#include "TermVectorMapper.h"

namespace Lucene {

class TermVectorsReader : public LuceneObject {
public:
    TermVectorsReader();
    TermVectorsReader(const DirectoryPtr& d, const String& segment, const FieldInfosPtr& fieldInfos);
    TermVectorsReader(const DirectoryPtr& d, const String& segment, const FieldInfosPtr& fieldInfos,
                      int32_t readBufferSize, int32_t docStoreOffset = -1, int32_t size = 0);
    virtual ~TermVectorsReader();

    LUCENE_CLASS(TermVectorsReader);

public:
    /// NOTE: if you make a new format, it must be larger than the current format
    static const int32_t FORMAT_VERSION;

    /// Changes to speed up bulk merging of term vectors
    static const int32_t FORMAT_VERSION2;

    /// Changed strings to UTF8 with length-in-bytes not length-in-chars
    static const int32_t FORMAT_UTF8_LENGTH_IN_BYTES;

    /// NOTE: always change this if you switch to a new format.
    static const int32_t FORMAT_CURRENT;

    /// The size in bytes that the FORMAT_VERSION will take up at the beginning of each file
    static const int32_t FORMAT_SIZE;

    static const uint8_t STORE_POSITIONS_WITH_TERMVECTOR;
    static const uint8_t STORE_OFFSET_WITH_TERMVECTOR;

protected:
    FieldInfosPtr fieldInfos;

    IndexInputPtr tvx;
    IndexInputPtr tvd;
    IndexInputPtr tvf;
    int32_t _size;
    int32_t numTotalDocs;

    /// The docID offset where our docs begin in the index file.  This will be 0 if we have our own private file.
    int32_t docStoreOffset;

    int32_t format;

public:
    /// Used for bulk copy when merging
    IndexInputPtr getTvdStream();

    /// Used for bulk copy when merging
    IndexInputPtr getTvfStream();

    bool canReadRawDocs();

    /// Retrieve the length (in bytes) of the tvd and tvf entries for the next numDocs starting with
    /// startDocID.  This is used for bulk copying when merging segments, if the field numbers are
    /// congruent.  Once this returns, the tvf & tvd streams are seeked to the startDocID.
    void rawDocs(Collection<int32_t> tvdLengths, Collection<int32_t> tvfLengths, int32_t startDocID, int32_t numDocs);

    void close();

    /// @return The number of documents in the reader
    int32_t size();

    void get(int32_t docNum, const String& field, const TermVectorMapperPtr& mapper);

    /// Retrieve the term vector for the given document and field
    /// @param docNum The document number to retrieve the vector for
    /// @param field The field within the document to retrieve
    /// @return The TermFreqVector for the document and field or null if there is no termVector for
    /// this field.
    TermFreqVectorPtr get(int32_t docNum, const String& field);

    /// Return all term vectors stored for this document or null if the could not be read in.
    ///
    /// @param docNum The document number to retrieve the vector for
    /// @return All term frequency vectors
    Collection<TermFreqVectorPtr> get(int32_t docNum);

    void get(int32_t docNumber, const TermVectorMapperPtr& mapper);

    virtual LuceneObjectPtr clone(const LuceneObjectPtr& other = LuceneObjectPtr());

protected:
    void ConstructReader(const DirectoryPtr& d, const String& segment, const FieldInfosPtr& fieldInfos, int32_t readBufferSize, int32_t docStoreOffset, int32_t size);

    void seekTvx(int32_t docNum);

    int32_t checkValidFormat(const IndexInputPtr& in);

    /// Reads the String[] fields; you have to pre-seek tvd to the right point
    Collection<String> readFields(int32_t fieldCount);

    /// Reads the long[] offsets into TVF; you have to pre-seek tvx/tvd to the right point
    Collection<int64_t> readTvfPointers(int32_t fieldCount);

    Collection<TermFreqVectorPtr> readTermVectors(int32_t docNum, Collection<String> fields, Collection<int64_t> tvfPointers);
    void readTermVectors(Collection<String> fields, Collection<int64_t> tvfPointers, const TermVectorMapperPtr& mapper);

    /// @param field The field to read in
    /// @param tvfPointer The pointer within the tvf file where we should start reading
    /// @param mapper The mapper used to map the TermVector
    void readTermVector(const String& field, int64_t tvfPointer, const TermVectorMapperPtr& mapper);
};

/// Models the existing parallel array structure
class ParallelArrayTermVectorMapper : public TermVectorMapper {
public:
    ParallelArrayTermVectorMapper();
    virtual ~ParallelArrayTermVectorMapper();

    LUCENE_CLASS(ParallelArrayTermVectorMapper);

protected:
    Collection<String> terms;
    Collection<int32_t> termFreqs;
    Collection< Collection<int32_t> > positions;
    Collection< Collection<TermVectorOffsetInfoPtr> > offsets;
    int32_t currentPosition;
    bool storingOffsets;
    bool storingPositions;
    String field;

public:
    /// Tell the mapper what to expect in regards to field, number of terms, offset and position storage.
    /// This method will be called once before retrieving the vector for a field.
    virtual void setExpectations(const String& field, int32_t numTerms, bool storeOffsets, bool storePositions);

    /// Map the Term Vector information into your own structure
    virtual void map(const String& term, int32_t frequency, Collection<TermVectorOffsetInfoPtr> offsets, Collection<int32_t> positions);

    /// Construct the vector
    /// @return The {@link TermFreqVector} based on the mappings.
    TermFreqVectorPtr materializeVector();
};

}

#endif
