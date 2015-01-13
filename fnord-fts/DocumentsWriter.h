/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef DOCUMENTSWRITER_H
#define DOCUMENTSWRITER_H

#include "ByteBlockPool.h"
#include "RAMFile.h"

namespace Lucene {

/// This class accepts multiple added documents and directly writes a single segment file.  It does this more
/// efficiently than creating a single segment per document (with DocumentWriter) and doing standard merges on
/// those segments.
///
/// Each added document is passed to the {@link DocConsumer}, which in turn processes the document and interacts
/// with other consumers in the indexing chain.  Certain consumers, like {@link StoredFieldsWriter} and {@link
/// TermVectorsTermsWriter}, digest a document and immediately write bytes to the "doc store" files (ie,
/// they do not consume RAM per document, except while they are processing the document).
///
/// Other consumers, eg {@link FreqProxTermsWriter} and {@link NormsWriter}, buffer bytes in RAM and flush only
/// when a new segment is produced.
///
/// Once we have used our allowed RAM buffer, or the number of added docs is large enough (in the case we are
/// flushing by doc count instead of RAM usage), we create a real segment and flush it to the Directory.
///
/// Threads:
/// Multiple threads are allowed into addDocument at once. There is an initial synchronized call to
/// getThreadState which allocates a ThreadState for this thread.  The same thread will get the same ThreadState
/// over time (thread affinity) so that if there are consistent patterns (for example each thread is indexing a
/// different content source) then we make better use of RAM.  Then processDocument is called on that ThreadState
/// without synchronization (most of the "heavy lifting" is in this call).  Finally the synchronized
/// "finishDocument" is called to flush changes to the directory.
///
/// When flush is called by IndexWriter we forcefully idle all threads and flush only once they are all idle.
/// This means you can call flush with a given thread even while other threads are actively adding/deleting
/// documents.
///
/// Exceptions:
/// Because this class directly updates in-memory posting lists, and flushes stored fields and term vectors
/// directly to files in the directory, there are certain limited times when an exception can corrupt this state.
/// For example, a disk full while flushing stored fields leaves this file in a corrupt state.  Or, an
/// std::bad_alloc exception while appending to the in-memory posting lists can corrupt that posting list.
/// We call such exceptions "aborting exceptions".  In these cases we must call abort() to discard all docs added
/// since the last flush.
///
/// All other exceptions ("non-aborting exceptions") can still partially update the index structures.  These
/// updates are consistent, but, they represent only a part of the document seen up until the exception was hit.
/// When this happens, we immediately mark the document as deleted so that the document is always atomically
/// ("all or none") added to the index.
class DocumentsWriter : public LuceneObject {
public:
    DocumentsWriter(const DirectoryPtr& directory, const IndexWriterPtr& writer, const IndexingChainPtr& indexingChain);
    virtual ~DocumentsWriter();

    LUCENE_CLASS(DocumentsWriter);

protected:
    String docStoreSegment; // Current doc-store segment we are writing
    int32_t docStoreOffset; // Current starting doc-store offset of current segment

    int32_t nextDocID; // Next docID to be added
    int32_t numDocsInRAM; // # docs buffered in RAM

    /// Max # ThreadState instances; if there are more threads than this they share ThreadStates
    static const int32_t MAX_THREAD_STATE;
    Collection<DocumentsWriterThreadStatePtr> threadStates;
    MapThreadDocumentsWriterThreadState threadBindings;

    int32_t pauseThreads; // Non-zero when we need all threads to pause (eg to flush)
    bool aborting; // True if an abort is pending

    DocFieldProcessorPtr docFieldProcessor;

    /// Deletes done after the last flush; these are discarded on abort
    BufferedDeletesPtr deletesInRAM;

    /// Deletes done before the last flush; these are still kept on abort
    BufferedDeletesPtr deletesFlushed;

    /// The max number of delete terms that can be buffered before they must be flushed to disk.
    int32_t maxBufferedDeleteTerms;

