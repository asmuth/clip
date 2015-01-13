/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef DIRECTORYREADER_H
#define DIRECTORYREADER_H

#include "IndexReader.h"
#include "TermEnum.h"
#include "TermPositions.h"
#include "IndexCommit.h"
#include "SegmentMergeQueue.h"

namespace Lucene {

/// An IndexReader which reads indexes with multiple segments.
class DirectoryReader : public IndexReader {
public:
    /// Construct reading the named set of readers.
    DirectoryReader(const DirectoryPtr& directory, const SegmentInfosPtr& sis, const IndexDeletionPolicyPtr& deletionPolicy, bool readOnly, int32_t termInfosIndexDivisor);

    /// Used by near real-time search.
    DirectoryReader(const IndexWriterPtr& writer, const SegmentInfosPtr& infos, int32_t termInfosIndexDivisor);

    /// This constructor is only used for {@link #reopen()}
    DirectoryReader(const DirectoryPtr& directory, const SegmentInfosPtr& infos, Collection<SegmentReaderPtr> oldReaders,
                    Collection<int32_t> oldStarts, MapStringByteArray oldNormsCache, bool readOnly,
                    bool doClone, int32_t termInfosIndexDivisor);

    virtual ~DirectoryReader();

    LUCENE_CLASS(DirectoryReader);

protected:
    DirectoryPtr _directory;
    bool readOnly;
    IndexWriterWeakPtr _writer;
    IndexDeletionPolicyPtr deletionPolicy;
    HashSet<String> synced;
    LockPtr writeLock;
    SegmentInfosPtr segmentInfos;
    SegmentInfosPtr segmentInfosStart;
    bool stale;
    int32_t termInfosIndexDivisor;

    bool rollbackHasChanges;

    Collection<SegmentReaderPtr> subReaders;
    Collection<int32_t> starts; // 1st docno for each segment
    MapStringByteArray normsCache;
    int32_t _maxDoc;
    int32_t _numDocs;
    bool _hasDeletions;

    // Max version in index as of when we opened; this can be > our current segmentInfos version
    // in case we were opened on a past IndexCommit
    int64_t maxIndexVersion;

public:
    void _initialize(Collection<SegmentReaderPtr> subReaders);

    static IndexReaderPtr open(const DirectoryPtr& directory, const IndexDeletionPolicyPtr& deletionPolicy, const IndexCommitPtr& commit, bool readOnly, int32_t termInfosIndexDivisor);

    virtual LuceneObjectPtr clone(const LuceneObjectPtr& other = LuceneObjectPtr());
    virtual LuceneObjectPtr clone(bool openReadOnly, const LuceneObjectPtr& other = LuceneObjectPtr());

    virtual IndexReaderPtr reopen();
    virtual IndexReaderPtr reopen(bool openReadOnly);
    virtual IndexReaderPtr reopen(const IndexCommitPtr& commit);

    /// Version number when this IndexReader was opened.
    virtual int64_t getVersion();

    /// Return an array of term frequency vectors for the specified document.
    virtual Collection<TermFreqVectorPtr> getTermFreqVectors(int32_t docNumber);

    /// Return a term frequency vector for the specified document and field.
    virtual TermFreqVectorPtr getTermFreqVector(int32_t docNumber, const String& field);

    /// Load the Term Vector into a user-defined data structure instead of relying on the parallel arrays of the {@link TermFreqVector}.
    virtual void getTermFreqVector(int32_t docNumber, const String& field, const TermVectorMapperPtr& mapper);

    /// Map all the term vectors for all fields in a Document
    virtual void getTermFreqVector(int32_t docNumber, const TermVectorMapperPtr& mapper);

    /// Checks is the index is optimized (if it has a single segment and no deletions).  Not implemented in the IndexReader base class.
    /// @return true if the index is optimized; false otherwise
    virtual bool isOptimized();

    /// Returns the number of documents in this index.
    virtual int32_t numDocs();

    /// Returns one greater than the largest possible document number.
    virtual int32_t maxDoc();

    /// Get the {@link Document} at the n'th position. The {@link FieldSelector} may be used to determine what {@link Field}s to load and how they should be loaded.
    virtual DocumentPtr document(int32_t n, const FieldSelectorPtr& fieldSelector);

