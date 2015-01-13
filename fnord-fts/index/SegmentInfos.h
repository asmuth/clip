/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef SEGMENTINFOS_H
#define SEGMENTINFOS_H

#include "fnord-fts/index/SegmentInfoCollection.h"

namespace Lucene {

/// A collection of SegmentInfo objects with methods for operating on those segments in relation to the file system.
class SegmentInfos : public SegmentInfoCollection {
public:
    SegmentInfos();
    virtual ~SegmentInfos();

    LUCENE_CLASS(SegmentInfos);

public:
    /// The file format version, a negative number.  Works since counter, the old 1st entry, is always >= 0
    static const int32_t FORMAT;

    /// This format adds details used for lockless commits.  It differs slightly from the previous format in that file names
    /// are never re-used (write once).  Instead, each file is written to the next generation.  For example, segments_1,
    /// segments_2, etc.  This allows us to not use a commit lock.
    /// See <a href="http://lucene.apache.org/java/docs/fileformats.html">fileformats</a> for details.
    static const int32_t FORMAT_LOCKLESS;

    /// This format adds a "hasSingleNormFile" flag into each segment info.
    static const int32_t FORMAT_SINGLE_NORM_FILE;

    /// This format allows multiple segments to share a single vectors and stored fields file.
    static const int32_t FORMAT_SHARED_DOC_STORE;

    /// This format adds a checksum at the end of the file to ensure all bytes were successfully written.
    static const int32_t FORMAT_CHECKSUM;

    /// This format adds the deletion count for each segment. This way IndexWriter can efficiently report numDocs().
    static const int32_t FORMAT_DEL_COUNT;

    /// This format adds the boolean hasProx to record if any fields in the segment store prox information (ie, have
    /// omitTermFreqAndPositions == false)
    static const int32_t FORMAT_HAS_PROX;

    /// This format adds optional commit userData storage.
    static const int32_t FORMAT_USER_DATA;

    /// This format adds optional per-segment string diagnostics storage, and switches userData to Map
    static const int32_t FORMAT_DIAGNOSTICS;

    /// This must always point to the most recent file format.
    static const int32_t CURRENT_FORMAT;

    int32_t counter; // used to name new segments

private:
    /// Advanced configuration of retry logic in loading segments_N file.
    static int32_t defaultGenFileRetryCount;
    static int32_t defaultGenFileRetryPauseMsec;
    static int32_t defaultGenLookaheadCount;

    /// Counts how often the index has been changed by adding or deleting docs.
    /// Starting with the current time in milliseconds forces to create unique version numbers.
    int64_t version;

    int64_t generation; // generation of the "segments_N" for the next commit

    int64_t lastGeneration; // generation of the "segments_N" file we last successfully read
    // or wrote; this is normally the same as generation except if
    // there was an exception that had interrupted a commit

    MapStringString userData; // Opaque map<string, string> that user can specify during IndexWriter::commit

    static MapStringString singletonUserData;

    static InfoStreamPtr infoStream;
    ChecksumIndexOutputPtr pendingSegnOutput;

public:
    SegmentInfoPtr info(int32_t i);
    String getCurrentSegmentFileName();
    String getNextSegmentFileName();

    /// Read a particular segmentFileName.  Note that this may throw an IOException if a commit is in process.
    void read(const DirectoryPtr& directory, const String& segmentFileName);

    /// This version of read uses the retry logic (for lock-less commits) to find the right segments file to load.
    void read(const DirectoryPtr& directory);

    /// Returns a copy of this instance, also copying each SegmentInfo.
    virtual LuceneObjectPtr clone(const LuceneObjectPtr& other = LuceneObjectPtr());

    /// Version number when this SegmentInfos was generated.
    int64_t getVersion();
    int64_t getGeneration();
    int64_t getLastGeneration();

    /// Returns a new SegmentInfos containing the SegmentInfo instances in the specified range first (inclusive) to
    /// last (exclusive), so total number of segments returned is last-first.
    SegmentInfosPtr range(int32_t first, int32_t last);

    /// Carry over generation numbers from another SegmentInfos.
    void updateGeneration(const SegmentInfosPtr& other);

    void rollbackCommit(const DirectoryPtr& dir);

    /// Call this to start a commit.  This writes the new segments file, but writes an invalid checksum at the end, so
    /// that it is not visible to readers.  Once this is called you must call.
    /// {@link #finishCommit} to complete the commit or
    /// {@link #rollbackCommit} to abort it.
    void prepareCommit(const DirectoryPtr& dir);

    /// Returns all file names referenced by SegmentInfo instances matching the provided Directory (ie files associated
    /// with any "external" segments are skipped). The returned collection is recomputed on each invocation.
    HashSet<String> files(const DirectoryPtr& dir, bool includeSegmentsFile);

    void finishCommit(const DirectoryPtr& dir);

    /// Writes & syncs to the Directory dir, taking care to remove the segments file on exception.
    void commit(const DirectoryPtr& dir);

    String segString(const DirectoryPtr& directory);
    MapStringString getUserData();
    void setUserData(MapStringString data);

    /// Replaces all segments in this instance, but keeps generation, version, counter so that future commits remain
    /// write once.
    void replace(const SegmentInfosPtr& other);

    bool hasExternalSegments(const DirectoryPtr& dir);

    static int64_t getCurrentSegmentGeneration(HashSet<String> files);
    static int64_t getCurrentSegmentGeneration(const DirectoryPtr& directory);
    static String getCurrentSegmentFileName(HashSet<String> files);
    static String getCurrentSegmentFileName(const DirectoryPtr& directory);
    static int64_t generationFromSegmentsFileName(const String& fileName);

    /// Current version number from segments file.
    static int64_t readCurrentVersion(const DirectoryPtr& directory);

    /// Returns userData from latest segments file.
    static MapStringString readCurrentUserData(const DirectoryPtr& directory);

    /// If non-null, information about retries when loading the segments file will be printed to this.
    static void setInfoStream(const InfoStreamPtr& infoStream);

    /// Set how many times to try loading the segments.gen file contents to determine current segment generation.  This file
    /// is only referenced when the primary method (listing the directory) fails.
    static void setDefaultGenFileRetryCount(int32_t count);

    /// @see #setDefaultGenFileRetryCount
    static int32_t getDefaultGenFileRetryCount();

    /// Set how many milliseconds to pause in between attempts to load the segments.gen file.
    static void setDefaultGenFileRetryPauseMsec(int32_t msec);

    /// @see #setDefaultGenFileRetryPauseMsec
    static int32_t getDefaultGenFileRetryPauseMsec();

    /// Set how many times to try incrementing the gen when loading the segments file.  This only runs if the primary
    /// (listing directory) and secondary (opening segments.gen file) methods fail to find the segments file.
    static void setDefaultGenLookaheadCount(int32_t count);

    /// @see #setDefaultGenLookaheadCount
    static int32_t getDefaultGenLookahedCount();

    /// @see #setInfoStream
    static InfoStreamPtr getInfoStream();

    static void message(const String& message);

protected:
    void write(const DirectoryPtr& directory);

    friend class FindSegmentsFile;
};

}

#endif