    /// How much RAM we can use before flushing.  This is 0 if we are flushing by doc count instead.
    int64_t ramBufferSize;
    int64_t waitQueuePauseBytes;
    int64_t waitQueueResumeBytes;

    /// If we've allocated 5% over our RAM budget, we then free down to 95%
    int64_t freeTrigger;
    int64_t freeLevel;

    /// Flush @ this number of docs.  If ramBufferSize is non-zero we will flush by RAM usage instead.
    int32_t maxBufferedDocs;

    /// How many docs already flushed to index
    int32_t flushedDocCount;

    bool closed;

    /// List of files that were written before last abort()
    HashSet<String> _abortedFiles;
    SegmentWriteStatePtr flushState;

    Collection<IntArray> freeIntBlocks;
    Collection<CharArray> freeCharBlocks;

public:
    /// Coarse estimates used to measure RAM usage of buffered deletes
    static const int32_t OBJECT_HEADER_BYTES;
    static const int32_t POINTER_NUM_BYTE;
    static const int32_t INT_NUM_BYTE;
    static const int32_t CHAR_NUM_BYTE;

    /// Rough logic: HashMap has an array[Entry] with varying load factor (say 2 * POINTER).  Entry is object
    /// with Term key, BufferedDeletes.Num val, int hash, Entry next (OBJ_HEADER + 3*POINTER + INT).  Term is
    /// object with String field and String text (OBJ_HEADER + 2*POINTER).  We don't count Term's field since
    /// it's interned.  Term's text is String (OBJ_HEADER + 4*INT + POINTER + OBJ_HEADER + string.length*CHAR).
    /// BufferedDeletes.num is OBJ_HEADER + INT.
    static const int32_t BYTES_PER_DEL_TERM;

    /// Rough logic: del docIDs are List<Integer>.  Say list allocates ~2X size (2*POINTER).  Integer is
    /// OBJ_HEADER + int
    static const int32_t BYTES_PER_DEL_DOCID;

    /// Rough logic: HashMap has an array[Entry] with varying load factor (say 2 * POINTER).  Entry is object
    /// with Query key, Integer val, int hash, Entry next (OBJ_HEADER + 3*POINTER + INT).  Query we often undercount
    /// (say 24 bytes).  Integer is OBJ_HEADER + INT.
    static const int32_t BYTES_PER_DEL_QUERY;

    /// Initial chunks size of the shared byte[] blocks used to store postings data
    static const int32_t BYTE_BLOCK_SHIFT;
    static const int32_t BYTE_BLOCK_SIZE;
    static const int32_t BYTE_BLOCK_MASK;
    static const int32_t BYTE_BLOCK_NOT_MASK;

    /// Initial chunk size of the shared char[] blocks used to store term text
    static const int32_t CHAR_BLOCK_SHIFT;
    static const int32_t CHAR_BLOCK_SIZE;
    static const int32_t CHAR_BLOCK_MASK;

    static const int32_t MAX_TERM_LENGTH;

    /// Initial chunks size of the shared int[] blocks used to store postings data
    static const int32_t INT_BLOCK_SHIFT;
    static const int32_t INT_BLOCK_SIZE;
    static const int32_t INT_BLOCK_MASK;

    static const int32_t PER_DOC_BLOCK_SIZE;

INTERNAL:
    IndexWriterWeakPtr _writer;
    DirectoryPtr directory;
    IndexingChainPtr indexingChain;
    String segment; // Current segment we are working on

    int32_t numDocsInStore; // # docs written to doc stores

    bool flushPending; // True when a thread has decided to flush
    bool bufferIsFull; // True when it's time to write segment

    InfoStreamPtr infoStream;
    int32_t maxFieldLength;
    SimilarityPtr similarity;

    DocConsumerPtr consumer;

    HashSet<String> _openFiles;
    HashSet<String> _closedFiles;

    WaitQueuePtr waitQueue;
    SkipDocWriterPtr skipDocWriter;