    /// Returns true if document n has been deleted
    virtual bool isDeleted(int32_t n);

    /// Returns true if any documents have been deleted
    virtual bool hasDeletions();

    /// Find reader for doc n
    static int32_t readerIndex(int32_t n, Collection<int32_t> starts, int32_t numSubReaders);

    /// Returns true if there are norms stored for this field.
    virtual bool hasNorms(const String& field);

    /// Returns the byte-encoded normalization factor for the named field of every document.
    virtual ByteArray norms(const String& field);

    /// Reads the byte-encoded normalization factor for the named field of every document.
    virtual void norms(const String& field, ByteArray norms, int32_t offset);

    /// Returns an enumeration of all the terms in the index.
    virtual TermEnumPtr terms();

    /// Returns an enumeration of all terms starting at a given term.
    virtual TermEnumPtr terms(const TermPtr& t);

    /// Returns the number of documents containing the term t.
    virtual int32_t docFreq(const TermPtr& t);

    /// Returns an unpositioned {@link TermDocs} enumerator.
    virtual TermDocsPtr termDocs();

    /// Returns an unpositioned {@link TermPositions} enumerator.
    virtual TermPositionsPtr termPositions();

    /// Tries to acquire the WriteLock on this directory. this method is only valid if this
    /// IndexReader is directory owner.
    virtual void acquireWriteLock();

    void startCommit();
    void rollbackCommit();

    /// Retrieve the String userData optionally passed to IndexWriter#commit.
    virtual MapStringString getCommitUserData();

    /// Check whether any new changes have occurred to the index since this reader was opened.
    virtual bool isCurrent();

    /// Get a list of unique field names that exist in this index and have the specified field
    /// option information.
    virtual HashSet<String> getFieldNames(FieldOption fieldOption);

    static HashSet<String> getFieldNames(FieldOption fieldOption, Collection<IndexReaderPtr> subReaders);

    /// Returns the sequential sub readers that this reader is logically composed of.
    virtual Collection<IndexReaderPtr> getSequentialSubReaders();

    /// Returns the directory this index resides in.
    virtual DirectoryPtr directory();

    virtual int32_t getTermInfosIndexDivisor();

    /// Return the IndexCommit that this reader has opened.
    virtual IndexCommitPtr getIndexCommit();

    /// Returns all commit points that exist in the Directory.
    static Collection<IndexCommitPtr> listCommits(const DirectoryPtr& dir);

protected:
    IndexReaderPtr doReopenFromWriter(bool openReadOnly, const IndexCommitPtr& commit);
    IndexReaderPtr doReopen(bool openReadOnly, const IndexCommitPtr& commit);
    IndexReaderPtr doReopenNoWriter(bool openReadOnly, const IndexCommitPtr& commit);
    DirectoryReaderPtr doReopen(const SegmentInfosPtr& infos, bool doClone, bool openReadOnly);

    /// Implements deletion of the document numbered docNum.
    virtual void doDelete(int32_t docNum);

    /// Implements actual undeleteAll() in subclass.
    virtual void doUndeleteAll();

    int32_t readerIndex(int32_t n);

    /// Implements setNorm in subclass.
    virtual void doSetNorm(int32_t doc, const String& field, uint8_t value);

    /// Commit changes resulting from delete, undeleteAll, or setNorm operations
    ///
    /// If an exception is hit, then either no changes or all changes will have been committed to the index (transactional semantics).
    virtual void doCommit(MapStringString commitUserData);

    /// Implements close.
    virtual void doClose();

    friend class FindSegmentsReopen;
};

class MultiTermEnum : public TermEnum {
public:
    MultiTermEnum(const IndexReaderPtr& topReader, Collection<IndexReaderPtr> readers, Collection<int32_t> starts, const TermPtr& t);
    virtual ~MultiTermEnum();

    LUCENE_CLASS(MultiTermEnum);

protected:
    SegmentMergeQueuePtr queue;
    TermPtr _term;
    int32_t _docFreq;

public:
    IndexReaderWeakPtr _topReader;
    Collection<SegmentMergeInfoPtr> matchingSegments; // null terminated array of matching segments

public:
    /// Increments the enumeration to the next element.  True if one exists.
    virtual bool next();

