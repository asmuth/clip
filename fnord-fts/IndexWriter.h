/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef INDEXWRITER_H
#define INDEXWRITER_H

#include "MergePolicy.h"

namespace Lucene {

/// An IndexWriter creates and maintains an index.
///
/// The create argument to the {@link #IndexWriter(DirectoryPtr, AnalyzerPtr, bool, int32_t) constructor}
/// determines whether a new index is created, or whether an existing index is opened.  Note that you can
/// open an index with create=true even while readers are using the index.  The old readers will continue
/// to search the "point in time" snapshot they had opened, and won't see the newly created index until
/// they re-open.  There are also {@link #IndexWriter(DirectoryPtr, AnalyzerPtr, int32_t) constructors}
/// with no create argument which will create a new index if there is not already an index at the provided
/// path and otherwise open the existing index.
///
/// In either case, documents are added with {@link #addDocument(DocumentPtr) addDocument} and removed
/// with {@link #deleteDocuments(TermPtr)} or {@link #deleteDocuments(QueryPtr)}. A document can be updated
/// with {@link #updateDocument(TermPtr, DocumentPtr) updateDocument} (which just deletes and then adds
/// the entire document). When finished adding, deleting and updating documents, {@link #close() close}
/// should be called.
///
/// These changes are buffered in memory and periodically flushed to the {@link Directory} (during the
/// above method calls).  A flush is triggered when there are enough buffered deletes (see
/// {@link #setMaxBufferedDeleteTerms}) or enough added documents since the last flush, whichever is
/// sooner.  For the added documents, flushing is triggered either by RAM usage of the documents (see
/// {@link #setRAMBufferSizeMB}) or the number of added documents. The default is to flush when RAM usage
/// hits 16 MB.  For best indexing speed you should flush by RAM usage with a large RAM buffer.  Note that
/// flushing just moves the internal buffered state in IndexWriter into the index, but these changes are
/// not visible to IndexReader until either {@link #commit()} or {@link #close} is called.  A flush may
/// also trigger one or more segment merges which by default run with a background thread so as not to
/// block the addDocument calls (see mergePolicy below for changing the {@link MergeScheduler}).
///
/// If an index will not have more documents added for a while and optimal search performance is desired,
/// then either the full {@link #optimize() optimize} method or partial {@link #optimize(int32_t)} method
/// should be called before the index is closed.
///
/// Opening an IndexWriter creates a lock file for the directory in use. Trying to open another IndexWriter
/// on the same directory will lead to a LockObtainFailed exception. The LockObtainFailed exception is also
/// thrown if an IndexReader on the same directory is used to delete documents from the index.
///
/// IndexWriter allows an optional {@link IndexDeletionPolicy} implementation to be specified.  You can use
/// this to control when prior commits are deleted from the index.  The default policy is {@link
/// KeepOnlyLastCommitDeletionPolicy} which removes all prior commits as soon as a new commit is done (this
/// matches behavior before 2.2).  Creating your own policy can allow you to explicitly keep previous
/// "point in time" commits alive in the index for some time, to allow readers to refresh to the new commit
/// without having the old commit deleted out from under them.  This is necessary on file systems like NFS
/// that do not support "delete on last close" semantics, which Lucene's "point in time" search normally
/// relies on.
///
/// IndexWriter allows you to separately change the {@link MergePolicy} and the {@link MergeScheduler}.
/// The {@link MergePolicy} is invoked whenever there are changes to the segments in the index.  Its role
/// is to select which merges to do, if any, and return a {@link MergePolicy.MergeSpecification} describing
/// the merges.  It also selects merges to do for optimize().  (The default is {@link LogByteSizeMergePolicy}.
/// Then, the {@link MergeScheduler} is invoked with the requested merges and it decides when and how to run
/// the merges.  The default is {@link ConcurrentMergeScheduler}.
///
/// NOTE: if you hit an std::bad_alloc then IndexWriter will quietly record this fact and block all future
/// segment commits.  This is a defensive measure in case any internal state (buffered documents and
/// deletions) were corrupted.  Any subsequent calls to {@link #commit()} will throw an IllegalState
/// exception.  The only course of action is to call {@link #close()}, which internally will call {@link
/// #rollback()}, to undo any changes to the index since the last commit.  You can also just call {@link
/// #rollback()} directly.
///
/// NOTE: {@link IndexWriter} instances are completely thread safe, meaning multiple threads can call any of
/// its methods, concurrently.  If your application requires external synchronization, you should not
/// synchronize on the IndexWriter instance as this may cause deadlock; use your own (non-Lucene) objects
/// instead.
///
/// Clarification: Check Points (and commits)
/// IndexWriter writes new index files to the directory without writing a new segments_N file which
/// references these new files. It also means that the state of the in memory SegmentInfos object is different
/// than the most recent segments_N file written to the directory.
///
/// Each time the SegmentInfos is changed, and matches the (possibly modified) directory files, we have a new
/// "check point".  If the modified/new SegmentInfos is written to disk - as a new (generation of) segments_N
/// file - this check point is also an IndexCommit.
///
/// A new checkpoint always replaces the previous checkpoint and becomes the new "front" of the index. This
/// allows the IndexFileDeleter to delete files that are referenced only by stale checkpoints (files that were
/// created since the last commit, but are no longer referenced by the "front" of the index). For this,
/// IndexFileDeleter keeps track of the last non commit checkpoint.
class IndexWriter : public LuceneObject {
protected:
    IndexWriter(const DirectoryPtr& d, const AnalyzerPtr& a, bool create, const IndexDeletionPolicyPtr& deletionPolicy, int32_t mfl, const IndexingChainPtr& indexingChain, const IndexCommitPtr& commit);

public:
    IndexWriter(const DirectoryPtr& d, const AnalyzerPtr& a, bool create, int32_t mfl);
    IndexWriter(const DirectoryPtr& d, const AnalyzerPtr& a, int32_t mfl);
    IndexWriter(const DirectoryPtr& d, const AnalyzerPtr& a, const IndexDeletionPolicyPtr& deletionPolicy, int32_t mfl);
    IndexWriter(const DirectoryPtr& d, const AnalyzerPtr& a, bool create, const IndexDeletionPolicyPtr& deletionPolicy, int32_t mfl);
    IndexWriter(const DirectoryPtr& d, const AnalyzerPtr& a, const IndexDeletionPolicyPtr& deletionPolicy, int32_t mfl, const IndexCommitPtr& commit);
    virtual ~IndexWriter();

    LUCENE_CLASS(IndexWriter);

protected:
    int64_t writeLockTimeout;

