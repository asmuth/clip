/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "DocumentsWriter.h"
#include "DocumentsWriterThreadState.h"
#include "LuceneThread.h"
#include "IndexWriter.h"
#include "_IndexWriter.h"
#include "IndexReader.h"
#include "IndexSearcher.h"
#include "DocFieldProcessor.h"
#include "Term.h"
#include "TermDocs.h"
#include "TermVectorsTermsWriter.h"
#include "FreqProxTermsWriter.h"
#include "TermsHashConsumer.h"
#include "InvertedDocConsumer.h"
#include "TermsHash.h"
#include "DocInverter.h"
#include "NormsWriter.h"
#include "BufferedDeletes.h"
#include "FieldInfos.h"
#include "InfoStream.h"
#include "DocConsumerPerThread.h"
#include "SegmentWriteState.h"
#include "IndexFileNames.h"
#include "CompoundFileWriter.h"
#include "MergeDocIDRemapper.h"
#include "SegmentReader.h"
#include "SegmentInfos.h"
#include "SegmentInfo.h"
#include "Query.h"
#include "Weight.h"
#include "Scorer.h"
#include "TestPoint.h"
#include "MiscUtils.h"
#include "StringUtils.h"

namespace Lucene {

/// Max # ThreadState instances; if there are more threads than this they share ThreadStates
const int32_t DocumentsWriter::MAX_THREAD_STATE = 5;

/// Coarse estimates used to measure RAM usage of buffered deletes
const int32_t DocumentsWriter::OBJECT_HEADER_BYTES = 8;
#ifdef LPP_BUILD_64
const int32_t DocumentsWriter::POINTER_NUM_BYTE = 8;
#else
const int32_t DocumentsWriter::POINTER_NUM_BYTE = 4;
#endif
const int32_t DocumentsWriter::INT_NUM_BYTE = 4;
#ifdef LPP_UNICODE_CHAR_SIZE_4
const int32_t DocumentsWriter::CHAR_NUM_BYTE = 4;
#else
const int32_t DocumentsWriter::CHAR_NUM_BYTE = 2;
#endif

/// Rough logic: HashMap has an array[Entry] with varying load factor (say 2 * POINTER).  Entry is object
/// with Term key, BufferedDeletes.Num val, int hash, Entry next (OBJ_HEADER + 3*POINTER + INT).  Term is
/// object with String field and String text (OBJ_HEADER + 2*POINTER).  We don't count Term's field since
/// it's interned.  Term's text is String (OBJ_HEADER + 4*INT + POINTER + OBJ_HEADER + string.length*CHAR).
/// BufferedDeletes.num is OBJ_HEADER + INT.
const int32_t DocumentsWriter::BYTES_PER_DEL_TERM = 8 * DocumentsWriter::POINTER_NUM_BYTE + 5 *
        DocumentsWriter::OBJECT_HEADER_BYTES + 6 *
        DocumentsWriter::INT_NUM_BYTE;

/// Rough logic: del docIDs are List<Integer>.  Say list allocates ~2X size (2*POINTER).  Integer is
/// OBJ_HEADER + int
const int32_t DocumentsWriter::BYTES_PER_DEL_DOCID = 2 * DocumentsWriter::POINTER_NUM_BYTE +
        DocumentsWriter::OBJECT_HEADER_BYTES +
        DocumentsWriter::INT_NUM_BYTE;

/// Rough logic: HashMap has an array[Entry] with varying load factor (say 2 * POINTER).  Entry is object
/// with Query key, Integer val, int hash, Entry next (OBJ_HEADER + 3*POINTER + INT).  Query we often undercount
/// (say 24 bytes).  Integer is OBJ_HEADER + INT.
const int32_t DocumentsWriter::BYTES_PER_DEL_QUERY = 5 * DocumentsWriter::POINTER_NUM_BYTE + 2 *
        DocumentsWriter::OBJECT_HEADER_BYTES + 2 *
        DocumentsWriter::INT_NUM_BYTE + 24;

/// Initial chunks size of the shared byte[] blocks used to store postings data
const int32_t DocumentsWriter::BYTE_BLOCK_SHIFT = 15;
const int32_t DocumentsWriter::BYTE_BLOCK_SIZE = 1 << DocumentsWriter::BYTE_BLOCK_SHIFT;
const int32_t DocumentsWriter::BYTE_BLOCK_MASK = DocumentsWriter::BYTE_BLOCK_SIZE - 1;
const int32_t DocumentsWriter::BYTE_BLOCK_NOT_MASK = ~DocumentsWriter::BYTE_BLOCK_MASK;

/// Initial chunk size of the shared char[] blocks used to store term text
const int32_t DocumentsWriter::CHAR_BLOCK_SHIFT = 14;
const int32_t DocumentsWriter::CHAR_BLOCK_SIZE = 1 << DocumentsWriter::CHAR_BLOCK_SHIFT;
const int32_t DocumentsWriter::CHAR_BLOCK_MASK = DocumentsWriter::CHAR_BLOCK_SIZE - 1;

const int32_t DocumentsWriter::MAX_TERM_LENGTH = DocumentsWriter::CHAR_BLOCK_SIZE - 1;

/// Initial chunks size of the shared int[] blocks used to store postings data
const int32_t DocumentsWriter::INT_BLOCK_SHIFT = 13;
const int32_t DocumentsWriter::INT_BLOCK_SIZE = 1 << DocumentsWriter::INT_BLOCK_SHIFT;
const int32_t DocumentsWriter::INT_BLOCK_MASK = DocumentsWriter::INT_BLOCK_SIZE - 1;

const int32_t DocumentsWriter::PER_DOC_BLOCK_SIZE = 1024;

DocumentsWriter::DocumentsWriter(const DirectoryPtr& directory,  const IndexWriterPtr& writer, const IndexingChainPtr& indexingChain) {
    this->threadStates = Collection<DocumentsWriterThreadStatePtr>::newInstance();
    this->threadBindings = MapThreadDocumentsWriterThreadState::newInstance();
    this->_openFiles = HashSet<String>::newInstance();
    this->_closedFiles = HashSet<String>::newInstance();
    this->freeIntBlocks = Collection<IntArray>::newInstance();
    this->freeCharBlocks = Collection<CharArray>::newInstance();

    this->directory = directory;
    this->_writer = writer;
    this->indexingChain = indexingChain;
}

DocumentsWriter::~DocumentsWriter() {
}

void DocumentsWriter::initialize() {
    docStoreOffset = 0;
    nextDocID = 0;
    numDocsInRAM = 0;
    numDocsInStore = 0;
    pauseThreads = 0;
    flushPending = false;
    bufferIsFull = false;
    aborting = false;
    maxFieldLength = IndexWriter::DEFAULT_MAX_FIELD_LENGTH;
    deletesInRAM = newLucene<BufferedDeletes>(false);
    deletesFlushed = newLucene<BufferedDeletes>(true);
    maxBufferedDeleteTerms = IndexWriter::DEFAULT_MAX_BUFFERED_DELETE_TERMS;
    ramBufferSize = (int64_t)(IndexWriter::DEFAULT_RAM_BUFFER_SIZE_MB * 1024 * 1024);
    waitQueuePauseBytes = (int64_t)((double)ramBufferSize * 0.1);
    waitQueueResumeBytes = (int64_t)((double)ramBufferSize * 0.05);
    freeTrigger = (int64_t)(IndexWriter::DEFAULT_RAM_BUFFER_SIZE_MB * 1024.0 * 1024.0 * 1.05);
    freeLevel = (int64_t)(IndexWriter::DEFAULT_RAM_BUFFER_SIZE_MB * 1024.0 * 1024.0 * 0.95);
    maxBufferedDocs = IndexWriter::DEFAULT_MAX_BUFFERED_DOCS;
    flushedDocCount = 0;
    closed = false;
    waitQueue = newLucene<WaitQueue>(shared_from_this());
    skipDocWriter = newLucene<SkipDocWriter>();
    numBytesAlloc = 0;
    numBytesUsed = 0;
    byteBlockAllocator = newLucene<ByteBlockAllocator>(shared_from_this(), BYTE_BLOCK_SIZE);
    perDocAllocator = newLucene<ByteBlockAllocator>(shared_from_this(), PER_DOC_BLOCK_SIZE);

    IndexWriterPtr writer(_writer);
    this->similarity = writer->getSimilarity();
    flushedDocCount = writer->maxDoc();

    consumer = indexingChain->getChain(shared_from_this());
    docFieldProcessor = boost::dynamic_pointer_cast<DocFieldProcessor>(consumer);
}

PerDocBufferPtr DocumentsWriter::newPerDocBuffer() {
    return newLucene<PerDocBuffer>(shared_from_this());
}

IndexingChainPtr DocumentsWriter::getDefaultIndexingChain() {
    static DefaultIndexingChainPtr defaultIndexingChain;
    if (!defaultIndexingChain) {
        defaultIndexingChain = newLucene<DefaultIndexingChain>();
        CycleCheck::addStatic(defaultIndexingChain);
    }
    return defaultIndexingChain;
}

void DocumentsWriter::updateFlushedDocCount(int32_t n) {
    SyncLock syncLock(this);
    flushedDocCount += n;
}

int32_t DocumentsWriter::getFlushedDocCount() {
    SyncLock syncLock(this);
    return flushedDocCount;
}

void DocumentsWriter::setFlushedDocCount(int32_t n) {
    SyncLock syncLock(this);
    flushedDocCount = n;
}

bool DocumentsWriter::hasProx() {
    return docFieldProcessor ? docFieldProcessor->fieldInfos->hasProx() : true;
}

void DocumentsWriter::setInfoStream(const InfoStreamPtr& infoStream) {
    SyncLock syncLock(this);
    this->infoStream = infoStream;
    for (Collection<DocumentsWriterThreadStatePtr>::iterator threadState = threadStates.begin(); threadState != threadStates.end(); ++threadState) {
        (*threadState)->docState->infoStream = infoStream;
    }
}

void DocumentsWriter::setMaxFieldLength(int32_t maxFieldLength) {
    SyncLock syncLock(this);
    this->maxFieldLength = maxFieldLength;
    for (Collection<DocumentsWriterThreadStatePtr>::iterator threadState = threadStates.begin(); threadState != threadStates.end(); ++threadState) {
        (*threadState)->docState->maxFieldLength = maxFieldLength;
    }
}

void DocumentsWriter::setSimilarity(const SimilarityPtr& similarity) {
    SyncLock syncLock(this);
    this->similarity = similarity;
    for (Collection<DocumentsWriterThreadStatePtr>::iterator threadState = threadStates.begin(); threadState != threadStates.end(); ++threadState) {
        (*threadState)->docState->similarity = similarity;
    }
}

void DocumentsWriter::setRAMBufferSizeMB(double mb) {
    SyncLock syncLock(this);
    if (mb == IndexWriter::DISABLE_AUTO_FLUSH) {
        ramBufferSize = IndexWriter::DISABLE_AUTO_FLUSH;
        waitQueuePauseBytes = 4 * 1024 * 1024;
        waitQueueResumeBytes = 2 * 1024 * 1024;
    } else {
        ramBufferSize = (int64_t)(mb * 1024.0 * 1024.0);
        waitQueuePauseBytes = (int64_t)((double)ramBufferSize * 0.1);
        waitQueueResumeBytes = (int64_t)((double)ramBufferSize * 0.05);
        freeTrigger = (int64_t)(1.05 * (double)ramBufferSize);
        freeLevel = (int64_t)(0.95 * (double)ramBufferSize);
    }
}

double DocumentsWriter::getRAMBufferSizeMB() {
    SyncLock syncLock(this);
    if (ramBufferSize == IndexWriter::DISABLE_AUTO_FLUSH) {
        return (double)ramBufferSize;
    } else {
        return (double)ramBufferSize / 1024.0 / 1024.0;
    }
}

void DocumentsWriter::setMaxBufferedDocs(int32_t count) {
    maxBufferedDocs = count;
}

int32_t DocumentsWriter::getMaxBufferedDocs() {
    return maxBufferedDocs;
}

String DocumentsWriter::getSegment() {
    return segment;
}

int32_t DocumentsWriter::getNumDocsInRAM() {
    return numDocsInRAM;
}

String DocumentsWriter::getDocStoreSegment() {
    SyncLock syncLock(this);
    return docStoreSegment;
}

int32_t DocumentsWriter::getDocStoreOffset() {
    return docStoreOffset;
}

String DocumentsWriter::closeDocStore() {
    TestScope testScope(L"DocumentsWriter", L"closeDocStore");
    SyncLock syncLock(this);
    BOOST_ASSERT(allThreadsIdle());

    if (infoStream) {
        message(L"closeDocStore: " + StringUtils::toString(_openFiles.size()) + L" files to flush to segment " +
                docStoreSegment + L" numDocs=" + StringUtils::toString(numDocsInStore));
    }

    bool success = false;
    LuceneException finally;
    String s;
    try {
        initFlushState(true);
        _closedFiles.clear();

        consumer->closeDocStore(flushState);
        BOOST_ASSERT(_openFiles.empty());

        s = docStoreSegment;
        docStoreSegment.clear();
        docStoreOffset = 0;
        numDocsInStore = 0;
        success = true;
    } catch (LuceneException& e) {
        finally = e;
    }
    if (!success) {
        abort();
    }
    finally.throwException();
    return s;
}

HashSet<String> DocumentsWriter::abortedFiles() {
    return _abortedFiles;
}

void DocumentsWriter::message(const String& message) {
    if (infoStream) {
        *infoStream << L"DW " << message << L"\n";
    }
}

HashSet<String> DocumentsWriter::openFiles() {
    SyncLock syncLock(this);
    return HashSet<String>::newInstance(_openFiles.begin(), _openFiles.end());
}

HashSet<String> DocumentsWriter::closedFiles() {
    SyncLock syncLock(this);
    return HashSet<String>::newInstance(_closedFiles.begin(), _closedFiles.end());
}

void DocumentsWriter::addOpenFile(const String& name) {
    SyncLock syncLock(this);
    BOOST_ASSERT(!_openFiles.contains(name));
    _openFiles.add(name);
}

void DocumentsWriter::removeOpenFile(const String& name) {
    SyncLock syncLock(this);
    BOOST_ASSERT(_openFiles.contains(name));
    _openFiles.remove(name);
    _closedFiles.add(name);
}

void DocumentsWriter::setAborting() {
    SyncLock syncLock(this);
    aborting = true;
}

void DocumentsWriter::abort() {
    TestScope testScope(L"DocumentsWriter", L"abort");
    SyncLock syncLock(this);
    LuceneException finally;
    try {
        if (infoStream) {
            message(L"docWriter: now abort");
        }

        // Forcefully remove waiting ThreadStates from line
        waitQueue->abort();

        // Wait for all other threads to finish with DocumentsWriter
        pauseAllThreads();

        try {
            BOOST_ASSERT(waitQueue->numWaiting == 0);

            waitQueue->waitingBytes = 0;

            try {
                _abortedFiles = openFiles();
            } catch (...) {
                _abortedFiles.reset();
            }

            deletesInRAM->clear();
            deletesFlushed->clear();
            _openFiles.clear();

            for (Collection<DocumentsWriterThreadStatePtr>::iterator threadState = threadStates.begin(); threadState != threadStates.end(); ++threadState) {
                try {
                    (*threadState)->consumer->abort();
                } catch (...) {
                }
            }

            try {
                consumer->abort();
            } catch (...) {
            }

            docStoreSegment.clear();
            numDocsInStore = 0;
            docStoreOffset = 0;

            // Reset all postings data
            doAfterFlush();
        } catch (LuceneException& e) {
            finally = e;
        }
        resumeAllThreads();
    } catch (LuceneException& e) {
        if (finally.isNull()) {
            finally = e;
        }
    }
    aborting = false;
    notifyAll();
    if (infoStream) {
        message(L"docWriter: done abort");
    }
    finally.throwException();
}

void DocumentsWriter::doAfterFlush() {
    // All ThreadStates should be idle when we are called
    BOOST_ASSERT(allThreadsIdle());
    threadBindings.clear();
    waitQueue->reset();
    segment.clear();
    numDocsInRAM = 0;
    nextDocID = 0;
    bufferIsFull = false;
    flushPending = false;
    for (Collection<DocumentsWriterThreadStatePtr>::iterator threadState = threadStates.begin(); threadState != threadStates.end(); ++threadState) {
        (*threadState)->doAfterFlush();
    }
    numBytesUsed = 0;
}

bool DocumentsWriter::pauseAllThreads() {
    SyncLock syncLock(this);
    ++pauseThreads;
    while (!allThreadsIdle()) {
        wait(1000);
    }
    return aborting;
}

void DocumentsWriter::resumeAllThreads() {
    SyncLock syncLock(this);
    --pauseThreads;
    BOOST_ASSERT(pauseThreads >= 0);
    if (pauseThreads == 0) {
        notifyAll();
    }
}

bool DocumentsWriter::allThreadsIdle() {
    SyncLock syncLock(this);
    for (Collection<DocumentsWriterThreadStatePtr>::iterator threadState = threadStates.begin(); threadState != threadStates.end(); ++threadState) {
        if (!(*threadState)->isIdle) {
            return false;
        }
    }
    return true;
}

bool DocumentsWriter::anyChanges() {
    SyncLock syncLock(this);
    return (numDocsInRAM != 0 || deletesInRAM->numTerms != 0 || !deletesInRAM->docIDs.empty() || !deletesInRAM->queries.empty());
}

void DocumentsWriter::initFlushState(bool onlyDocStore) {
    SyncLock syncLock(this);
    initSegmentName(onlyDocStore);
    flushState = newLucene<SegmentWriteState>(shared_from_this(), directory, segment, docStoreSegment, numDocsInRAM, numDocsInStore, IndexWriterPtr(_writer)->getTermIndexInterval());
}

int32_t DocumentsWriter::flush(bool _closeDocStore) {
    SyncLock syncLock(this);
    BOOST_ASSERT(allThreadsIdle());

    BOOST_ASSERT(numDocsInRAM > 0);

    BOOST_ASSERT(nextDocID == numDocsInRAM);
    BOOST_ASSERT(waitQueue->numWaiting == 0);
    BOOST_ASSERT(waitQueue->waitingBytes == 0);

    initFlushState(false);

    docStoreOffset = numDocsInStore;

    if (infoStream) {
        message(L"flush postings as segment " + flushState->segmentName + L" numDocs=" + StringUtils::toString(numDocsInRAM));
    }

    bool success = false;
    LuceneException finally;

    try {
        if (_closeDocStore) {
            BOOST_ASSERT(!flushState->docStoreSegmentName.empty());
            BOOST_ASSERT(flushState->docStoreSegmentName == flushState->segmentName);

            closeDocStore();
            flushState->numDocsInStore = 0;
        }

        Collection<DocConsumerPerThreadPtr> threads(Collection<DocConsumerPerThreadPtr>::newInstance());
        for (Collection<DocumentsWriterThreadStatePtr>::iterator threadState = threadStates.begin(); threadState != threadStates.end(); ++threadState) {
            threads.add((*threadState)->consumer);
        }
        consumer->flush(threads, flushState);

        if (infoStream) {
            SegmentInfoPtr si(newLucene<SegmentInfo>(flushState->segmentName, flushState->numDocs, directory));
            int64_t newSegmentSize = si->sizeInBytes();
            if (infoStream) {
                message(L"  oldRAMSize=" + StringUtils::toString(numBytesUsed) + L" newFlushedSize=" +
                        StringUtils::toString(newSegmentSize) + L" docs/MB=" +
                        StringUtils::toString((double)numDocsInRAM / ((double)newSegmentSize / 1024.0 / 1024.0)) +
                        L" new/old=" + StringUtils::toString(100.0 * (double)newSegmentSize / (double)numBytesUsed) + L"%");
            }
        }

        flushedDocCount += flushState->numDocs;

        doAfterFlush();

        success = true;
    } catch (LuceneException& e) {
        finally = e;
    }
    if (!success) {
        abort();
    }
    finally.throwException();

    BOOST_ASSERT(waitQueue->waitingBytes == 0);

    return flushState->numDocs;
}

HashSet<String> DocumentsWriter::getFlushedFiles() {
    return flushState->flushedFiles;
}

void DocumentsWriter::createCompoundFile(const String& segment) {
    CompoundFileWriterPtr cfsWriter(newLucene<CompoundFileWriter>(directory, segment + L"." + IndexFileNames::COMPOUND_FILE_EXTENSION()));
    for (HashSet<String>::iterator flushedFile = flushState->flushedFiles.begin(); flushedFile != flushState->flushedFiles.end(); ++flushedFile) {
        cfsWriter->addFile(*flushedFile);
    }

    // Perform the merge
    cfsWriter->close();
}

bool DocumentsWriter::setFlushPending() {
    SyncLock syncLock(this);
    if (flushPending) {
        return false;
    } else {
        flushPending = true;
        return true;
    }
}

void DocumentsWriter::clearFlushPending() {
    SyncLock syncLock(this);
    flushPending = false;
}

void DocumentsWriter::pushDeletes() {
    SyncLock syncLock(this);
    deletesFlushed->update(deletesInRAM);
}

void DocumentsWriter::close() {
    SyncLock syncLock(this);
    closed = true;
    notifyAll();
}

void DocumentsWriter::initSegmentName(bool onlyDocStore) {
    SyncLock syncLock(this);
    if (segment.empty() && (!onlyDocStore || docStoreSegment.empty())) {
        segment = IndexWriterPtr(_writer)->newSegmentName();
        BOOST_ASSERT(numDocsInRAM == 0);
    }
    if (docStoreSegment.empty()) {
        docStoreSegment = segment;
        BOOST_ASSERT(numDocsInStore == 0);
    }
}

DocumentsWriterThreadStatePtr DocumentsWriter::getThreadState(const DocumentPtr& doc, const TermPtr& delTerm) {
    SyncLock syncLock(this);
    // First, find a thread state.  If this thread already has affinity to a specific ThreadState, use that one again.
    DocumentsWriterThreadStatePtr state(threadBindings.get(LuceneThread::currentId()));
    if (!state) {
        // First time this thread has called us since last flush.  Find the least loaded thread state
        DocumentsWriterThreadStatePtr minThreadState;
        for (Collection<DocumentsWriterThreadStatePtr>::iterator threadState = threadStates.begin(); threadState != threadStates.end(); ++threadState) {
            if (!minThreadState || (*threadState)->numThreads < minThreadState->numThreads) {
                minThreadState = *threadState;
            }
        }
        if (minThreadState && (minThreadState->numThreads == 0 || threadStates.size() >= MAX_THREAD_STATE)) {
            state = minThreadState;
            ++state->numThreads;
        } else {
            // Just create a new "private" thread state
            threadStates.resize(threadStates.size() + 1);
            state = newLucene<DocumentsWriterThreadState>(shared_from_this());
            threadStates[threadStates.size() - 1] = state;
        }
        threadBindings.put(LuceneThread::currentId(), state);
    }

    // Next, wait until my thread state is idle (in case it's shared with other threads) and for threads to
    // not be paused nor a flush pending
    waitReady(state);

    // Allocate segment name if this is the first doc since last flush
    initSegmentName(false);

    state->isIdle = false;

    bool success = false;
    LuceneException finally;
    try {
        state->docState->docID = nextDocID;

        BOOST_ASSERT(IndexWriterPtr(_writer)->testPoint(L"DocumentsWriter.ThreadState.init start"));

        if (delTerm) {
            addDeleteTerm(delTerm, state->docState->docID);
            state->doFlushAfter = timeToFlushDeletes();
        }

        BOOST_ASSERT(IndexWriterPtr(_writer)->testPoint(L"DocumentsWriter.ThreadState.init after delTerm"));

        ++nextDocID;
        ++numDocsInRAM;

        // We must at this point commit to flushing to ensure we always get N docs when we flush by doc
        // count, even if > 1 thread is adding documents
        if (!flushPending && maxBufferedDocs != IndexWriter::DISABLE_AUTO_FLUSH && numDocsInRAM >= maxBufferedDocs) {
            flushPending = true;
            state->doFlushAfter = true;
        }

        success = true;
    } catch (LuceneException& e) {
        finally = e;
    }
    if (!success) {
        // Forcefully idle this ThreadState
        state->isIdle = true;
        notifyAll();
        if (state->doFlushAfter) {
            state->doFlushAfter = false;
            flushPending = false;
        }
    }
    finally.throwException();

    return state;
}

bool DocumentsWriter::addDocument(const DocumentPtr& doc, const AnalyzerPtr& analyzer) {
    return updateDocument(doc, analyzer, TermPtr());
}

bool DocumentsWriter::updateDocument(const TermPtr& t, const DocumentPtr& doc, const AnalyzerPtr& analyzer) {
    return updateDocument(doc, analyzer, t);
}

bool DocumentsWriter::updateDocument(const DocumentPtr& doc, const AnalyzerPtr& analyzer, const TermPtr& delTerm) {
    // This call is synchronized but fast
    DocumentsWriterThreadStatePtr state(getThreadState(doc, delTerm));

    DocStatePtr docState(state->docState);
    docState->doc = doc;
    docState->analyzer = analyzer;

    bool success = false;
    LuceneException finally;
    try {
        // This call is not synchronized and does all the work
        DocWriterPtr perDoc;
        try {
            perDoc = state->consumer->processDocument();
        } catch (LuceneException& e) {
            finally = e;
        }
        docState->clear();
        finally.throwException();

        // This call is synchronized but fast
        finishDocument(state, perDoc);

        success = true;
    } catch (LuceneException& e) {
        finally = e;
    }
    if (!success) {
        SyncLock syncLock(this);
        if (aborting) {
            state->isIdle = true;
            notifyAll();
            abort();
        } else {
            skipDocWriter->docID = docState->docID;
            bool success2 = false;
            try {
                waitQueue->add(skipDocWriter);
                success2 = true;
            } catch (LuceneException& e) {
                finally = e;
            }
            if (!success2) {
                state->isIdle = true;
                notifyAll();
                abort();
                return false;
            }

            state->isIdle = true;
            notifyAll();

            // If this thread state had decided to flush, we must clear it so another thread can flush
            if (state->doFlushAfter) {
                state->doFlushAfter = false;
                flushPending = false;
                notifyAll();
            }

            // Immediately mark this document as deleted since likely it was partially added.  This keeps
            // indexing as "all or none" (atomic) when adding a document
            addDeleteDocID(state->docState->docID);
        }
    }

    finally.throwException();

    return (state->doFlushAfter || timeToFlushDeletes());
}

int32_t DocumentsWriter::getNumBufferedDeleteTerms() {
    SyncLock syncLock(this);
    return deletesInRAM->numTerms;
}

MapTermNum DocumentsWriter::getBufferedDeleteTerms() {
    SyncLock syncLock(this);
    return deletesInRAM->terms;
}

void DocumentsWriter::remapDeletes(const SegmentInfosPtr& infos, Collection< Collection<int32_t> > docMaps, Collection<int32_t> delCounts, const OneMergePtr& merge, int32_t mergeDocCount) {
    SyncLock syncLock(this);
    if (!docMaps) {
        // The merged segments had no deletes so docIDs did not change and we have nothing to do
        return;
    }
    MergeDocIDRemapperPtr mapper(newLucene<MergeDocIDRemapper>(infos, docMaps, delCounts, merge, mergeDocCount));
    deletesInRAM->remap(mapper, infos, docMaps, delCounts, merge, mergeDocCount);
    deletesFlushed->remap(mapper, infos, docMaps, delCounts, merge, mergeDocCount);
    flushedDocCount -= mapper->docShift;
}

void DocumentsWriter::waitReady(const DocumentsWriterThreadStatePtr& state) {
    SyncLock syncLock(this);
    while (!closed && ((state && !state->isIdle) || pauseThreads != 0 || flushPending || aborting)) {
        wait(1000);
    }
    if (closed) {
        boost::throw_exception(AlreadyClosedException(L"this IndexWriter is closed"));
    }
}

bool DocumentsWriter::bufferDeleteTerms(Collection<TermPtr> terms) {
    SyncLock syncLock(this);
    waitReady(DocumentsWriterThreadStatePtr());
    for (Collection<TermPtr>::iterator term = terms.begin(); term != terms.end(); ++term) {
        addDeleteTerm(*term, numDocsInRAM);
    }
    return timeToFlushDeletes();
}

bool DocumentsWriter::bufferDeleteTerm(const TermPtr& term) {
    SyncLock syncLock(this);
    waitReady(DocumentsWriterThreadStatePtr());
    addDeleteTerm(term, numDocsInRAM);
    return timeToFlushDeletes();
}

bool DocumentsWriter::bufferDeleteQueries(Collection<QueryPtr> queries) {
    SyncLock syncLock(this);
    waitReady(DocumentsWriterThreadStatePtr());
    for (Collection<QueryPtr>::iterator query = queries.begin(); query != queries.end(); ++query) {
        addDeleteQuery(*query, numDocsInRAM);
    }
    return timeToFlushDeletes();
}

bool DocumentsWriter::bufferDeleteQuery(const QueryPtr& query) {
    SyncLock syncLock(this);
    waitReady(DocumentsWriterThreadStatePtr());
    addDeleteQuery(query, numDocsInRAM);
    return timeToFlushDeletes();
}

bool DocumentsWriter::deletesFull() {
    SyncLock syncLock(this);
    return ((ramBufferSize != IndexWriter::DISABLE_AUTO_FLUSH &&
             (deletesInRAM->bytesUsed + deletesFlushed->bytesUsed + numBytesUsed) >= ramBufferSize) ||
            (maxBufferedDeleteTerms != IndexWriter::DISABLE_AUTO_FLUSH &&
             ((deletesInRAM->size() + deletesFlushed->size()) >= maxBufferedDeleteTerms)));
}

bool DocumentsWriter::doApplyDeletes() {
    SyncLock syncLock(this);
    // Very similar to deletesFull(), except we don't count numBytesAlloc, because we are checking whether
    // deletes (alone) are consuming too many resources now and thus should be applied.  We apply deletes
    // if RAM usage is > 1/2 of our allowed RAM buffer, to prevent too-frequent flushing of a long tail of
    // tiny segments when merges (which always apply deletes) are infrequent.
    return ((ramBufferSize != IndexWriter::DISABLE_AUTO_FLUSH &&
             (deletesInRAM->bytesUsed + deletesFlushed->bytesUsed) >= ramBufferSize / 2) ||
            (maxBufferedDeleteTerms != IndexWriter::DISABLE_AUTO_FLUSH &&
             ((deletesInRAM->size() + deletesFlushed->size()) >= maxBufferedDeleteTerms)));
}

bool DocumentsWriter::timeToFlushDeletes() {
    SyncLock syncLock(this);
    return ((bufferIsFull || deletesFull()) && setFlushPending());
}

bool DocumentsWriter::checkDeleteTerm(const TermPtr& term) {
    if (term) {
        BOOST_ASSERT(!lastDeleteTerm || term->compareTo(lastDeleteTerm) > 0);
    }
    lastDeleteTerm = term;
    return true;
}

void DocumentsWriter::setMaxBufferedDeleteTerms(int32_t maxBufferedDeleteTerms) {
    this->maxBufferedDeleteTerms = maxBufferedDeleteTerms;
}

int32_t DocumentsWriter::getMaxBufferedDeleteTerms() {
    return maxBufferedDeleteTerms;
}

bool DocumentsWriter::hasDeletes() {
    SyncLock syncLock(this);
    return deletesFlushed->any();
}

bool DocumentsWriter::applyDeletes(const SegmentInfosPtr& infos) {
    SyncLock syncLock(this);
    if (!hasDeletes()) {
        return false;
    }

    if (infoStream) {
        message(L"apply " + StringUtils::toString(deletesFlushed->numTerms) + L" buffered deleted terms and " +
                StringUtils::toString(deletesFlushed->docIDs.size()) + L" deleted docIDs and " +
                StringUtils::toString(deletesFlushed->queries.size()) + L" deleted queries on " +
                StringUtils::toString(infos->size()) + L" segments.");
    }

    int32_t infosEnd = infos->size();

    int32_t docStart = 0;
    bool any = false;
    IndexWriterPtr writer(_writer);

    for (int32_t i = 0; i < infosEnd; ++i) {
        // Make sure we never attempt to apply deletes to segment in external dir
        BOOST_ASSERT(infos->info(i)->dir == directory);

        SegmentReaderPtr reader(writer->readerPool->get(infos->info(i), false));
        LuceneException finally;
        try {
            if (applyDeletes(reader, docStart)) {
                any = true;
            }
            docStart += reader->maxDoc();
        } catch (LuceneException& e) {
            finally = e;
        }
        writer->readerPool->release(reader);
        finally.throwException();
    }

    deletesFlushed->clear();

    return any;
}

bool DocumentsWriter::applyDeletes(const IndexReaderPtr& reader, int32_t docIDStart) {
    SyncLock syncLock(this);
    int32_t docEnd = docIDStart + reader->maxDoc();
    bool any = false;

    BOOST_ASSERT(checkDeleteTerm(TermPtr()));

    // Delete by term
    TermDocsPtr docs(reader->termDocs());
    LuceneException finally;
    try {
        for (MapTermNum::iterator entry = deletesFlushed->terms.begin(); entry != deletesFlushed->terms.end(); ++entry) {
            // we should be iterating a Map here, so terms better be in order
            BOOST_ASSERT(checkDeleteTerm(entry->first));
            docs->seek(entry->first);
            int32_t limit = entry->second->getNum();
            while (docs->next()) {
                int32_t docID = docs->doc();
                if (docIDStart + docID >= limit) {
                    break;
                }
                reader->deleteDocument(docID);
                any = true;
            }
        }
    } catch (LuceneException& e) {
        finally = e;
    }
    docs->close();
    finally.throwException();

    // Delete by docID
    for (Collection<int32_t>::iterator docID = deletesFlushed->docIDs.begin(); docID != deletesFlushed->docIDs.end(); ++docID) {
        if (*docID >= docIDStart && *docID < docEnd) {
            reader->deleteDocument(*docID - docIDStart);
            any = true;
        }
    }

    // Delete by query
    IndexSearcherPtr searcher(newLucene<IndexSearcher>(reader));
    for (MapQueryInt::iterator entry = deletesFlushed->queries.begin(); entry != deletesFlushed->queries.end(); ++entry) {
        WeightPtr weight(entry->first->weight(searcher));
        ScorerPtr scorer(weight->scorer(reader, true, false));
        if (scorer) {
            while (true) {
                int32_t doc = scorer->nextDoc();
                if ((int64_t)docIDStart + doc >= entry->second) {
                    break;
                }
                reader->deleteDocument(doc);
                any = true;
            }
        }
    }
    searcher->close();
    return any;
}

void DocumentsWriter::addDeleteTerm(const TermPtr& term, int32_t docCount) {
    SyncLock syncLock(this);
    NumPtr num(deletesInRAM->terms.get(term));
    int32_t docIDUpto = flushedDocCount + docCount;
    if (!num) {
        deletesInRAM->terms.put(term, newLucene<Num>(docIDUpto));
    } else {
        num->setNum(docIDUpto);
    }
    ++deletesInRAM->numTerms;

    deletesInRAM->addBytesUsed(BYTES_PER_DEL_TERM + term->_text.length() * CHAR_NUM_BYTE);
}

void DocumentsWriter::addDeleteDocID(int32_t docID) {
    SyncLock syncLock(this);
    deletesInRAM->docIDs.add(flushedDocCount + docID);
    deletesInRAM->addBytesUsed(BYTES_PER_DEL_DOCID);
}

void DocumentsWriter::addDeleteQuery(const QueryPtr& query, int32_t docID) {
    SyncLock syncLock(this);
    deletesInRAM->queries.put(query, flushedDocCount + docID);
    deletesInRAM->addBytesUsed(BYTES_PER_DEL_QUERY);
}

bool DocumentsWriter::doBalanceRAM() {
    SyncLock syncLock(this);
    return (ramBufferSize != IndexWriter::DISABLE_AUTO_FLUSH && !bufferIsFull &&
            (numBytesUsed + deletesInRAM->bytesUsed + deletesFlushed->bytesUsed >= ramBufferSize ||
             numBytesAlloc >= freeTrigger));
}

void DocumentsWriter::finishDocument(const DocumentsWriterThreadStatePtr& perThread, const DocWriterPtr& docWriter) {
    if (doBalanceRAM()) {
        // Must call this without holding synchronized(this) else we'll hit deadlock
        balanceRAM();
    }

    {
        SyncLock syncLock(this);
        BOOST_ASSERT(!docWriter || docWriter->docID == perThread->docState->docID);

        if (aborting) {
            // We are currently aborting, and another thread is waiting for me to become idle.  We
            // just forcefully idle this threadState; it will be fully reset by abort()
            if (docWriter) {
                try {
                    docWriter->abort();
                } catch (...) {
                }
            }

            perThread->isIdle = true;
            notifyAll();
            return;
        }

        bool doPause;

        if (docWriter) {
            doPause = waitQueue->add(docWriter);
        } else {
            skipDocWriter->docID = perThread->docState->docID;
            doPause = waitQueue->add(skipDocWriter);
        }

        if (doPause) {
            waitForWaitQueue();
        }

        if (bufferIsFull && !flushPending) {
            flushPending = true;
            perThread->doFlushAfter = true;
        }

        perThread->isIdle = true;
        notifyAll();
    }
}

void DocumentsWriter::waitForWaitQueue() {
    SyncLock syncLock(this);
    do {
        wait(1000);
    } while (!waitQueue->doResume());
}

int64_t DocumentsWriter::getRAMUsed() {
    return numBytesUsed + deletesInRAM->bytesUsed + deletesFlushed->bytesUsed;
}

IntArray DocumentsWriter::getIntBlock(bool trackAllocations) {
    SyncLock syncLock(this);
    int32_t size = freeIntBlocks.size();
    IntArray b;
    if (size == 0) {
        // Always record a block allocated, even if trackAllocations is false.  This is necessary because
        // this block will be shared between things that don't track allocations (term vectors) and things
        // that do (freq/prox postings).
        numBytesAlloc += INT_BLOCK_SIZE * INT_NUM_BYTE;
        b = IntArray::newInstance(INT_BLOCK_SIZE);
    } else {
        b = freeIntBlocks.removeLast();
    }
    if (trackAllocations) {
        numBytesUsed += INT_BLOCK_SIZE * INT_NUM_BYTE;
    }
    BOOST_ASSERT(numBytesUsed <= numBytesAlloc);
    return b;
}

void DocumentsWriter::bytesAllocated(int64_t numBytes) {
    SyncLock syncLock(this);
    numBytesAlloc += numBytes;
}

void DocumentsWriter::bytesUsed(int64_t numBytes) {
    SyncLock syncLock(this);
    numBytesUsed += numBytes;
    BOOST_ASSERT(numBytesUsed <= numBytesAlloc);
}

void DocumentsWriter::recycleIntBlocks(Collection<IntArray> blocks, int32_t start, int32_t end) {
    SyncLock syncLock(this);
    for (int32_t i = start; i < end; ++i) {
        freeIntBlocks.add(blocks[i]);
        blocks[i].reset();
    }
}

CharArray DocumentsWriter::getCharBlock() {
    SyncLock syncLock(this);
    int32_t size = freeCharBlocks.size();
    CharArray c;
    if (size == 0) {
        numBytesAlloc += CHAR_BLOCK_SIZE * CHAR_NUM_BYTE;
        c = CharArray::newInstance(CHAR_BLOCK_SIZE);
    } else {
        c = freeCharBlocks.removeLast();
    }
    // We always track allocations of char blocks for now because nothing that skips allocation tracking
    // (currently only term vectors) uses its own char blocks.
    numBytesUsed += CHAR_BLOCK_SIZE * CHAR_NUM_BYTE;
    BOOST_ASSERT(numBytesUsed <= numBytesAlloc);
    return c;
}

void DocumentsWriter::recycleCharBlocks(Collection<CharArray> blocks, int32_t numBlocks) {
    SyncLock syncLock(this);
    for (int32_t i = 0; i < numBlocks; ++i) {
        freeCharBlocks.add(blocks[i]);
        blocks[i].reset();
    }
}

String DocumentsWriter::toMB(int64_t v) {
    return StringUtils::toString((double)v / 1024.0 / 1024.0);
}

void DocumentsWriter::balanceRAM() {
    // We flush when we've used our target usage
    int64_t flushTrigger = ramBufferSize;

    int64_t deletesRAMUsed = deletesInRAM->bytesUsed + deletesFlushed->bytesUsed;

    if (numBytesAlloc + deletesRAMUsed > freeTrigger) {
        if (infoStream) {
            message(L"  RAM: now balance allocations: usedMB=" + toMB(numBytesUsed) +
                    L" vs trigger=" + toMB(flushTrigger) +
                    L" allocMB=" + toMB(numBytesAlloc) +
                    L" deletesMB=" + toMB(deletesRAMUsed) +
                    L" vs trigger=" + toMB(freeTrigger) +
                    L" byteBlockFree=" + toMB(byteBlockAllocator->freeByteBlocks.size() * BYTE_BLOCK_SIZE) +
                    L" perDocFree=" + toMB(perDocAllocator->freeByteBlocks.size() * PER_DOC_BLOCK_SIZE) +
                    L" charBlockFree=" + toMB(freeCharBlocks.size() * CHAR_BLOCK_SIZE * CHAR_NUM_BYTE));
        }

        int64_t startBytesAlloc = numBytesAlloc + deletesRAMUsed;

        int32_t iter = 0;

        // We free equally from each pool in 32 KB chunks until we are below our threshold (freeLevel)

        bool any = true;

        while (numBytesAlloc + deletesRAMUsed > freeLevel) {
            {
                SyncLock syncLock(this);
                if (perDocAllocator->freeByteBlocks.empty() && byteBlockAllocator->freeByteBlocks.empty() &&
                        freeCharBlocks.empty() && freeIntBlocks.empty() && !any) {
                    // Nothing else to free -- must flush now.
                    bufferIsFull = (numBytesUsed + deletesRAMUsed > flushTrigger);
                    if (infoStream) {
                        if (bufferIsFull) {
                            message(L"    nothing to free; now set bufferIsFull");
                        } else {
                            message(L"    nothing to free");
                        }
                    }
                    BOOST_ASSERT(numBytesUsed <= numBytesAlloc);
                    break;
                }

                if ((iter % 5) == 0 && !byteBlockAllocator->freeByteBlocks.empty()) {
                    byteBlockAllocator->freeByteBlocks.removeLast();
                    numBytesAlloc -= BYTE_BLOCK_SIZE;
                }

                if ((iter % 5) == 1 && !freeCharBlocks.empty()) {
                    freeCharBlocks.removeLast();
                    numBytesAlloc -= CHAR_BLOCK_SIZE * CHAR_NUM_BYTE;
                }

                if ((iter % 5) == 2 && !freeIntBlocks.empty()) {
                    freeIntBlocks.removeLast();
                    numBytesAlloc -= INT_BLOCK_SIZE * INT_NUM_BYTE;
                }

                if ((iter % 5) == 3 && !perDocAllocator->freeByteBlocks.empty()) {
                    // Remove upwards of 32 blocks (each block is 1K)
                    for (int32_t i = 0; i < 32; ++i) {
                        perDocAllocator->freeByteBlocks.removeLast();
                        numBytesAlloc -= PER_DOC_BLOCK_SIZE;
                        if (perDocAllocator->freeByteBlocks.empty()) {
                            break;
                        }
                    }
                }
            }

            if ((iter % 5) == 4 && any) {
                // Ask consumer to free any recycled state
                any = consumer->freeRAM();
            }

            ++iter;
        }

        if (infoStream) {
            message(L"    after free: freedMB=" + StringUtils::toString((double)(startBytesAlloc - numBytesAlloc - deletesRAMUsed) / 1024.0 / 1024.0) +
                    L" usedMB=" + StringUtils::toString((double)(numBytesUsed + deletesRAMUsed) / 1024.0 / 1024.0) +
                    L" allocMB=" + StringUtils::toString((double)numBytesAlloc / 1024.0 / 1024.0));
        }
    } else {
        // If we have not crossed the 100% mark, but have crossed the 95% mark of RAM we are actually
        // using, go ahead and flush.  This prevents over-allocating and then freeing, with every flush.
        SyncLock syncLock(this);
        if (numBytesUsed + deletesRAMUsed > flushTrigger) {
            if (infoStream) {
                message(L"  RAM: now flush @ usedMB=" + StringUtils::toString((double)numBytesUsed / 1024.0 / 1024.0) +
                        L" allocMB=" + StringUtils::toString((double)numBytesAlloc / 1024.0 / 1024.0) +
                        L" deletesMB=" + StringUtils::toString((double)deletesRAMUsed / 1024.0 / 1024.0) +
                        L" triggerMB=" + StringUtils::toString((double)flushTrigger / 1024.0 / 1024.0));
            }
            bufferIsFull = true;
        }
    }
}

DocState::DocState() {
    maxFieldLength = 0;
    docID = 0;
}

DocState::~DocState() {
}

bool DocState::testPoint(const String& name) {
    return IndexWriterPtr(DocumentsWriterPtr(_docWriter)->_writer)->testPoint(name);
}

void DocState::clear() {
    // don't hold onto doc nor analyzer, in case it is large
    doc.reset();
    analyzer.reset();
}

PerDocBuffer::PerDocBuffer(const DocumentsWriterPtr& docWriter) {
    _docWriter = docWriter;
}

PerDocBuffer::~PerDocBuffer() {
}

ByteArray PerDocBuffer::newBuffer(int32_t size) {
    BOOST_ASSERT(size == DocumentsWriter::PER_DOC_BLOCK_SIZE);
    return DocumentsWriterPtr(_docWriter)->perDocAllocator->getByteBlock(false);
}

void PerDocBuffer::recycle() {
    SyncLock syncLock(this);
    if (!buffers.empty()) {
        setLength(0);

        // Recycle the blocks
        DocumentsWriterPtr(_docWriter)->perDocAllocator->recycleByteBlocks(buffers);
        buffers.clear();
        sizeInBytes = 0;

        BOOST_ASSERT(numBuffers() == 0);
    }
}

DocWriter::DocWriter() {
    docID = 0;
}

DocWriter::~DocWriter() {
}

void DocWriter::setNext(const DocWriterPtr& next) {
    this->next = next;
}

IndexingChain::~IndexingChain() {
}

DefaultIndexingChain::~DefaultIndexingChain() {
}

DocConsumerPtr DefaultIndexingChain::getChain(const DocumentsWriterPtr& documentsWriter) {
    TermsHashConsumerPtr termVectorsWriter(newLucene<TermVectorsTermsWriter>(documentsWriter));
    TermsHashConsumerPtr freqProxWriter(newLucene<FreqProxTermsWriter>());

    InvertedDocConsumerPtr termsHash(newLucene<TermsHash>(documentsWriter, true, freqProxWriter,
                                     newLucene<TermsHash>(documentsWriter, false,
                                             termVectorsWriter, TermsHashPtr())));

    DocInverterPtr docInverter(newLucene<DocInverter>(termsHash, newLucene<NormsWriter>()));
    return newLucene<DocFieldProcessor>(documentsWriter, docInverter);
}

SkipDocWriter::~SkipDocWriter() {
}

void SkipDocWriter::finish() {
}

void SkipDocWriter::abort() {
}

int64_t SkipDocWriter::sizeInBytes() {
    return 0;
}

WaitQueue::WaitQueue(const DocumentsWriterPtr& docWriter) {
    this->_docWriter = docWriter;
    waiting = Collection<DocWriterPtr>::newInstance(10);
    nextWriteDocID = 0;
    nextWriteLoc = 0;
    numWaiting = 0;
    waitingBytes = 0;
}

WaitQueue::~WaitQueue() {
}

void WaitQueue::reset() {
    SyncLock syncLock(this);
    // NOTE: nextWriteLoc doesn't need to be reset
    BOOST_ASSERT(numWaiting == 0);
    BOOST_ASSERT(waitingBytes == 0);
    nextWriteDocID = 0;
}

bool WaitQueue::doResume() {
    SyncLock syncLock(this);
    return (waitingBytes <= DocumentsWriterPtr(_docWriter)->waitQueueResumeBytes);
}

bool WaitQueue::doPause() {
    SyncLock syncLock(this);
    return (waitingBytes > DocumentsWriterPtr(_docWriter)->waitQueuePauseBytes);
}

void WaitQueue::abort() {
    SyncLock syncLock(this);
    int32_t count = 0;
    for (Collection<DocWriterPtr>::iterator doc = waiting.begin(); doc != waiting.end(); ++doc) {
        if (*doc) {
            (*doc)->abort();
            doc->reset();
            ++count;
        }
    }
    waitingBytes = 0;
    BOOST_ASSERT(count == numWaiting);
    numWaiting = 0;
}

void WaitQueue::writeDocument(const DocWriterPtr& doc) {
    DocumentsWriterPtr docWriter(_docWriter);
    BOOST_ASSERT(doc == DocumentsWriterPtr(docWriter)->skipDocWriter || nextWriteDocID == doc->docID);
    bool success = false;
    LuceneException finally;
    try {
        doc->finish();
        ++nextWriteDocID;
        ++docWriter->numDocsInStore;
        ++nextWriteLoc;
        BOOST_ASSERT(nextWriteLoc <= waiting.size());
        if (nextWriteLoc == waiting.size()) {
            nextWriteLoc = 0;
        }
        success = true;
    } catch (LuceneException& e) {
        finally = e;
    }
    if (!success) {
        docWriter->setAborting();
    }
    finally.throwException();
}

bool WaitQueue::add(const DocWriterPtr& doc) {
    DocWriterPtr _doc(doc);
    SyncLock syncLock(this);
    BOOST_ASSERT(_doc->docID >= nextWriteDocID);
    if (_doc->docID == nextWriteDocID) {
        writeDocument(_doc);
        while (true) {
            _doc = waiting[nextWriteLoc];
            if (_doc) {
                --numWaiting;
                waiting[nextWriteLoc].reset();
                waitingBytes -= _doc->sizeInBytes();
                writeDocument(_doc);
            } else {
                break;
            }
        }
    } else {
        // I finished before documents that were added before me.  This can easily happen when I am a small doc
        // and the docs before me were large, or just due to luck in the thread scheduling.  Just add myself to
        // the queue and when that large doc finishes, it will flush me
        int32_t gap = _doc->docID - nextWriteDocID;
        if (gap >= waiting.size()) {
            // Grow queue
            Collection<DocWriterPtr> newArray(Collection<DocWriterPtr>::newInstance(MiscUtils::getNextSize(gap)));
            BOOST_ASSERT(nextWriteLoc >= 0);
            MiscUtils::arrayCopy(waiting.begin(), nextWriteLoc, newArray.begin(), 0, waiting.size() - nextWriteLoc);
            MiscUtils::arrayCopy(waiting.begin(), 0, newArray.begin(), waiting.size() - nextWriteLoc, nextWriteLoc);
            nextWriteLoc = 0;
            waiting = newArray;
            gap = _doc->docID - nextWriteDocID;
        }

        int32_t loc = nextWriteLoc + gap;
        if (loc >= waiting.size()) {
            loc -= waiting.size();
        }

        // We should only wrap one time
        BOOST_ASSERT(loc < waiting.size());

        // Nobody should be in my spot!
        BOOST_ASSERT(!waiting[loc]);
        waiting[loc] = _doc;
        ++numWaiting;
        waitingBytes += _doc->sizeInBytes();
    }

    return doPause();
}

ByteBlockAllocator::ByteBlockAllocator(const DocumentsWriterPtr& docWriter, int32_t blockSize) {
    this->blockSize = blockSize;
    this->freeByteBlocks = Collection<ByteArray>::newInstance();
    this->_docWriter = docWriter;
}

ByteBlockAllocator::~ByteBlockAllocator() {
}

ByteArray ByteBlockAllocator::getByteBlock(bool trackAllocations) {
    DocumentsWriterPtr docWriter(_docWriter);
    SyncLock syncLock(docWriter);
    int32_t size = freeByteBlocks.size();
    ByteArray b;
    if (size == 0) {
        // Always record a block allocated, even if trackAllocations is false.  This is necessary because this block will
        // be shared between things that don't track allocations (term vectors) and things that do (freq/prox postings).
        docWriter->numBytesAlloc += blockSize;
        b = ByteArray::newInstance(blockSize);
        MiscUtils::arrayFill(b.get(), 0, b.size(), 0);
    } else {
        b = freeByteBlocks.removeLast();
    }
    if (trackAllocations) {
        docWriter->numBytesUsed += blockSize;
    }
    BOOST_ASSERT(docWriter->numBytesUsed <= docWriter->numBytesAlloc);
    return b;
}

void ByteBlockAllocator::recycleByteBlocks(Collection<ByteArray> blocks, int32_t start, int32_t end) {
    DocumentsWriterPtr docWriter(_docWriter);
    SyncLock syncLock(docWriter);
    for (int32_t i = start; i < end; ++i) {
        freeByteBlocks.add(blocks[i]);
        blocks[i].reset();
    }
}

void ByteBlockAllocator::recycleByteBlocks(Collection<ByteArray> blocks) {
    DocumentsWriterPtr docWriter(_docWriter);
    SyncLock syncLock(docWriter);
    int32_t size = blocks.size();
    for (int32_t i = 0; i < size; ++i) {
        freeByteBlocks.add(blocks[i]);
    }
}

}
