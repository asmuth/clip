/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef SEGMENTMERGER_H
#define SEGMENTMERGER_H

#include "LuceneObject.h"

namespace Lucene {

/// The SegmentMerger class combines two or more Segments, represented by an IndexReader ({@link #add}, into a single
/// Segment.  After adding the appropriate readers, call the merge method to combine the segments.
///
/// If the compoundFile flag is set, then the segments will be merged into a compound file.
/// @see #merge
/// @see #add
class SegmentMerger : public LuceneObject {
public:
    SegmentMerger(const DirectoryPtr& dir, const String& name);
    SegmentMerger(const IndexWriterPtr& writer, const String& name, const OneMergePtr& merge);
    virtual ~SegmentMerger();

    LUCENE_CLASS(SegmentMerger);

protected:
    DirectoryPtr directory;
    String segment;
    int32_t termIndexInterval;

    Collection<IndexReaderPtr> readers;
    FieldInfosPtr fieldInfos;

    int32_t mergedDocs;
    CheckAbortPtr checkAbort;

    /// Whether we should merge doc stores (stored fields and vectors files).  When all segments we
    /// are merging already share the same doc store files, we don't need to merge the doc stores.
    bool mergeDocStores;

    /// Maximum number of contiguous documents to bulk-copy when merging stored fields
    static const int32_t MAX_RAW_MERGE_DOCS;

    Collection<SegmentReaderPtr> matchingSegmentReaders;
    Collection<int32_t> rawDocLengths;
    Collection<int32_t> rawDocLengths2;

    SegmentMergeQueuePtr queue;
    bool omitTermFreqAndPositions;

    ByteArray payloadBuffer;
    Collection< Collection<int32_t> > docMaps;
    Collection<int32_t> delCounts;

public:
    /// norms header placeholder
    static const uint8_t NORMS_HEADER[];
    static const int32_t NORMS_HEADER_LENGTH;

public:
    bool hasProx();

    /// Add an IndexReader to the collection of readers that are to be merged
    void add(const IndexReaderPtr& reader);

    /// @param i The index of the reader to return
    /// @return The i'th reader to be merged
    IndexReaderPtr segmentReader(int32_t i);

    /// Merges the readers specified by the {@link #add} method into the directory passed to the constructor.
    /// @return The number of documents that were merged
    int32_t merge();

    /// Merges the readers specified by the {@link #add} method into the directory passed to the constructor.
    /// @param mergeDocStores if false, we will not merge the stored fields nor vectors files
    /// @return The number of documents that were merged
    int32_t merge(bool mergeDocStores);

    /// close all IndexReaders that have been added. Should not be called before merge().
    void closeReaders();

    HashSet<String> getMergedFiles();
    HashSet<String> createCompoundFile(const String& fileName);

    /// @return The number of documents in all of the readers
    int32_t mergeFields();

    Collection< Collection<int32_t> > getDocMaps();
    Collection<int32_t> getDelCounts();

protected:
    void addIndexed(const IndexReaderPtr& reader, const FieldInfosPtr& fInfos, HashSet<String> names, bool storeTermVectors,
                    bool storePositionWithTermVector, bool storeOffsetWithTermVector, bool storePayloads,
                    bool omitTFAndPositions);

    void setMatchingSegmentReaders();
    int32_t copyFieldsWithDeletions(const FieldsWriterPtr& fieldsWriter, const IndexReaderPtr& reader, const FieldsReaderPtr& matchingFieldsReader);
    int32_t copyFieldsNoDeletions(const FieldsWriterPtr& fieldsWriter, const IndexReaderPtr& reader, const FieldsReaderPtr& matchingFieldsReader);

    /// Merge the TermVectors from each of the segments into the new one.
    void mergeVectors();

    void copyVectorsWithDeletions(const TermVectorsWriterPtr& termVectorsWriter, const TermVectorsReaderPtr& matchingVectorsReader, const IndexReaderPtr& reader);
    void copyVectorsNoDeletions(const TermVectorsWriterPtr& termVectorsWriter, const TermVectorsReaderPtr& matchingVectorsReader, const IndexReaderPtr& reader);

    void mergeTerms();

    void mergeTermInfos(const FormatPostingsFieldsConsumerPtr& consumer);

    /// Process postings from multiple segments all positioned on the same term. Writes out merged entries
    /// into freqOutput and the proxOutput streams.
    /// @param smis array of segments
    /// @param n number of cells in the array actually occupied
    /// @return number of documents across all segments where this term was found
    int32_t appendPostings(const FormatPostingsTermsConsumerPtr& termsConsumer, Collection<SegmentMergeInfoPtr> smis, int32_t n);

    void mergeNorms();
};

class CheckAbort : public LuceneObject {
public:
    CheckAbort(const OneMergePtr& merge, const DirectoryPtr& dir);
    virtual ~CheckAbort();

    LUCENE_CLASS(CheckAbort);

protected:
    double workCount;
    OneMergePtr merge;
    DirectoryWeakPtr _dir;

public:
    /// Records the fact that roughly units amount of work have been done since this method was last called.
    /// When adding time-consuming code into SegmentMerger, you should test different values for units to
    /// ensure that the time in between calls to merge.checkAborted is up to ~ 1 second.
    virtual void work(double units);
};

class CheckAbortNull : public CheckAbort {
public:
    CheckAbortNull();
    virtual ~CheckAbortNull();

    LUCENE_CLASS(CheckAbortNull);

public:
    /// do nothing
    virtual void work(double units);
};

}

#endif