    /// The normal read buffer size defaults to 1024, but increasing this during merging seems to
    /// yield performance gains.  However we don't want to increase it too much because there are
    /// quite a few BufferedIndexInputs created during merging.
    static const int32_t MERGE_READ_BUFFER_SIZE;

    SynchronizePtr messageIDLock;
    static int32_t MESSAGE_ID;
    int32_t messageID;
    bool hitOOM;

    DirectoryPtr directory; // where this index resides
    AnalyzerPtr analyzer; // how to analyze text

    bool create;
    IndexDeletionPolicyPtr deletionPolicy;
    IndexingChainPtr indexingChain;
    IndexCommitPtr indexCommit;

    SimilarityPtr similarity; // how to normalize

    int64_t changeCount; // increments every time a change is completed
    int64_t lastCommitChangeCount; // last changeCount that was committed

    SegmentInfosPtr rollbackSegmentInfos; // segmentInfos we will fallback to if the commit fails
    MapSegmentInfoInt rollbackSegments;

    SegmentInfosPtr localRollbackSegmentInfos; // segmentInfos we will fallback to if the commit fails
    int32_t localFlushedDocCount;

    SegmentInfosPtr segmentInfos; // the segments

    DocumentsWriterPtr docWriter;
    IndexFileDeleterPtr deleter;

    SetSegmentInfo segmentsToOptimize; // used by optimize to note those needing optimization
    int32_t optimizeMaxNumSegments;

    LockPtr writeLock;

    int32_t termIndexInterval;

    bool closed;
    bool closing;

    SetSegmentInfo mergingSegments;
    MergePolicyPtr mergePolicy;
    MergeSchedulerPtr mergeScheduler;
    Collection<OneMergePtr> pendingMerges;
    SetOneMerge runningMerges;
    Collection<OneMergePtr> mergeExceptions;
    int64_t mergeGen;
    bool stopMerges;

    int32_t flushCount;
    int32_t flushDeletesCount;

    /// Used to only allow one addIndexes to proceed at once
    int32_t readCount; // count of how many threads are holding read lock
    int64_t writeThread; // non-null if any thread holds write lock
    int32_t upgradeCount;

    int32_t readerTermsIndexDivisor;

    // This is a "write once" variable (like the organic dye  on a DVD-R that may or may not
    // be heated by a laser and then cooled to permanently record the event): it's false,
    // until getReader() is called for the first time, at which point it's switched to true
    // and never changes back to false.  Once this is true, we hold open and reuse SegmentReader
    // instances internally for applying deletes, doing merges, and reopening near real-time readers.
    bool poolReaders;

    /// The maximum number of terms that will be indexed for a single field in a document.  This
    /// limits the amount of memory required for indexing, so that collections with very large files
    /// will not crash the indexing process by running out of memory.
    /// Note that this effectively truncates large documents, excluding from the index terms that
    /// occur further in the document.  If you know your source documents are large, be sure to set
    /// this value high enough to accommodate the expected size.  If you set it to INT_MAX, then the
    /// only limit is your memory, but you should anticipate an std::bad_alloc.  By default, no more
    /// than 10,000 terms will be indexed for a field.
    ///
    /// @see #setMaxFieldLength(int32_t)
    int32_t maxFieldLength;

    InfoStreamPtr infoStream;
    static InfoStreamPtr defaultInfoStream;

    HashSet<String> synced; // files that have been sync'd already
    HashSet<String> syncing; // files that are now being sync'd

    IndexReaderWarmerPtr mergedSegmentWarmer;

    /// Used only by commit; lock order is commitLock -> IW
    SynchronizePtr commitLock;

INTERNAL:
    SegmentInfosPtr pendingCommit; // set when a commit is pending (after prepareCommit() & before commit())
    int64_t pendingCommitChangeCount;

    ReaderPoolPtr readerPool;

public:
    /// Default value for the write lock timeout (1,000).
    /// @see #setDefaultWriteLockTimeout
    static int64_t WRITE_LOCK_TIMEOUT;

    static const String WRITE_LOCK_NAME;

    /// Value to denote a flush trigger is disabled.
    static const int32_t DISABLE_AUTO_FLUSH;

    /// Disabled by default (because IndexWriter flushes by RAM usage by default). Change using
    /// {@link #setMaxBufferedDocs(int32_t)}.
    static const int32_t DEFAULT_MAX_BUFFERED_DOCS;

    /// Default value is 16 MB (which means flush when buffered docs consume 16 MB RAM).
    /// Change using {@link #setRAMBufferSizeMB}.
    static const double DEFAULT_RAM_BUFFER_SIZE_MB;

    /// Disabled by default (because IndexWriter flushes by RAM usage by default). Change using
    /// {@link #setMaxBufferedDeleteTerms(int32_t)}.
    static const int32_t DEFAULT_MAX_BUFFERED_DELETE_TERMS;

    /// Default value is 10,000. Change using {@link #setMaxFieldLength(int32_t)}.
    static const int32_t DEFAULT_MAX_FIELD_LENGTH;

    /// Default value is 128. Change using {@link #setTermIndexInterval(int32_t)}.
    static const int32_t DEFAULT_TERM_INDEX_INTERVAL;

    /// Absolute hard maximum length for a term.  If a term arrives from the analyzer longer than
    /// this length, it is skipped and a message is printed to infoStream, if set (see {@link
    /// #setInfoStream}).
    static int32_t MAX_TERM_LENGTH();

    /// Sets the maximum field length to INT_MAX
    static const int32_t MaxFieldLengthUNLIMITED;

    /// Sets the maximum field length to {@link #DEFAULT_MAX_FIELD_LENGTH}
    static const int32_t MaxFieldLengthLIMITED;

public:
    virtual void initialize();

    /// Returns a read-only reader, covering all committed as well as un-committed changes to the
    /// index. This provides "near real-time" searching, in that changes made during an IndexWriter
    /// session can be quickly made available for searching without closing the writer nor calling
    /// {@link #commit}.
    ///
    /// Note that this is functionally equivalent to calling {#commit} and then using {@link
    /// IndexReader#open} to open a new reader.  But the turnaround time of this method should be
    /// faster since it avoids the potentially costly {@link #commit}.
    ///
    /// You must close the {@link IndexReader} returned by this method once you are done using it.
    ///
    /// It's near real-time because there is no hard guarantee on how quickly you can get a new
    /// reader after making changes with IndexWriter.  You'll have to experiment in your situation
    /// to determine if it's fast enough.  As this is a new and experimental feature, please report
    /// back on your findings so we can learn, improve and iterate.
    ///
    /// The resulting reader supports {@link IndexReader#reopen}, but that call will simply forward
    /// back to this method (though this may change in the future).
    ///
    /// The very first time this method is called, this writer instance will make every effort to
    /// pool the readers that it opens for doing merges, applying deletes, etc.  This means additional
    /// resources (RAM, file descriptors, CPU time) will be consumed.
    ///
    /// For lower latency on reopening a reader, you should call {@link #setMergedSegmentWarmer} to
    /// pre-warm a newly merged segment before it's committed to the index.  This is important for
    /// minimizing index-to-search delay after a large merge.
    ///
    /// If an addIndexes* call is running in another thread, then this reader will only search those
    /// segments from the foreign index that have been successfully copied over, so far.
    ///
    /// NOTE: Once the writer is closed, any outstanding readers may continue to be used.  However,
    /// if you attempt to reopen any of those readers, you'll hit an AlreadyClosed exception.
    ///
    /// NOTE: This API is experimental and might change in incompatible ways in the next release.
    ///
    /// @return IndexReader that covers entire index plus all changes made so far by this IndexWriter
    /// instance
    virtual IndexReaderPtr getReader();

