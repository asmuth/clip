/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef CHECKINDEX_H
#define CHECKINDEX_H

#include "fnord-fts/index/SegmentTermDocs.h"

namespace Lucene {

/// Basic tool and API to check the health of an index and write a new segments file that removes reference to
/// problematic segments.
///
/// As this tool checks every byte in the index, on a large index it can take quite a long time to run.
///
/// WARNING: Please make a complete backup of your index before using this to fix your index!
class CheckIndex : public LuceneObject {
public:
    /// Create a new CheckIndex on the directory.
    CheckIndex(const DirectoryPtr& dir);
    virtual ~CheckIndex();

    LUCENE_CLASS(CheckIndex);

protected:
    InfoStreamPtr infoStream;
    DirectoryPtr dir;

    static bool _assertsOn;

public:
    /// Set infoStream where messages should go.  If null, no messages are printed
    void setInfoStream(const InfoStreamPtr& out);

    /// Returns a {@link IndexStatus} instance detailing the state of the index.
    ///
    /// As this method checks every byte in the index, on a large index it can take quite a long time to run.
    ///
    /// WARNING: make sure you only call this when the index is not opened by any writer.
    IndexStatusPtr checkIndex();

    /// Returns a {@link IndexStatus} instance detailing the state of the index.
    ///
    /// @param onlySegments list of specific segment names to check
    ///
    /// As this method checks every byte in the specified segments, on a large index it can take quite a long
    /// time to run.
    ///
    /// WARNING: make sure you only call this when the index is not opened by any writer.
    IndexStatusPtr checkIndex(Collection<String> onlySegments);

    /// Repairs the index using previously returned result from {@link #checkIndex}.  Note that this does not
    /// remove any of the unreferenced files after it's done; you must separately open an {@link IndexWriter},
    /// which deletes unreferenced files when it's created.
    ///
    /// WARNING: this writes a new segments file into the index, effectively removing all documents in broken
    /// segments from the index. BE CAREFUL.
    ///
    /// WARNING: Make sure you only call this when the index is not opened  by any writer.
    void fixIndex(const IndexStatusPtr& result);

    static bool testAsserts();
    static bool assertsOn();

    /// Command-line interface to check and fix an index.
    ///
    /// Run it like this:
    /// CheckIndex pathToIndex [-fix] [-segment X] [-segment Y]
    ///
    /// -fix: actually write a new segments_N file, removing any problematic segments
    ///
    /// -segment X: only check the specified segment(s).  This can be specified multiple times,
    ///             to check more than one segment, eg -segment _2 -segment _a.
    ///             You can't use this with the -fix option.
    ///
    /// WARNING: -fix should only be used on an emergency basis as it will cause documents (perhaps many)
    /// to be permanently removed from the index.  Always make a backup copy of your index before running
    /// this!  Do not run this tool on an index that is actively being written to.  You have been warned!
    ///
    /// Run without -fix, this tool will open the index, report version information and report any exceptions
    /// it hits and what action it would take if -fix were specified.  With -fix, this tool will remove any
    /// segments that have issues and write a new segments_N file.  This means all documents contained in the
    /// affected segments will be removed.
    ///
    /// This tool exits with exit code 1 if the index cannot be opened or has any corruption, else 0.
    static int main(Collection<String> args);

protected:
    void msg(const String& msg);

    /// Test field norms.
    FieldNormStatusPtr testFieldNorms(Collection<String> fieldNames, const SegmentReaderPtr& reader);

    /// Test the term index.
    TermIndexStatusPtr testTermIndex(const SegmentInfoPtr& info, const SegmentReaderPtr& reader);

    /// Test stored fields for a segment.
    StoredFieldStatusPtr testStoredFields(const SegmentInfoPtr& info, const SegmentReaderPtr& reader);

    /// Test term vectors for a segment.
    TermVectorStatusPtr testTermVectors(const SegmentInfoPtr& info, const SegmentReaderPtr& reader);
};

/// Returned from {@link #checkIndex()} detailing the health and status of the index.
class IndexStatus : public LuceneObject {
public:
    IndexStatus();
    virtual ~IndexStatus();

    LUCENE_CLASS(IndexStatus);

public:
    /// True if no problems were found with the index.
    bool clean;

    /// True if we were unable to locate and load the segments_N file.
    bool missingSegments;

    /// True if we were unable to open the segments_N file.
    bool cantOpenSegments;

    /// True if we were unable to read the version number from segments_N file.
    bool missingSegmentVersion;

    /// Name of latest segments_N file in the index.
    String segmentsFileName;

    /// Number of segments in the index.
    int32_t numSegments;

    /// String description of the version of the index.
    String segmentFormat;