    ByteBlockAllocatorPtr byteBlockAllocator;
    ByteBlockAllocatorPtr perDocAllocator;

    int64_t numBytesAlloc;
    int64_t numBytesUsed;

    // used only by assert
    TermPtr lastDeleteTerm;

public:
    virtual void initialize();

    /// Create and return a new DocWriterBuffer.
    PerDocBufferPtr newPerDocBuffer();

    static IndexingChainPtr getDefaultIndexingChain();

    void updateFlushedDocCount(int32_t n);
    int32_t getFlushedDocCount();
    void setFlushedDocCount(int32_t n);

    /// Returns true if any of the fields in the current buffered docs have omitTermFreqAndPositions==false
    bool hasProx();

    /// If non-null, various details of indexing are printed here.
    void setInfoStream(const InfoStreamPtr& infoStream);

    void setMaxFieldLength(int32_t maxFieldLength);
    void setSimilarity(const SimilarityPtr& similarity);

    /// Set how much RAM we can use before flushing.
    void setRAMBufferSizeMB(double mb);
    double getRAMBufferSizeMB();

    /// Set max buffered docs, which means we will flush by doc count instead of by RAM usage.
    void setMaxBufferedDocs(int32_t count);
    int32_t getMaxBufferedDocs();

    /// Get current segment name we are writing.
    String getSegment();

    /// Returns how many docs are currently buffered in RAM.
    int32_t getNumDocsInRAM();

    /// Returns the current doc store segment we are writing to.
    String getDocStoreSegment();

    /// Returns the doc offset into the shared doc store for the current buffered docs.
    int32_t getDocStoreOffset();

    /// Closes the current open doc stores an returns the doc store segment name.  This returns null if there
    /// are no buffered documents.
    String closeDocStore();

    HashSet<String> abortedFiles();

    void message(const String& message);

    /// Returns Collection of files in use by this instance, including any flushed segments.
    HashSet<String> openFiles();
    HashSet<String> closedFiles();

    void addOpenFile(const String& name);
    void removeOpenFile(const String& name);

    void setAborting();

    /// Called if we hit an exception at a bad time (when updating the index files) and must discard all
    /// currently buffered docs.  This resets our state, discarding any docs added since last flush.
    void abort();

    /// Returns true if an abort is in progress
    bool pauseAllThreads();
    void resumeAllThreads();

    bool anyChanges();

    void initFlushState(bool onlyDocStore);

    /// Flush all pending docs to a new segment
    int32_t flush(bool _closeDocStore);

    HashSet<String> getFlushedFiles();

    /// Build compound file for the segment we just flushed
    void createCompoundFile(const String& segment);

    /// Set flushPending if it is not already set and returns whether it was set. This is used by IndexWriter
    /// to trigger a single flush even when multiple threads are trying to do so.
    bool setFlushPending();
    void clearFlushPending();

    void pushDeletes();

    void close();

    void initSegmentName(bool onlyDocStore);

    /// Returns a free (idle) ThreadState that may be used for indexing this one document.  This call also
    /// pauses if a flush is pending.  If delTerm is non-null then we buffer this deleted term after the
    /// thread state has been acquired.
    DocumentsWriterThreadStatePtr getThreadState(const DocumentPtr& doc, const TermPtr& delTerm);

    /// Returns true if the caller (IndexWriter) should now flush.
    bool addDocument(const DocumentPtr& doc, const AnalyzerPtr& analyzer);

    bool updateDocument(const TermPtr& t, const DocumentPtr& doc, const AnalyzerPtr& analyzer);
    bool updateDocument(const DocumentPtr& doc, const AnalyzerPtr& analyzer, const TermPtr& delTerm);

    int32_t getNumBufferedDeleteTerms(); // for testing
    MapTermNum getBufferedDeleteTerms(); // for testing

    /// Called whenever a merge has completed and the merged segments had deletions
    void remapDeletes(const SegmentInfosPtr& infos, Collection< Collection<int32_t> > docMaps, Collection<int32_t> delCounts, const OneMergePtr& merge, int32_t mergeDocCount);