    /// Like {@link #getReader}, except you can specify which termInfosIndexDivisor should be used for
    /// any newly opened readers.
    ///
    /// @param termInfosIndexDivisor Subsamples which indexed terms are loaded into RAM. This has the
    /// same effect as {@link IndexWriter#setTermIndexInterval} except that setting must be done at
    /// indexing time while this setting can be set per reader.  When set to N, then one in every
    /// N*termIndexInterval terms in the index is loaded into memory.  By setting this to a value > 1
    /// you can reduce memory usage, at the expense of higher latency when loading a TermInfo.
    /// The default value is 1.  Set this to -1 to skip loading the terms index entirely.
    virtual IndexReaderPtr getReader(int32_t termInfosIndexDivisor);

    /// Obtain the number of deleted docs for a pooled reader. If the reader isn't being pooled,
    /// the segmentInfo's delCount is returned.
    virtual int32_t numDeletedDocs(const SegmentInfoPtr& info);

    virtual void acquireWrite();
    virtual void releaseWrite();
    virtual void acquireRead();

    /// Allows one readLock to upgrade to a writeLock even if there are other readLocks as long
    /// as all other readLocks are also blocked in this method
    virtual void upgradeReadToWrite();

    virtual void releaseRead();
    virtual bool isOpen(bool includePendingClose);
    virtual void message(const String& message);

    /// Get the current setting of whether newly flushed segments will use the compound file format.
    /// Note that this just returns the value previously set with setUseCompoundFile(bool), or the
    /// default value (true).  You cannot use this to query the status of previously flushed segments.
    ///
    /// Note that this method is a convenience method: it just calls mergePolicy.getUseCompoundFile
    /// as long as mergePolicy is an instance of {@link LogMergePolicy}. Otherwise an IllegalArgument
    /// exception is thrown.
    /// @see #setUseCompoundFile(bool)
    virtual bool getUseCompoundFile();

    /// Setting to turn on usage of a compound file. When on, multiple files for each segment are
    /// merged into a single file when a new segment is flushed.
    ///
    /// Note that this method is a convenience method: it just calls mergePolicy.setUseCompoundFile
    /// as long as mergePolicy is an instance of {@link LogMergePolicy}. Otherwise an IllegalArgument
    /// exception is thrown.
    virtual void setUseCompoundFile(bool value);

    /// Set the Similarity implementation used by this IndexWriter.
    virtual void setSimilarity(const SimilarityPtr& similarity);

    /// Return the Similarity implementation used by this IndexWriter.
    /// This defaults to the current value of {@link Similarity#getDefault()}.
    virtual SimilarityPtr getSimilarity();

    /// Set the interval between indexed terms.  Large values cause less memory to be used by
    /// IndexReader, but slow random-access to terms.  Small values cause more memory to be used by
    /// an IndexReader, and speed random-access to terms.
    ///
    /// This parameter determines the amount of computation required per query term, regardless of
    /// the number of documents that contain that term.  In particular, it is the maximum number of
    /// other terms that must be scanned before a term is located and its frequency and position
    /// information may be processed.  In a large index with user-entered query terms, query
    /// processing time is likely to be dominated not by term lookup but rather by the processing of
    /// frequency and positional data.  In a small index or when many uncommon query terms are
    /// generated (eg., by wildcard queries) term lookup may become a dominant cost.
    ///
    /// In particular, numUniqueTerms/interval terms are read into memory by an IndexReader, and on
    /// average, interval/2 terms must be scanned for each random term access.
    ///
    /// @see #DEFAULT_TERM_INDEX_INTERVAL
    virtual void setTermIndexInterval(int32_t interval);

    /// Return the interval between indexed terms.
    /// @see #setTermIndexInterval(int32_t)
    virtual int32_t getTermIndexInterval();

    /// Set the merge policy used by this writer.
    virtual void setMergePolicy(const MergePolicyPtr& mp);

    /// Returns the current MergePolicy in use by this writer.
    /// @see #setMergePolicy
    virtual MergePolicyPtr getMergePolicy();

    /// Set the merge scheduler used by this writer.
    virtual void setMergeScheduler(const MergeSchedulerPtr& mergeScheduler);

    /// Returns the current MergePolicy in use by this writer.
    /// @see #setMergePolicy
    virtual MergeSchedulerPtr getMergeScheduler();

    /// Determines the largest segment (measured by document count) that may be merged with other
    /// segments.  Small values (eg., less than 10,000) are best for interactive indexing, as this
    /// limits the length of pauses while indexing to a few seconds.  Larger values are best for
    /// batched indexing and speedier searches.
    ///
    /// The default value is INT_MAX.
    ///
    /// Note that this method is a convenience method: it just calls mergePolicy.setMaxMergeDocs as
    /// long as mergePolicy is an instance of {@link LogMergePolicy}. Otherwise an IllegalArgument
    /// exception is thrown.
    ///
    /// The default merge policy ({@link LogByteSizeMergePolicy}) also allows you to set this limit
    /// by net size (in MB) of the segment, using {@link LogByteSizeMergePolicy#setMaxMergeMB}.
    virtual void setMaxMergeDocs(int32_t maxMergeDocs);

    /// Returns the largest segment (measured by document count) that may be merged with other
    /// segments.
    ///
    /// Note that this method is a convenience method: it just calls mergePolicy.getMaxMergeDocs as
    /// long as mergePolicy is an instance of {@link LogMergePolicy}. Otherwise an IllegalArgument
    /// exception is thrown.
    ///
    /// @see #setMaxMergeDocs
    virtual int32_t getMaxMergeDocs();