    /// Empty unless you passed specific segments list to check as optional 3rd argument.
    /// @see CheckIndex#checkIndex(List)
    Collection<String> segmentsChecked;

    /// True if the index was created with a newer version of Lucene than the CheckIndex tool.
    bool toolOutOfDate;

    /// List of {@link SegmentInfoStatus} instances, detailing status of each segment.
    Collection<SegmentInfoStatusPtr> segmentInfos;

    /// Directory index is in.
    DirectoryPtr dir;

    /// SegmentInfos instance containing only segments that had no problems (this is used with the
    /// {@link CheckIndex#fixIndex} method to repair the index.
    SegmentInfosPtr newSegments;

    /// How many documents will be lost to bad segments.
    int32_t totLoseDocCount;

    /// How many bad segments were found.
    int32_t numBadSegments;

    /// True if we checked only specific segments ({@link #checkIndex(List)}) was called with non-null argument).
    bool partial;

    /// Holds the userData of the last commit in the index
    MapStringString userData;
};

/// Holds the status of each segment in the index. See {@link #segmentInfos}.
class SegmentInfoStatus : public LuceneObject {
public:
    SegmentInfoStatus();
    virtual ~SegmentInfoStatus();

    LUCENE_CLASS(SegmentInfoStatus);

public:
    /// Name of the segment.
    String name;

    /// Document count (does not take deletions into account).
    int32_t docCount;

    /// True if segment is compound file format.
    bool compound;

    /// Number of files referenced by this segment.
    int32_t numFiles;

    /// Net size (MB) of the files referenced by this segment.
    double sizeMB;

    /// Doc store offset, if this segment shares the doc store files (stored fields and term vectors) with
    /// other segments.  This is -1 if it does not share.
    int32_t docStoreOffset;

    /// String of the shared doc store segment, or null if this segment does not share the doc store files.
    String docStoreSegment;

    /// True if the shared doc store files are compound file format.
    bool docStoreCompoundFile;

    /// True if this segment has pending deletions.
    bool hasDeletions;

    /// Name of the current deletions file name.
    String deletionsFileName;

    /// Number of deleted documents.
    int32_t numDeleted;

    /// True if we were able to open a SegmentReader on this segment.
    bool openReaderPassed;

    /// Number of fields in this segment.
    int32_t numFields;

    /// True if at least one of the fields in this segment does not omitTermFreqAndPositions.
    /// @see AbstractField#setOmitTermFreqAndPositions
    bool hasProx;

    /// Map that includes certain debugging details that IndexWriter records into each segment it creates
    MapStringString diagnostics;

    /// Status for testing of field norms (null if field norms could not be tested).
    FieldNormStatusPtr fieldNormStatus;

    /// Status for testing of indexed terms (null if indexed terms could not be tested).
    TermIndexStatusPtr termIndexStatus;

    /// Status for testing of stored fields (null if stored fields could not be tested).
    StoredFieldStatusPtr storedFieldStatus;

    /// Status for testing of term vectors (null if term vectors could not be tested).
    TermVectorStatusPtr termVectorStatus;
};

/// Status from testing field norms.
class FieldNormStatus : public LuceneObject {
public:
    FieldNormStatus();
    virtual ~FieldNormStatus();

    LUCENE_CLASS(FieldNormStatus);

public:
    /// Number of fields successfully tested
    int64_t totFields;

    /// Exception thrown during term index test (null on success)
    LuceneException error;
};

/// Status from testing term index.
class TermIndexStatus : public LuceneObject {
public:
    TermIndexStatus();
    virtual ~TermIndexStatus();

    LUCENE_CLASS(TermIndexStatus);

public:
    /// Total term count
    int64_t termCount;

    /// Total frequency across all terms.
    int64_t totFreq;

    /// Total number of positions.
    int64_t totPos;

    /// Exception thrown during term index test (null on success)
    LuceneException error;
};

/// Status from testing stored fields.
class StoredFieldStatus : public LuceneObject {
public:
    StoredFieldStatus();
    virtual ~StoredFieldStatus();

    LUCENE_CLASS(StoredFieldStatus);

public:
    /// Number of documents tested.
    int32_t docCount;

    /// Total number of stored fields tested.
    int64_t totFields;

    /// Exception thrown during stored fields test (null on success)
    LuceneException error;
};

/// Status from testing stored fields.
class TermVectorStatus : public LuceneObject {
public:
    TermVectorStatus();
    virtual ~TermVectorStatus();

    LUCENE_CLASS(TermVectorStatus);

public:
    /// Number of documents tested.
    int32_t docCount;

    /// Total number of term vectors tested.
    int64_t totVectors;

    /// Exception thrown during term vector test (null on success)
    LuceneException error;
};

}

#endif