    bool bufferDeleteTerms(Collection<TermPtr> terms);
    bool bufferDeleteTerm(const TermPtr& term);
    bool bufferDeleteQueries(Collection<QueryPtr> queries);
    bool bufferDeleteQuery(const QueryPtr& query);
    bool deletesFull();
    bool doApplyDeletes();

    void setMaxBufferedDeleteTerms(int32_t maxBufferedDeleteTerms);
    int32_t getMaxBufferedDeleteTerms();

    bool hasDeletes();
    bool applyDeletes(const SegmentInfosPtr& infos);
    bool doBalanceRAM();

    void waitForWaitQueue();

    int64_t getRAMUsed();

    IntArray getIntBlock(bool trackAllocations);
    void bytesAllocated(int64_t numBytes);
    void bytesUsed(int64_t numBytes);
    void recycleIntBlocks(Collection<IntArray> blocks, int32_t start, int32_t end);

    CharArray getCharBlock();
    void recycleCharBlocks(Collection<CharArray> blocks, int32_t numBlocks);

    String toMB(int64_t v);

    /// We have four pools of RAM: Postings, byte blocks (holds freq/prox posting data), char blocks (holds
    /// characters in the term) and per-doc buffers (stored fields/term vectors).  Different docs require
    /// varying amount of storage from these four classes.
    ///
    /// For example, docs with many unique single-occurrence short terms will use up the Postings
    /// RAM and hardly any of the other two.  Whereas docs with very large terms will use alot of char blocks
    /// RAM and relatively less of the other two.  This method just frees allocations from the pools once we
    /// are over-budget, which balances the pools to match the current docs.
    void balanceRAM();

protected:
    /// Reset after a flush
    void doAfterFlush();

    bool allThreadsIdle();

    void waitReady(const DocumentsWriterThreadStatePtr& state);

    bool timeToFlushDeletes();

    // used only by assert
    bool checkDeleteTerm(const TermPtr& term);

    bool applyDeletes(const IndexReaderPtr& reader, int32_t docIDStart);
    void addDeleteTerm(const TermPtr& term, int32_t docCount);

    /// Buffer a specific docID for deletion.  Currently only used when we hit a exception when adding a document
    void addDeleteDocID(int32_t docID);
    void addDeleteQuery(const QueryPtr& query, int32_t docID);

    /// Does the synchronized work to finish/flush the inverted document.
    void finishDocument(const DocumentsWriterThreadStatePtr& perThread, const DocWriterPtr& docWriter);

    friend class WaitQueue;
};

class DocState : public LuceneObject {
public:
    DocState();
    virtual ~DocState();

    LUCENE_CLASS(DocState);

public:
    DocumentsWriterWeakPtr _docWriter;
    AnalyzerPtr analyzer;
    int32_t maxFieldLength;
    InfoStreamPtr infoStream;
    SimilarityPtr similarity;
    int32_t docID;
    DocumentPtr doc;
    String maxTermPrefix;

public:
    /// Only called by asserts
    virtual bool testPoint(const String& name);

    void clear();
};

/// RAMFile buffer for DocWriters.
class PerDocBuffer : public RAMFile {
public:
    PerDocBuffer(const DocumentsWriterPtr& docWriter);
    virtual ~PerDocBuffer();

    LUCENE_CLASS(PerDocBuffer);

protected:
    DocumentsWriterWeakPtr _docWriter;

public:
    /// Recycle the bytes used.
    void recycle();

protected:
    /// Allocate bytes used from shared pool.
    virtual ByteArray newBuffer(int32_t size);
};

/// Consumer returns this on each doc.  This holds any state that must be flushed synchronized
/// "in docID order".  We gather these and flush them in order.
class DocWriter : public LuceneObject {
public:
    DocWriter();
    virtual ~DocWriter();