    /// The maximum number of terms that will be indexed for a single field in a document.  This
    /// limits the amount of memory required for indexing, so that collections with very large files
    /// will not crash the indexing process by running out of memory.  This setting refers to the
    /// number of running terms, not to the number of different terms.
    /// Note: this silently truncates large documents, excluding from the index all terms that occur
    /// further in the document.  If you know your source documents are large, be sure to set this
    /// value high enough to accommodate the expected size.  If you set it to INT_MAX, then the only
    /// limit is your memory, but you should anticipate an std::bad_alloc.
    /// By default, no more than {@link #DEFAULT_MAX_FIELD_LENGTH} terms will be indexed for a field.
    virtual void setMaxFieldLength(int32_t maxFieldLength);

    /// Returns the maximum number of terms that will be indexed for a single field in a document.
    /// @see #setMaxFieldLength
    virtual int32_t getMaxFieldLength();

    /// Sets the termsIndexDivisor passed to any readers that IndexWriter opens, for example when
    /// applying deletes or creating a near-real-time reader in {@link IndexWriter#getReader}.
    /// Default value is {@link IndexReader#DEFAULT_TERMS_INDEX_DIVISOR}.
    virtual void setReaderTermsIndexDivisor(int32_t divisor);

    /// @see #setReaderTermsIndexDivisor()
    virtual int32_t getReaderTermsIndexDivisor();

    /// Determines the minimal number of documents required before the buffered in-memory documents
    /// are flushed as a new Segment.  Large values generally gives faster indexing.
    ///
    /// When this is set, the writer will flush every maxBufferedDocs added documents.  Pass in
    /// {@link #DISABLE_AUTO_FLUSH} to prevent triggering a flush due to number of buffered
    /// documents.  Note that if flushing by RAM usage is also enabled, then the flush will be
    /// triggered by whichever comes first.
    ///
    /// Disabled by default (writer flushes by RAM usage).
    ///
    /// @see #setRAMBufferSizeMB
    virtual void setMaxBufferedDocs(int32_t maxBufferedDocs);

    /// Returns the number of buffered added documents that will trigger a flush if enabled.
    /// @see #setMaxBufferedDocs
    virtual int32_t getMaxBufferedDocs();

    /// Determines the amount of RAM that may be used for buffering added documents and deletions
    /// before they are flushed to the Directory.  Generally for faster indexing performance it's
    /// best to flush by RAM usage instead of document count and use as large a RAM buffer as you can.
    ///
    /// When this is set, the writer will flush whenever buffered documents and deletions use this
    /// much RAM.  Pass in {@link #DISABLE_AUTO_FLUSH} to prevent triggering a flush due to RAM usage.
    /// Note that if flushing by document count is also enabled, then the flush will be triggered by
    /// whichever comes first.
    ///
    /// Note: the account of RAM usage for pending deletions is only approximate.  Specifically, if
    /// you delete by Query, Lucene currently has no way to measure the RAM usage if individual
    /// Queries so the accounting will under-estimate and you should compensate by either calling
    /// commit() periodically yourself, or by using {@link #setMaxBufferedDeleteTerms} to flush by
    /// count instead of RAM usage (each buffered delete Query counts as one).
    ///
    /// Note: because IndexWriter uses int32_t when managing its internal storage, the absolute
    /// maximum value for this setting is somewhat less than 2048 MB.  The precise limit depends on
    /// various factors, such as how large your documents are, how many fields have norms, etc., so
    /// it's best to set this value comfortably under 2048.
    ///
    /// The default value is {@link #DEFAULT_RAM_BUFFER_SIZE_MB}.
    virtual void setRAMBufferSizeMB(double mb);

    /// Returns the value set by {@link #setRAMBufferSizeMB} if enabled.
    virtual double getRAMBufferSizeMB();

    /// Determines the minimal number of delete terms required before the buffered in-memory delete
    /// terms are applied and flushed. If there are documents buffered in memory at the time, they
    /// are merged and a new segment is created.
    ///
    /// Disabled by default (writer flushes by RAM usage).
    /// @see #setRAMBufferSizeMB
    virtual void setMaxBufferedDeleteTerms(int32_t maxBufferedDeleteTerms);

    /// Returns the number of buffered deleted terms that will trigger a flush if enabled.
    /// @see #setMaxBufferedDeleteTerms
    virtual int32_t getMaxBufferedDeleteTerms();

    /// Determines how often segment indices are merged by addDocument().  With smaller values, less
    /// RAM is used while indexing, and searches on unoptimized indices are faster, but indexing
    /// speed is slower.  With larger values, more RAM is used during indexing, and while searches
    /// on unoptimized indices are slower, indexing is faster.  Thus larger values (> 10) are best
    /// for batch index creation, and smaller values (< 10) for indices that are interactively maintained.
    ///
    /// Note that this method is a convenience method: it just calls mergePolicy.setMergeFactor as long
    /// as mergePolicy is an instance of {@link LogMergePolicy}. Otherwise an IllegalArgument exception
    /// is thrown.  This must never be less than 2.  The default value is 10.
    virtual void setMergeFactor(int32_t mergeFactor);

    /// Returns the number of segments that are merged at once and also controls the total number of
    /// segments allowed to accumulate in the index.
    ///
    /// Note that this method is a convenience method: it just calls mergePolicy.getMergeFactor as long
    /// as mergePolicy is an instance of {@link LogMergePolicy}. Otherwise an IllegalArgument exception
    /// is thrown.
    /// @see #setMergeFactor
    virtual int32_t getMergeFactor();

    /// If non-null, this will be the default infoStream used by a newly instantiated IndexWriter.
    /// @see #setInfoStream
    static void setDefaultInfoStream(const InfoStreamPtr& infoStream);

    /// Returns the current default infoStream for newly instantiated IndexWriters.
    /// @see #setDefaultInfoStream
    static InfoStreamPtr getDefaultInfoStream();

    /// If non-null, information about merges, deletes and a message when maxFieldLength is reached
    /// will be printed to this.
    virtual void setInfoStream(const InfoStreamPtr& infoStream);

    /// Returns the current infoStream in use by this writer.
    /// @see #setInfoStream
    virtual InfoStreamPtr getInfoStream();

    /// Returns true if verbosing is enabled (i.e., infoStream != null).
    virtual bool verbose();

    /// Sets the maximum time to wait for a write lock (in milliseconds) for this instance of
    /// IndexWriter.  @see #setDefaultWriteLockTimeout to change the default value for all instances
    /// of IndexWriter.
    virtual void setWriteLockTimeout(int64_t writeLockTimeout);

    /// Returns allowed timeout when acquiring the write lock.
    /// @see #setWriteLockTimeout
    virtual int64_t getWriteLockTimeout();

    /// Sets the default (for any instance of IndexWriter) maximum time to wait for a write lock
    /// (in milliseconds).
    static void setDefaultWriteLockTimeout(int64_t writeLockTimeout);