    /// Returns the current Term in the enumeration.
    virtual TermPtr term();

    /// Returns the docFreq of the current Term in the enumeration.
    virtual int32_t docFreq();

    /// Closes the enumeration to further activity, freeing resources.
    virtual void close();
};

class MultiTermDocs : public TermPositions, public LuceneObject {
public:
    MultiTermDocs(const IndexReaderPtr& topReader, Collection<IndexReaderPtr> r, Collection<int32_t> s);
    virtual ~MultiTermDocs();

    LUCENE_CLASS(MultiTermDocs);

protected:
    IndexReaderWeakPtr _topReader; // used for matching TermEnum to TermDocs
    Collection<IndexReaderPtr> readers;
    Collection<int32_t> starts;
    TermPtr term;

    int32_t base;
    int32_t pointer;

    Collection<TermDocsPtr> readerTermDocs;
    TermDocsPtr current;
    MultiTermEnumPtr tenum; // the term enum used for seeking
    int32_t matchingSegmentPos; // position into the matching segments from tenum
    SegmentMergeInfoPtr smi; // current segment mere info

public:
    /// Returns the current document number.
    virtual int32_t doc();

    /// Returns the frequency of the term within the current document.
    virtual int32_t freq();

    /// Sets this to the data for a term.
    virtual void seek(const TermPtr& term);

    /// Sets this to the data for the current term in a {@link TermEnum}.
    virtual void seek(const TermEnumPtr& termEnum);

    /// Moves to the next pair in the enumeration.
    virtual bool next();

    /// Attempts to read multiple entries from the enumeration, up to length of docs.
    /// Optimized implementation.
    virtual int32_t read(Collection<int32_t> docs, Collection<int32_t> freqs);

    /// Skips entries to the first beyond the current whose document number is greater than or equal to target.
    virtual bool skipTo(int32_t target);

    /// Frees associated resources.
    virtual void close();

protected:
    virtual TermDocsPtr termDocs(int32_t i);
    virtual TermDocsPtr termDocs(const IndexReaderPtr& reader);
};

class MultiTermPositions : public MultiTermDocs {
public:
    MultiTermPositions(const IndexReaderPtr& topReader, Collection<IndexReaderPtr> r, Collection<int32_t> s);
    virtual ~MultiTermPositions();

    LUCENE_CLASS(MultiTermPositions);

public:
    /// Returns next position in the current document.
    virtual int32_t nextPosition();

    /// Returns the length of the payload at the current term position.
    virtual int32_t getPayloadLength();

    /// Returns the payload data at the current term position.
    virtual ByteArray getPayload(ByteArray data, int32_t offset);

    /// Checks if a payload can be loaded at this position.
    virtual bool isPayloadAvailable();

protected:
    virtual TermDocsPtr termDocs(const IndexReaderPtr& reader);
};

class ReaderCommit : public IndexCommit {
public:
    ReaderCommit(const SegmentInfosPtr& infos, const DirectoryPtr& dir);
    virtual ~ReaderCommit();

    LUCENE_CLASS(ReaderCommit);

protected:
    String segmentsFileName;
    HashSet<String> files;
    DirectoryPtr dir;
    int64_t generation;
    int64_t version;
    bool _isOptimized;
    MapStringString userData;

public:
    virtual String toString();

    /// Returns true if this commit is an optimized index.
    virtual bool isOptimized();

    /// Two IndexCommits are equal if both their Directory and versions are equal.
    virtual String getSegmentsFileName();

    /// Returns all index files referenced by this commit point.
    virtual HashSet<String> getFileNames();

    /// Returns the {@link Directory} for the index.
    virtual DirectoryPtr getDirectory();

    /// Returns the version for this IndexCommit.
    virtual int64_t getVersion();

    /// Returns the generation (the _N in segments_N) for this IndexCommit.
    virtual int64_t getGeneration();

    virtual bool isDeleted();

    /// Returns userData, previously passed to {@link IndexWriter#commit(Map)} for this commit.
    virtual MapStringString getUserData();

    virtual void deleteCommit();
};

}

#endif