    LUCENE_CLASS(DocWriter);

public:
    DocWriterPtr next;
    int32_t docID;

public:
    virtual void finish() = 0;
    virtual void abort() = 0;
    virtual int64_t sizeInBytes() = 0;

    virtual void setNext(const DocWriterPtr& next);
};

/// The IndexingChain must define the {@link #getChain(DocumentsWriter)} method which returns the DocConsumer
/// that the DocumentsWriter calls to process the documents.
class IndexingChain : public LuceneObject {
public:
    virtual ~IndexingChain();

    LUCENE_CLASS(IndexingChain);

public:
    virtual DocConsumerPtr getChain(const DocumentsWriterPtr& documentsWriter) = 0;
};

/// This is the current indexing chain:
/// DocConsumer / DocConsumerPerThread
///   --> code: DocFieldProcessor / DocFieldProcessorPerThread
///     --> DocFieldConsumer / DocFieldConsumerPerThread / DocFieldConsumerPerField
///       --> code: DocFieldConsumers / DocFieldConsumersPerThread / DocFieldConsumersPerField
///         --> code: DocInverter / DocInverterPerThread / DocInverterPerField
///          --> InvertedDocConsumer / InvertedDocConsumerPerThread / InvertedDocConsumerPerField
///            --> code: TermsHash / TermsHashPerThread / TermsHashPerField
///              --> TermsHashConsumer / TermsHashConsumerPerThread / TermsHashConsumerPerField
///                --> code: FreqProxTermsWriter / FreqProxTermsWriterPerThread / FreqProxTermsWriterPerField
///                --> code: TermVectorsTermsWriter / TermVectorsTermsWriterPerThread / TermVectorsTermsWriterPerField
///          --> InvertedDocEndConsumer / InvertedDocConsumerPerThread / InvertedDocConsumerPerField
///            --> code: NormsWriter / NormsWriterPerThread / NormsWriterPerField
///        --> code: StoredFieldsWriter / StoredFieldsWriterPerThread / StoredFieldsWriterPerField
class DefaultIndexingChain : public IndexingChain {
public:
    virtual ~DefaultIndexingChain();

    LUCENE_CLASS(DefaultIndexingChain);

public:
    virtual DocConsumerPtr getChain(const DocumentsWriterPtr& documentsWriter);
};

class SkipDocWriter : public DocWriter {
public:
    virtual ~SkipDocWriter();

    LUCENE_CLASS(SkipDocWriter);

public:
    virtual void finish();
    virtual void abort();
    virtual int64_t sizeInBytes();
};

class WaitQueue : public LuceneObject {
public:
    WaitQueue(const DocumentsWriterPtr& docWriter);
    virtual ~WaitQueue();

    LUCENE_CLASS(WaitQueue);

protected:
    DocumentsWriterWeakPtr _docWriter;

public:
    Collection<DocWriterPtr> waiting;
    int32_t nextWriteDocID;
    int32_t nextWriteLoc;
    int32_t numWaiting;
    int64_t waitingBytes;

public:
    void reset();
    bool doResume();
    bool doPause();
    void abort();
    bool add(const DocWriterPtr& doc);

protected:
    void writeDocument(const DocWriterPtr& doc);
};

class ByteBlockAllocator : public ByteBlockPoolAllocatorBase {
public:
    ByteBlockAllocator(const DocumentsWriterPtr& docWriter, int32_t blockSize);
    virtual ~ByteBlockAllocator();

    LUCENE_CLASS(ByteBlockAllocator);

protected:
    DocumentsWriterWeakPtr _docWriter;

public:
    int32_t blockSize;
    Collection<ByteArray> freeByteBlocks;

public:
    /// Allocate another byte[] from the shared pool
    virtual ByteArray getByteBlock(bool trackAllocations);

    /// Return byte[]'s to the pool
    virtual void recycleByteBlocks(Collection<ByteArray> blocks, int32_t start, int32_t end);
    virtual void recycleByteBlocks(Collection<ByteArray> blocks);
};

}

#endif