    /// Returns default write lock timeout for newly instantiated IndexWriters.
    /// @see #setDefaultWriteLockTimeout
    static int64_t getDefaultWriteLockTimeout();

    /// Commits all changes to an index and closes all associated files.  Note that this may be
    /// a costly operation, so try to re-use a single writer instead of closing and opening a
    /// new one.  See {@link #commit()} for caveats about write caching done by some IO devices.
    ///
    /// If an Exception is hit during close, eg due to disk full or some other reason, then both
    /// the on-disk index and the internal state of the IndexWriter instance will be consistent.
    /// However, the close will not be complete even though part of it (flushing buffered documents)
    /// may have succeeded, so the write lock will still be held.
    ///
    /// If you can correct the underlying cause (eg free up some disk space) then you can call
    /// close() again.
    ///
    /// NOTE: if this method hits an std::bad_alloc you should immediately close the writer, again.
    virtual void close();

    /// Closes the index with or without waiting for currently running merges to finish.  This is
    /// only meaningful when using a MergeScheduler that runs merges in background threads.
    ///
    /// NOTE: if this method hits an std::bad_alloc you should immediately close the writer, again.
    ///
    /// NOTE: it is dangerous to always call close(false), especially when IndexWriter is not open
    /// for very long, because this can result in "merge starvation" whereby long merges will never
    /// have a chance to finish.  This will cause too many segments in your index over time.
    ///
    /// @param waitForMerges if true, this call will block until all merges complete; else, it will
    /// ask all running merges to abort, wait until those merges have finished (which should be at
    /// most a few seconds), and then return.
    virtual void close(bool waitForMerges);

    /// Returns the Directory used by this index.
    virtual DirectoryPtr getDirectory();

    /// Returns the analyzer used by this index.
    virtual AnalyzerPtr getAnalyzer();

    /// Returns total number of docs in this index, including docs not yet flushed (still in the
    /// RAM buffer), not counting deletions.
    /// @see #numDocs
    virtual int32_t maxDoc();

    /// Returns total number of docs in this index, including docs not yet flushed (still in the
    /// RAM buffer), and including deletions.
    /// NOTE: buffered deletions are not counted.  If you really need these to be counted you should
    /// call {@link #commit()} first.
    virtual int32_t numDocs();

    virtual bool hasDeletions();

    /// Adds a document to this index.  If the document contains more than {@link
    /// #setMaxFieldLength(int32_t)} terms for a given field, the remainder are discarded.
    ///
    /// Note that if an Exception is hit (for example disk full) then the index will be consistent,
    /// but this document may not have been added.  Furthermore, it's possible the index will have
    /// one segment in non-compound format even when using compound files (when a merge has partially
    /// succeeded).
    ///
    /// This method periodically flushes pending documents to the Directory, and also periodically
    /// triggers segment merges in the index according to the {@link MergePolicy} in use.
    ///
    /// Merges temporarily consume space in the directory. The amount of space required is up to 1X
    /// the size of all segments being merged, when no size of all segments being merged, when no
    /// 2X the size of all segments being merged when readers/searchers are open against the index
    /// (see {@link #optimize()} for details). The sequence of primitive merge operations performed
    /// is governed by the merge policy.
    ///
    /// Note that each term in the document can be no longer than 16383 characters, otherwise an
    /// IllegalArgument exception will be thrown.
    ///
    /// NOTE: if this method hits an std::bad_alloc you should immediately close the writer.
    virtual void addDocument(const DocumentPtr& doc);

    /// Adds a document to this index, using the provided analyzer instead of the value of {@link
    /// #getAnalyzer()}.  If the document contains more than {@link #setMaxFieldLength(int32_t)} terms
    /// for a given field, the remainder are discarded.
    ///
    /// See {@link #addDocument(DocumentPtr)} for details on index and IndexWriter state after an
    /// exception, and flushing/merging temporary free space requirements.
    ///
    /// NOTE: if this method hits an std::bad_alloc you should immediately close the writer.
    virtual void addDocument(const DocumentPtr& doc, const AnalyzerPtr& analyzer);

    /// Deletes the document(s) containing term.
    ///
    /// NOTE: if this method hits an std::bad_alloc you should immediately close the writer.
    ///
    /// @param term the term to identify the documents to be deleted
    virtual void deleteDocuments(const TermPtr& term);

    /// Deletes the document(s) containing any of the terms. All deletes are flushed at the same time.
    ///
    /// NOTE: if this method hits an std::bad_alloc you should immediately close the writer.
    ///
    /// @param terms array of terms to identify the documents to be deleted
    virtual void deleteDocuments(Collection<TermPtr> terms);

    /// Deletes the document(s) matching the provided query.
    ///
    /// NOTE: if this method hits an std::bad_alloc you should immediately close the writer.
    ///
    /// @param query the query to identify the documents to be deleted
    virtual void deleteDocuments(const QueryPtr& query);

    /// Deletes the document(s) matching any of the provided queries.  All deletes are flushed at
    /// the same time.
    ///
    /// NOTE: if this method hits an std::bad_alloc you should immediately close the writer.
    ///
    /// @param queries array of queries to identify the documents to be deleted
    virtual void deleteDocuments(Collection<QueryPtr> queries);

    /// Updates a document by first deleting the document(s) containing term and then adding the new
    /// document.  The delete and then add are atomic as seen by a reader on the same index (flush
    /// may happen only after the add).
    ///
    /// NOTE: if this method hits an std::bad_alloc you should immediately close the writer.
    ///
    /// @param term the term to identify the document(s) to be deleted
    /// @param doc the document to be added
    virtual void updateDocument(const TermPtr& term, const DocumentPtr& doc);

    /// Updates a document by first deleting the document(s) containing term and then adding the new
    /// document.  The delete and then add are atomic as seen by a reader on the same index (flush
    /// may happen only after the add).
    ///
    /// NOTE: if this method hits an std::bad_alloc you should immediately close the writer.
    ///
    /// @param term the term to identify the document(s) to be deleted
    /// @param doc the document to be added
    /// @param analyzer the analyzer to use when analyzing the document
    virtual void updateDocument(const TermPtr& term, const DocumentPtr& doc, const AnalyzerPtr& analyzer);

    virtual int32_t getSegmentCount();
    virtual int32_t getNumBufferedDocuments();
    virtual int32_t getDocCount(int32_t i);
    virtual int32_t getFlushCount();
    virtual int32_t getFlushDeletesCount();

    virtual String newSegmentName();

    /// Requests an "optimize" operation on an index, priming the index for the fastest available
    /// search. Traditionally this has meant merging all segments into a single segment as is done in
    /// the default merge policy, but individual merge policies may implement optimize in different ways.
    ///
    /// It is recommended that this method be called upon completion of indexing.  In environments with
    /// frequent updates, optimize is best done during low volume times, if at all.
    ///
    /// Note that optimize requires 2X the index size free space in your Directory (3X if you're using
    /// compound file format).  For example, if your index size is 10 MB then you need 20 MB free for
    /// optimize to complete (30 MB if you're using compound file format).
    ///
    /// If some but not all readers re-open while an optimize is underway, this will cause > 2X temporary
    /// space to be consumed as those new readers will then hold open the partially optimized segments at
    /// that time.  It is best not to re-open readers while optimize is running.
    ///
    /// The actual temporary usage could be much less than these figures (it depends on many factors).
    ///
    /// In general, once the optimize completes, the total size of the index will be less than the size
    /// of the starting index.  It could be quite a bit smaller (if there were many pending deletes) or
    /// just slightly smaller.
    ///
    /// If an Exception is hit during optimize(), for example due to disk full, the index will not be
    /// corrupt and no documents will have been lost.  However, it may have been partially optimized
    /// (some segments were merged but not all), and it's possible that one of the segments in the index
    /// will be in non-compound format even when using compound file format.  This will occur when the
    /// exception is hit during conversion of the segment into compound format.
    ///
    /// This call will optimize those segments present in the index when the call started.  If other
    /// threads are still adding documents and flushing segments, those newly created segments will not
    /// be optimized unless you call optimize again.
    ///
    /// NOTE: if this method hits an std::bad_alloc you should immediately close the writer.
    ///
    /// @see LogMergePolicy#findMergesForOptimize
    virtual void optimize();

    /// Optimize the index down to <= maxNumSegments.  If maxNumSegments==1 then this is the same as
    /// {@link #optimize()}.
    ///
    /// NOTE: if this method hits an std::bad_alloc you should immediately close the writer.
    ///
    /// @param maxNumSegments maximum number of segments left in the index after optimization finishes
    virtual void optimize(int32_t maxNumSegments);

    /// Just like {@link #optimize()}, except you can specify whether the call should block until the
    /// optimize completes.  This is only meaningful with a {@link MergeScheduler} that is able to run
    /// merges in background threads.
    ///
    /// NOTE: if this method hits an std::bad_alloc you should immediately close the writer.
    virtual void optimize(bool doWait);

    /// Just like {@link #optimize(int32_t)}, except you can specify whether the call should block
    /// until the optimize completes.  This is only meaningful with a {@link MergeScheduler} that is
    /// able to run merges in background threads.
    ///
    /// NOTE: if this method hits an std::bad_alloc you should immediately close the writer.
    virtual void optimize(int32_t maxNumSegments, bool doWait);

    /// Just like {@link #expungeDeletes()}, except you can specify whether the call should block
    /// until the operation completes.  This is only meaningful with a {@link MergeScheduler} that
    /// is able to run merges in background threads.
    ///
    /// NOTE: if this method hits an std::bad_alloc you should immediately close the writer.
    virtual void expungeDeletes(bool doWait);

    /// Expunges all deletes from the index.  When an index has many document deletions (or updates
    /// to existing documents), it's best to either call optimize or expungeDeletes to remove all
    /// unused data in the index associated with the deleted documents.  To see how many deletions
    /// you have pending in your index, call {@link IndexReader#numDeletedDocs}.  This saves disk
    /// space and memory usage while searching.  expungeDeletes should be somewhat faster than
    /// optimize since it does not insist on reducing the index to a single segment (though, this
    /// depends on the {@link MergePolicy}; see {@link MergePolicy#findMergesToExpungeDeletes}.).
    /// Note that this call does not first commit any buffered documents, so you must do so yourself
    /// if necessary. See also {@link #expungeDeletes(bool)}
    ///
    /// NOTE: if this method hits an std::bad_alloc you should immediately close the writer.
    virtual void expungeDeletes();

    /// Asks the mergePolicy whether any merges are necessary now and if so, runs the requested
    /// merges and then iterate (test again if merges are needed) until no more merges are returned
    /// by the mergePolicy.
    ///
    /// Explicit calls to maybeMerge() are usually not necessary. The most common case is when merge
    /// policy parameters have changed.
    ///
    /// NOTE: if this method hits an std::bad_alloc you should immediately close the writer.
    virtual void maybeMerge();

    /// The {@link MergeScheduler} calls this method to retrieve the next merge requested by the
    /// MergePolicy.
    virtual OneMergePtr getNextMerge();

    /// Close the IndexWriter without committing any changes that have occurred since the last commit
    /// (or since it was opened, if commit hasn't been called).  This removes any temporary files that
    /// had been created, after which the state of the index will be the same as it was when commit()
    /// was last called or when this writer was first opened.  This also clears a previous call to
    /// {@link #prepareCommit}.
    virtual void rollback();

    /// Delete all documents in the index.
    ///
    /// This method will drop all buffered documents and will remove all segments from the index. This
    /// change will not be visible until a {@link #commit()} has been called. This method can be rolled
    /// back using {@link #rollback()}.
    ///
    /// NOTE: this method is much faster than using {@link #deleteDocuments()}.
    ///
    /// NOTE: this method will forcefully abort all merges in progress.  If other threads are running
    /// {@link #optimize()} or any of the addIndexes methods, they will receive {@link
    /// MergePolicy.MergeAbortedException}
    virtual void deleteAll();

    /// Wait for any currently outstanding merges to finish.
    ///
    /// It is guaranteed that any merges started prior to calling this method will have completed once
    /// this method completes.
    virtual void waitForMerges();

    /// Merges all segments from an array of indexes into this index.
    ///
    /// This may be used to parallelize batch indexing.  A large document collection can be broken into
    /// sub-collections.  Each sub-collection can be indexed in parallel, on a different thread, process
    /// or machine.  The complete index can then be created by merging sub-collection indexes with this
    /// method.
    ///
    /// NOTE: the index in each Directory must not be changed (opened by a writer) while this method is
    /// running.  This method does not acquire a write lock in each input Directory, so it is up to the
    /// caller to enforce this.
    ///
    /// NOTE: while this is running, any attempts to add or delete documents (with another thread) will
    /// be paused until this method completes.
    ///
    /// This method is transactional in how exceptions are handled: it does not commit a new segments_N
    /// file until all indexes are added.  This means if an exception occurs (for example disk full),
    /// then either no indexes will have been added or they all will have been.
    ///
    /// Note that this requires temporary free space in the Directory up to 2X the sum of all input
    /// indexes (including the starting index).  If readers/searchers are open against the starting index,
    /// then temporary free space required will be higher by the size of the starting index (see
    /// {@link #optimize()} for details).
    ///
    /// Once this completes, the final size of the index will be less than the sum of all input index
    /// sizes (including the starting index).  It could be quite a bit smaller (if there were many pending
    /// deletes) or just slightly smaller.
    ///
    /// This requires this index not be among those to be added.
    ///
    /// NOTE: if this method hits an std::bad_alloc you should immediately close the writer.
    virtual void addIndexesNoOptimize(Collection<DirectoryPtr> dirs);

    /// Merges the provided indexes into this index.
    /// After this completes, the index is optimized.  The provided IndexReaders are not closed.
    ///
    /// NOTE: while this is running, any attempts to add or delete documents (with another thread) will
    /// be paused until this method completes.
    ///
    /// See {@link #addIndexesNoOptimize} for details on transactional semantics, temporary free space
    /// required in the Directory, and non-CFS segments on an exception.
    ///
    /// NOTE: if this method hits an std::bad_alloc you should immediately close the writer.
    virtual void addIndexes(Collection<IndexReaderPtr> readers);

    /// Prepare for commit.
    ///
    /// NOTE: if this method hits an std::bad_alloc you should immediately close the writer.
    /// @see #prepareCommit(MapStringString)
    virtual void prepareCommit();

    /// Prepare for commit, specifying commitUserData Map (String -> String).  This does the first phase
    /// of 2-phase commit. This method does all steps necessary to commit changes since this writer was
    /// opened: flushes pending added and deleted docs, syncs the index files, writes most of next
    /// segments_N file.  After calling this you must call either {@link #commit()} to finish the commit,
    /// or {@link #rollback()} to revert the commit and undo all changes done since the writer was opened.
    ///
    /// You can also just call {@link #commit(Map)} directly without prepareCommit first in which case
    /// that method will internally call prepareCommit.
    ///
    /// NOTE: if this method hits an std::bad_alloc you should immediately close the writer.
    ///
    /// @param commitUserData Opaque Map (String->String) that's recorded into the segments file in the
    /// index, and retrievable by {@link IndexReader#getCommitUserData}.  Note that when IndexWriter
    /// commits itself during {@link #close}, the commitUserData is unchanged (just carried over from the
    /// prior commit).  If this is null then the previous commitUserData is kept.  Also, the commitUserData
    // will only "stick" if there are actually changes in the index to commit.
    virtual void prepareCommit(MapStringString commitUserData);

    /// Commits all pending changes (added & deleted documents, optimizations, segment merges, added
    /// indexes, etc.) to the index, and syncs all referenced index files, such that a reader will see the
    /// changes and the index updates will survive an OS or machine crash or power loss.  Note that this
    /// does not wait for any running background merges to finish.  This may be a costly operation, so you
    /// should test the cost in your application and do it only when really necessary.
    ///
    /// Note that this operation calls Directory.sync on the index files.  That call should not return until
    /// the file contents & metadata are on stable storage.  For FSDirectory, this calls the OS's fsync.
    /// But, beware: some hardware devices may in fact cache writes even during fsync, and return before the
    /// bits are actually on stable storage, to give the appearance of faster performance.  If you have such
    /// a device, and it does not have a battery backup (for example) then on power loss it may still lose
    /// data.  Lucene cannot guarantee consistency on such devices.
    ///
    /// NOTE: if this method hits an std::bad_alloc you should immediately close the writer.
    ///
    /// @see #prepareCommit
    /// @see #commit(MapStringString)
    virtual void commit();

    /// Commits all changes to the index, specifying a commitUserData Map (String -> String).  This just
    /// calls {@link #prepareCommit(MapStringString)} (if you didn't already call it) and then
    /// {@link #finishCommit}.
    ///
    /// NOTE: if this method hits an std::bad_alloc you should immediately close the writer.
    virtual void commit(MapStringString commitUserData);

    /// Return the total size of all index files currently cached in memory.  Useful for size management
    /// with flushRamDocs()
    virtual int64_t ramSizeInBytes();

    /// Return the number of documents currently buffered in RAM.
    virtual int32_t numRamDocs();

    /// Merges the indicated segments, replacing them in the stack with a single segment.
    virtual void merge(const OneMergePtr& merge);

    /// Hook that's called when the specified merge is complete.
    virtual void mergeSuccess(const OneMergePtr& merge);

    /// Checks whether this merge involves any segments already participating in a merge.  If not, this
    /// merge is "registered", meaning we record that its segments are now participating in a merge,
    /// and true is returned.  Else (the merge conflicts) false is returned.
    virtual bool registerMerge(const OneMergePtr& merge);

    /// Does initial setup for a merge, which is fast but holds the synchronized lock on IndexWriter
    /// instance.
    virtual void mergeInit(const OneMergePtr& merge);

    /// Does finishing for a merge, which is fast but holds the synchronized lock on IndexWriter instance.
    virtual void mergeFinish(const OneMergePtr& merge);

    virtual void addMergeException(const OneMergePtr& merge);

    /// For test purposes.
    virtual int32_t getBufferedDeleteTermsSize();

    /// For test purposes.
    virtual int32_t getNumBufferedDeleteTerms();

    /// Utility routines for tests
    virtual SegmentInfoPtr newestSegment();

    virtual String segString();

    /// Returns true if the index in the named directory is currently locked.
    /// @param directory the directory to check for a lock
    static bool isLocked(const DirectoryPtr& directory);

    /// Forcibly unlocks the index in the named directory.
    /// Caution: this should only be used by failure recovery code, when it is known that no other process
    /// nor thread is in fact currently accessing this index.
    static void unlock(const DirectoryPtr& directory);

    /// Set the merged segment warmer.  See {@link IndexReaderWarmer}.
    virtual void setMergedSegmentWarmer(const IndexReaderWarmerPtr& warmer);

    /// Returns the current merged segment warmer.  See {@link IndexReaderWarmer}.
    virtual IndexReaderWarmerPtr getMergedSegmentWarmer();

    /// Used only by assert for testing.  Current points:
    ///   startDoFlush
    ///   startCommitMerge
    ///   startStartCommit
    ///   midStartCommit
    ///   midStartCommit2
    ///   midStartCommitSuccess
    ///   finishStartCommit
    ///   startCommitMergeDeletes
    ///   startMergeInit
    ///   startApplyDeletes
    ///   startMergeInit
    ///   startMergeInit
    virtual bool testPoint(const String& name);

    virtual bool nrtIsCurrent(const SegmentInfosPtr& infos);
    virtual bool isClosed();

protected:
    virtual void ensureOpen(bool includePendingClose);
    virtual void ensureOpen();
    virtual void setMessageID(const InfoStreamPtr& infoStream);

    /// Casts current mergePolicy to LogMergePolicy, and throws an exception if the
    /// mergePolicy is not a LogMergePolicy.
    virtual LogMergePolicyPtr getLogMergePolicy();

    virtual void setRollbackSegmentInfos(const SegmentInfosPtr& infos);

    /// If we are flushing by doc count (not by RAM usage), and using LogDocMergePolicy then push
    /// maxBufferedDocs down as its minMergeDocs, to keep backwards compatibility.
    virtual void pushMaxBufferedDocs();

    virtual void messageState();

    /// Returns true if this thread should attempt to close, or false if IndexWriter is now closed;
    /// else, waits until another thread finishes closing
    virtual bool shouldClose();
    virtual void closeInternal(bool waitForMerges);

    /// Tells the docWriter to close its currently open shared doc stores (stored fields & vectors
    /// files).  Return value specifies whether new doc store files are compound or not.
    virtual bool flushDocStores();

    /// Returns true if any merges in pendingMerges or runningMerges are optimization merges.
    virtual bool optimizeMergesPending();

    virtual void maybeMerge(bool optimize);
    virtual void maybeMerge(int32_t maxNumSegmentsOptimize, bool optimize);
    virtual void updatePendingMerges(int32_t maxNumSegmentsOptimize, bool optimize);

    /// Like {@link #getNextMerge()} except only returns a merge if it's external.
    virtual OneMergePtr getNextExternalMerge();

    /// Begin a transaction.  During a transaction, any segment merges that happen (or ram segments
    /// flushed) will not write a new segments file and will not remove any files that were present
    /// at the start of the transaction.  You must make a matched call to commitTransaction() or
    /// rollbackTransaction() to finish the transaction.
    ///
    /// Note that buffered documents and delete terms are not handled within the transactions, so
    /// they must be flushed before the transaction is started.
    virtual void startTransaction(bool haveReadLock);

    /// Rolls back the transaction and restores state to where we were at the start.
    virtual void rollbackTransaction();

    /// Commits the transaction.  This will write the new segments file and remove and pending
    /// deletions we have accumulated during the transaction.
    virtual void commitTransaction();
    virtual void rollbackInternal();

    virtual void finishMerges(bool waitForMerges);

    /// Called whenever the SegmentInfos has been updated and the index files referenced exist
    /// (correctly) in the index directory.
    virtual void checkpoint();

    virtual void finishAddIndexes();
    virtual void blockAddIndexes(bool includePendingClose);
    virtual void resumeAddIndexes();
    virtual void resetMergeExceptions();
    virtual void noDupDirs(Collection<DirectoryPtr> dirs);

    virtual bool hasExternalSegments();

    /// If any of our segments are using a directory != ours then we have to either copy them over one
    /// by one, merge them (if merge policy has chosen to) or wait until currently running merges (in
    /// the background) complete.  We don't return until the SegmentInfos has no more external segments.
    /// Currently this is only used by addIndexesNoOptimize().
    virtual void resolveExternalSegments();

    /// A hook for extending classes to execute operations after pending added and deleted documents have
    /// been flushed to the Directory but before the change is committed (new segments_N file written).
    virtual void doAfterFlush();

    /// A hook for extending classes to execute operations before pending added and deleted documents are
    /// flushed to the Directory.
    virtual void doBeforeFlush();

    virtual void commit(int64_t sizeInBytes);
    virtual void finishCommit();

    /// Flush all in-memory buffered updates (adds and deletes) to the Directory.
    /// @param triggerMerge if true, we may merge segments (if deletes or docs were flushed) if necessary
    /// @param flushDocStores if false we are allowed to keep doc stores open to share with the next segment
    /// @param flushDeletes whether pending deletes should also be flushed
    virtual void flush(bool triggerMerge, bool flushDocStores, bool flushDeletes);
    virtual bool doFlush(bool flushDocStores, bool flushDeletes);
    virtual bool doFlushInternal(bool flushDocStores, bool flushDeletes);

    virtual int32_t ensureContiguousMerge(const OneMergePtr& merge);

    /// Carefully merges deletes for the segments we just merged.  This is tricky because, although merging
    /// will clear all deletes (compacts the documents), new deletes may have been flushed to the segments
    /// since the merge was started.  This method "carries over" such new deletes onto the newly merged
    /// segment, and saves the resulting deletes file (incrementing the delete generation for merge.info).
    /// If no deletes were flushed, no new deletes file is saved.
    virtual void commitMergedDeletes(const OneMergePtr& merge, const SegmentReaderPtr& mergeReader);
    virtual bool commitMerge(const OneMergePtr& merge, const SegmentMergerPtr& merger, int32_t mergedDocCount, const SegmentReaderPtr& mergedReader);

    virtual LuceneException handleMergeException(const LuceneException& exc, const OneMergePtr& merge);

    virtual void _mergeInit(const OneMergePtr& merge);

    virtual void setDiagnostics(const SegmentInfoPtr& info, const String& source);
    virtual void setDiagnostics(const SegmentInfoPtr& info, const String& source, MapStringString details);

    virtual void setMergeDocStoreIsCompoundFile(const OneMergePtr& merge);
    virtual void closeMergeReaders(const OneMergePtr& merge, bool suppressExceptions);

    /// Does the actual (time-consuming) work of the merge, but without holding synchronized lock on
    /// IndexWriter instance.
    virtual int32_t mergeMiddle(const OneMergePtr& merge);

    /// Apply buffered deletes to all segments.
    virtual bool applyDeletes();

    virtual String segString(const SegmentInfosPtr& infos);

    virtual bool startSync(const String& fileName, HashSet<String> pending);
    virtual void finishSync(const String& fileName, bool success);

    /// Blocks until all files in syncing are sync'd
    bool waitForAllSynced(HashSet<String> syncing);
    void doWait();

    /// Walk through all files referenced by the current segmentInfos and ask the Directory to sync each
    /// file, if it wasn't already.  If that succeeds, then we prepare a new segments_N file but do not
    /// fully commit it.
    virtual void startCommit(int64_t sizeInBytes, MapStringString commitUserData);

    virtual LuceneException handleOOM(const std::bad_alloc& oom, const String& location);

    friend class ReaderPool;
};

/// If {@link #getReader} has been called (ie, this writer is in near real-time mode), then after
/// a merge completes, this class can be invoked to warm the reader on the newly merged segment,
/// before the merge commits.  This is not required for near real-time search, but will reduce
/// search latency on opening a new near real-time reader after a merge completes.
///
/// NOTE: warm is called before any deletes have been carried over to the merged segment.
class IndexReaderWarmer : public LuceneObject {
public:
    virtual ~IndexReaderWarmer();

    LUCENE_CLASS(IndexReaderWarmer);

public:
    virtual void warm(const IndexReaderPtr& reader) = 0;
};

}

#endif
