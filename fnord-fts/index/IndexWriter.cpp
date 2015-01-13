/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "IndexWriter.h"
#include "_IndexWriter.h"
#include "Directory.h"
#include "Analyzer.h"
#include "KeepOnlyLastCommitDeletionPolicy.h"
#include "DocumentsWriter.h"
#include "IndexFileDeleter.h"
#include "IndexFileNames.h"
#include "Lock.h"
#include "SegmentInfo.h"
#include "SegmentReader.h"
#include "ReadOnlyDirectoryReader.h"
#include "BufferedIndexInput.h"
#include "LogByteSizeMergePolicy.h"
#include "LogDocMergePolicy.h"
#include "Similarity.h"
#include "ConcurrentMergeScheduler.h"
#include "CompoundFileWriter.h"
#include "SegmentMerger.h"
#include "DateTools.h"
#include "Constants.h"
#include "InfoStream.h"
#include "TestPoint.h"
#include "StringUtils.h"

namespace Lucene {

/// The normal read buffer size defaults to 1024, but increasing this during merging seems to
/// yield performance gains.  However we don't want to increase it too much because there are
/// quite a few BufferedIndexInputs created during merging.
const int32_t IndexWriter::MERGE_READ_BUFFER_SIZE = 4096;

int32_t IndexWriter::MESSAGE_ID = 0;
InfoStreamPtr IndexWriter::defaultInfoStream;

/// Default value for the write lock timeout (1,000).
int64_t IndexWriter::WRITE_LOCK_TIMEOUT = 1000;

const String IndexWriter::WRITE_LOCK_NAME = L"write.lock";

/// Value to denote a flush trigger is disabled.
const int32_t IndexWriter::DISABLE_AUTO_FLUSH = -1;

/// Disabled by default (because IndexWriter flushes by RAM usage by default).
const int32_t IndexWriter::DEFAULT_MAX_BUFFERED_DOCS = IndexWriter::DISABLE_AUTO_FLUSH;

/// Default value is 16 MB (which means flush when buffered docs consume 16 MB RAM).
const double IndexWriter::DEFAULT_RAM_BUFFER_SIZE_MB = 16.0;

/// Disabled by default (because IndexWriter flushes by RAM usage by default).
const int32_t IndexWriter::DEFAULT_MAX_BUFFERED_DELETE_TERMS = IndexWriter::DISABLE_AUTO_FLUSH;

/// Default value is 10000.
const int32_t IndexWriter::DEFAULT_MAX_FIELD_LENGTH = 10000;

/// Default value is 128.
const int32_t IndexWriter::DEFAULT_TERM_INDEX_INTERVAL = 128;

/// Sets the maximum field length to INT_MAX
const int32_t IndexWriter::MaxFieldLengthUNLIMITED = INT_MAX;

/// Sets the maximum field length to {@link #DEFAULT_MAX_FIELD_LENGTH}
const int32_t IndexWriter::MaxFieldLengthLIMITED = IndexWriter::DEFAULT_MAX_FIELD_LENGTH;

IndexWriter::IndexWriter(const DirectoryPtr& d, const AnalyzerPtr& a, bool create, int32_t mfl) {
    this->directory = d;
    this->analyzer = a;
    this->create = create;
    this->maxFieldLength = mfl;
}

IndexWriter::IndexWriter(const DirectoryPtr& d, const AnalyzerPtr& a, int32_t mfl) {
    this->directory = d;
    this->analyzer = a;
    this->create = !IndexReader::indexExists(d);
    this->maxFieldLength = mfl;
}

IndexWriter::IndexWriter(const DirectoryPtr& d, const AnalyzerPtr& a, const IndexDeletionPolicyPtr& deletionPolicy, int32_t mfl) {
    this->directory = d;
    this->analyzer = a;
    this->deletionPolicy = deletionPolicy;
    this->create = !IndexReader::indexExists(d);
    this->maxFieldLength = mfl;
}

IndexWriter::IndexWriter(const DirectoryPtr& d, const AnalyzerPtr& a, bool create, const IndexDeletionPolicyPtr& deletionPolicy, int32_t mfl) {
    this->directory = d;
    this->analyzer = a;
    this->create = create;
    this->deletionPolicy = deletionPolicy;
    this->maxFieldLength = mfl;
}

IndexWriter::IndexWriter(const DirectoryPtr& d, const AnalyzerPtr& a, bool create, const IndexDeletionPolicyPtr& deletionPolicy, int32_t mfl, const IndexingChainPtr& indexingChain, const IndexCommitPtr& commit) {
    this->directory = d;
    this->analyzer = a;
    this->create = create;
    this->deletionPolicy = deletionPolicy;
    this->maxFieldLength = mfl;
    this->indexingChain = indexingChain;
    this->indexCommit = commit;
}

IndexWriter::IndexWriter(const DirectoryPtr& d, const AnalyzerPtr& a, const IndexDeletionPolicyPtr& deletionPolicy, int32_t mfl, const IndexCommitPtr& commit) {
    this->directory = d;
    this->analyzer = a;
    this->create = false;
    this->deletionPolicy = deletionPolicy;
    this->maxFieldLength = mfl;
    this->indexCommit = commit;
}

IndexWriter::~IndexWriter() {
}

void IndexWriter::initialize() {
    messageID = -1;
    messageIDLock = newInstance<Synchronize>();
    setMessageID(defaultInfoStream);
    this->writeLockTimeout = WRITE_LOCK_TIMEOUT;
    this->segmentInfos = newLucene<SegmentInfos>();
    pendingMerges = Collection<OneMergePtr>::newInstance();
    mergeExceptions = Collection<OneMergePtr>::newInstance();
    segmentsToOptimize = SetSegmentInfo::newInstance();
    optimizeMaxNumSegments = 0;
    mergingSegments = SetSegmentInfo::newInstance();
    runningMerges = SetOneMerge::newInstance();
    synced = HashSet<String>::newInstance();
    syncing = HashSet<String>::newInstance();
    changeCount = 0;
    lastCommitChangeCount = 0;
    poolReaders = false;
    readCount = 0;
    writeThread = 0;
    upgradeCount = 0;
    readerTermsIndexDivisor = IndexReader::DEFAULT_TERMS_INDEX_DIVISOR;
    readerPool = newLucene<ReaderPool>(shared_from_this());
    closed = false;
    closing = false;
    hitOOM = false;
    stopMerges = false;
    mergeGen = 0;
    flushCount = 0;
    flushDeletesCount = 0;
    localFlushedDocCount = 0;
    pendingCommitChangeCount = 0;
    mergePolicy = newLucene<LogByteSizeMergePolicy>(shared_from_this());
    mergeScheduler = newLucene<ConcurrentMergeScheduler>();
    similarity = Similarity::getDefault();
    termIndexInterval = DEFAULT_TERM_INDEX_INTERVAL;
    commitLock  = newInstance<Synchronize>();

    if (!indexingChain) {
        indexingChain = DocumentsWriter::getDefaultIndexingChain();
    }

    if (create) {
        directory->clearLock(WRITE_LOCK_NAME);    // clear the write lock in case it's leftover
    }

    LockPtr writeLock(directory->makeLock(WRITE_LOCK_NAME));

    if (!writeLock->obtain((int32_t)writeLockTimeout)) { // obtain write lock
        boost::throw_exception(LockObtainFailedException(L"Index locked for write: " + writeLock->toString()));
    }
    this->writeLock = writeLock;

    bool success = false;
    LuceneException finally;

    try {
        if (create) {
            // Try to read first.  This is to allow create against an index that's currently open for
            // searching.  In this case we write the next segments_N file with no segments
            bool doCommit;
            try {
                segmentInfos->read(directory);
                segmentInfos->clear();
                doCommit = false;
            } catch (LuceneException&) {
                // Likely this means it's a fresh directory
                doCommit = true;
            }

            if (doCommit) {
                // Only commit if there is no segments file in this dir already.
                segmentInfos->commit(directory);
                HashSet<String> files(segmentInfos->files(directory, true));
                synced.addAll(files.begin(), files.end());
            } else {
                // Record that we have a change (zero out all segments) pending
                ++changeCount;
            }
        } else {
            segmentInfos->read(directory);

            if (indexCommit) {
                // Swap out all segments, but, keep metadata in SegmentInfos, like version & generation, to
                // preserve write-once.  This is important if readers are open against the future commit points.
                if (indexCommit->getDirectory() != directory) {
                    boost::throw_exception(IllegalArgumentException(L"IndexCommit's directory doesn't match my directory"));
                }
                SegmentInfosPtr oldInfos(newLucene<SegmentInfos>());
                oldInfos->read(directory, indexCommit->getSegmentsFileName());
                segmentInfos->replace(oldInfos);
                ++changeCount;
                if (infoStream) {
                    message(L"init: loaded commit \"" + indexCommit->getSegmentsFileName() + L"\"");
                }
            }

            // We assume that this segments_N was previously properly sync'd
            HashSet<String> files(segmentInfos->files(directory, true));
            synced.addAll(files.begin(), files.end());
        }

        setRollbackSegmentInfos(segmentInfos);

        docWriter = newLucene<DocumentsWriter>(directory, shared_from_this(), indexingChain);
        docWriter->setInfoStream(infoStream);
        docWriter->setMaxFieldLength(maxFieldLength);

        // Default deleter (for backwards compatibility) is KeepOnlyLastCommitDeleter
        deleter = newLucene<IndexFileDeleter>(directory, deletionPolicy ? deletionPolicy : newLucene<KeepOnlyLastCommitDeletionPolicy>(), segmentInfos, infoStream, docWriter, synced);

        if (deleter->startingCommitDeleted) {
            // Deletion policy deleted the "head" commit point.  We have to mark ourself as changed so that if we
            // are closed without any further changes we write a new segments_N file.
            ++changeCount;
        }

        pushMaxBufferedDocs();

        if (infoStream) {
            message(L"init: create=" + StringUtils::toString(create));
        }
        messageState();

        success = true;
    } catch (LuceneException& e) {
        finally = e;
    }

    if (!success) {
        if (infoStream) {
            message(L"init: hit exception on init; releasing write lock");
        }
        try {
            this->writeLock->release();
        } catch (...) {
            // don't mask the original exception
        }
        this->writeLock.reset();
    }

    finally.throwException();
}

int32_t IndexWriter::MAX_TERM_LENGTH() {
    static int32_t _MAX_TERM_LENGTH = 0;
    if (_MAX_TERM_LENGTH == 0) {
        _MAX_TERM_LENGTH = DocumentsWriter::MAX_TERM_LENGTH;
    }
    return _MAX_TERM_LENGTH;
}

IndexReaderPtr IndexWriter::getReader() {
    return getReader(readerTermsIndexDivisor);
}

IndexReaderPtr IndexWriter::getReader(int32_t termInfosIndexDivisor) {
    ensureOpen();

    if (infoStream) {
        message(L"flush at getReader");
    }

    // Do this up front before flushing so that the readers obtained during this flush are pooled, the first time
    // this method is called
    poolReaders = true;

    // Prevent segmentInfos from changing while opening the reader; in theory we could do similar retry logic,
    // just like we do when loading segments_N
    IndexReaderPtr r;
    {
        SyncLock syncLock(this);
        flush(false, true, true);
        r = newLucene<ReadOnlyDirectoryReader>(shared_from_this(), segmentInfos, termInfosIndexDivisor);
    }
    maybeMerge();
    return r;
}

int32_t IndexWriter::numDeletedDocs(const SegmentInfoPtr& info) {
    SegmentReaderPtr reader(readerPool->getIfExists(info));
    int32_t deletedDocs = 0;
    LuceneException finally;
    try {
        deletedDocs = reader ? reader->numDeletedDocs() : info->getDelCount();
    } catch (LuceneException& e) {
        finally = e;
    }
    if (reader) {
        readerPool->release(reader);
    }
    finally.throwException();
    return deletedDocs;
}

void IndexWriter::acquireWrite() {
    SyncLock syncLock(this);
    BOOST_ASSERT(writeThread != LuceneThread::currentId());
    while (writeThread != 0 || readCount > 0) {
        doWait();
    }

    // we could have been closed while we were waiting
    ensureOpen();

    writeThread = LuceneThread::currentId();
}

void IndexWriter::releaseWrite() {
    SyncLock syncLock(this);
    BOOST_ASSERT(writeThread == LuceneThread::currentId());
    writeThread = 0;
    notifyAll();
}

void IndexWriter::acquireRead() {
    SyncLock syncLock(this);
    int64_t current = LuceneThread::currentId();
    while (writeThread != 0 && writeThread != current) {
        doWait();
    }
    ++readCount;
}

void IndexWriter::upgradeReadToWrite() {
    SyncLock syncLock(this);
    BOOST_ASSERT(readCount > 0);
    ++upgradeCount;
    while (readCount > upgradeCount || writeThread != 0) {
        doWait();
    }
    writeThread = LuceneThread::currentId();
    --readCount;
    --upgradeCount;
}

void IndexWriter::releaseRead() {
    SyncLock syncLock(this);
    --readCount;
    BOOST_ASSERT(readCount >= 0);
    notifyAll();
}

bool IndexWriter::isOpen(bool includePendingClose) {
    SyncLock syncLock(this);
    return !(closed || (includePendingClose && closing));
}

void IndexWriter::ensureOpen(bool includePendingClose) {
    SyncLock syncLock(this);
    if (!isOpen(includePendingClose)) {
        boost::throw_exception(AlreadyClosedException(L"This IndexWriter is closed"));
    }
}

void IndexWriter::ensureOpen() {
    ensureOpen(true);
}

void IndexWriter::message(const String& message) {
    if (infoStream) {
        *infoStream << L"IW " << StringUtils::toString(messageID);
        *infoStream << L" [" << DateTools::timeToString(MiscUtils::currentTimeMillis(), DateTools::RESOLUTION_SECOND);
        *infoStream << L"; " << StringUtils::toString(LuceneThread::currentId()) << L"]: " << message << L"\n";
    }
}

void IndexWriter::setMessageID(const InfoStreamPtr& infoStream) {
    SyncLock syncLock(this);
    if (infoStream && messageID == -1) {
        SyncLock messageLock(messageIDLock);
        messageID = MESSAGE_ID++;
    }
    this->infoStream = infoStream;
}

LogMergePolicyPtr IndexWriter::getLogMergePolicy() {
    LogMergePolicyPtr logMergePolicy(boost::dynamic_pointer_cast<LogMergePolicy>(mergePolicy));
    if (logMergePolicy) {
        return logMergePolicy;
    }
    boost::throw_exception(IllegalArgumentException(L"This method can only be called when the merge policy is the default LogMergePolicy"));
    return LogMergePolicyPtr();
}

bool IndexWriter::getUseCompoundFile() {
    return getLogMergePolicy()->getUseCompoundFile();
}

void IndexWriter::setUseCompoundFile(bool value) {
    getLogMergePolicy()->setUseCompoundFile(value);
    getLogMergePolicy()->setUseCompoundDocStore(value);
}

void IndexWriter::setSimilarity(const SimilarityPtr& similarity) {
    ensureOpen();
    this->similarity = similarity;
    docWriter->setSimilarity(similarity);
}

SimilarityPtr IndexWriter::getSimilarity() {
    ensureOpen();
    return this->similarity;
}

void IndexWriter::setTermIndexInterval(int32_t interval) {
    ensureOpen();
    this->termIndexInterval = interval;
}

int32_t IndexWriter::getTermIndexInterval() {
    // We pass false because this method is called by SegmentMerger while we are in the process of closing
    ensureOpen(false);
    return termIndexInterval;
}

void IndexWriter::setRollbackSegmentInfos(const SegmentInfosPtr& infos) {
    SyncLock syncLock(this);
    rollbackSegmentInfos = boost::dynamic_pointer_cast<SegmentInfos>(infos->clone());
    BOOST_ASSERT(!rollbackSegmentInfos->hasExternalSegments(directory));
    rollbackSegments = MapSegmentInfoInt::newInstance();
    int32_t size = rollbackSegmentInfos->size();
    for (int32_t i = 0; i < size; ++i) {
        rollbackSegments.put(rollbackSegmentInfos->info(i), i);
    }
}

void IndexWriter::setMergePolicy(const MergePolicyPtr& mp) {
    ensureOpen();
    if (!mp) {
        boost::throw_exception(NullPointerException(L"MergePolicy must be non-null"));
    }

    if (mergePolicy != mp) {
        mergePolicy->close();
    }
    mergePolicy = mp;
    pushMaxBufferedDocs();
    if (infoStream) {
        message(L"setMergePolicy");
    }
}

MergePolicyPtr IndexWriter::getMergePolicy() {
    ensureOpen();
    return mergePolicy;
}

void IndexWriter::setMergeScheduler(const MergeSchedulerPtr& mergeScheduler) {
    SyncLock syncLock(this);
    ensureOpen();
    if (!mergeScheduler) {
        boost::throw_exception(NullPointerException(L"MergeScheduler must be non-null"));
    }
    if (this->mergeScheduler != mergeScheduler) {
        finishMerges(true);
        this->mergeScheduler->close();
    }
    this->mergeScheduler = mergeScheduler;
    if (infoStream) {
        message(L"setMergeScheduler");
    }
}

MergeSchedulerPtr IndexWriter::getMergeScheduler() {
    ensureOpen();
    return mergeScheduler;
}

void IndexWriter::setMaxMergeDocs(int32_t maxMergeDocs) {
    getLogMergePolicy()->setMaxMergeDocs(maxMergeDocs);
}

int32_t IndexWriter::getMaxMergeDocs() {
    return getLogMergePolicy()->getMaxMergeDocs();
}

void IndexWriter::setMaxFieldLength(int32_t maxFieldLength) {
    ensureOpen();
    this->maxFieldLength = maxFieldLength;
    docWriter->setMaxFieldLength(maxFieldLength);
    if (infoStream) {
        message(L"setMaxFieldLength " + StringUtils::toString(maxFieldLength));
    }
}

int32_t IndexWriter::getMaxFieldLength() {
    ensureOpen();
    return maxFieldLength;
}

void IndexWriter::setReaderTermsIndexDivisor(int32_t divisor) {
    ensureOpen();
    if (divisor <= 0) {
        boost::throw_exception(IllegalArgumentException(L"divisor must be >= 1 (got " + StringUtils::toString(divisor) + L")"));
    }
    readerTermsIndexDivisor = divisor;
    if (infoStream) {
        message(L"setReaderTermsIndexDivisor " + StringUtils::toString(readerTermsIndexDivisor));
    }
}

int32_t IndexWriter::getReaderTermsIndexDivisor() {
    ensureOpen();
    return readerTermsIndexDivisor;
}

void IndexWriter::setMaxBufferedDocs(int32_t maxBufferedDocs) {
    ensureOpen();
    if (maxBufferedDocs != DISABLE_AUTO_FLUSH && maxBufferedDocs < 2) {
        boost::throw_exception(IllegalArgumentException(L"maxBufferedDocs must at least be 2 when enabled"));
    }
    if (maxBufferedDocs == DISABLE_AUTO_FLUSH && getRAMBufferSizeMB() == DISABLE_AUTO_FLUSH) {
        boost::throw_exception(IllegalArgumentException(L"at least one of ramBufferSize and maxBufferedDocs must be enabled"));
    }
    docWriter->setMaxBufferedDocs(maxBufferedDocs);
    pushMaxBufferedDocs();
    if (infoStream) {
        message(L"setMaxBufferedDocs " + StringUtils::toString(maxBufferedDocs));
    }
}

void IndexWriter::pushMaxBufferedDocs() {
    if (docWriter->getMaxBufferedDocs() != DISABLE_AUTO_FLUSH) {
        LogDocMergePolicyPtr lmp(boost::dynamic_pointer_cast<LogDocMergePolicy>(mergePolicy));
        if (lmp) {
            int32_t maxBufferedDocs = docWriter->getMaxBufferedDocs();
            if (lmp->getMinMergeDocs() != maxBufferedDocs) {
                if (infoStream) {
                    message(L"now push maxBufferedDocs " + StringUtils::toString(maxBufferedDocs) + L" to LogDocMergePolicy");
                }
                lmp->setMinMergeDocs(maxBufferedDocs);
            }
        }
    }
}

int32_t IndexWriter::getMaxBufferedDocs() {
    ensureOpen();
    return docWriter->getMaxBufferedDocs();
}

void IndexWriter::setRAMBufferSizeMB(double mb) {
    if (mb > 2048.0) {
        boost::throw_exception(IllegalArgumentException(L"ramBufferSize " + StringUtils::toString(mb) + L" is too large; should be comfortably less than 2048"));
    }
    if (mb != DISABLE_AUTO_FLUSH && mb <= 0.0) {
        boost::throw_exception(IllegalArgumentException(L"ramBufferSize should be > 0.0 MB when enabled"));
    }
    if (mb == DISABLE_AUTO_FLUSH && getMaxBufferedDocs() == DISABLE_AUTO_FLUSH) {
        boost::throw_exception(IllegalArgumentException(L"at least one of ramBufferSize and maxBufferedDocs must be enabled"));
    }
    docWriter->setRAMBufferSizeMB(mb);
    if (infoStream) {
        message(L"setRAMBufferSizeMB " + StringUtils::toString(mb));
    }
}

double IndexWriter::getRAMBufferSizeMB() {
    return docWriter->getRAMBufferSizeMB();
}

void IndexWriter::setMaxBufferedDeleteTerms(int32_t maxBufferedDeleteTerms) {
    ensureOpen();
    if (maxBufferedDeleteTerms != DISABLE_AUTO_FLUSH && maxBufferedDeleteTerms < 1) {
        boost::throw_exception(IllegalArgumentException(L"maxBufferedDeleteTerms must at least be 1 when enabled"));
    }
    docWriter->setMaxBufferedDeleteTerms(maxBufferedDeleteTerms);
    if (infoStream) {
        message(L"setMaxBufferedDeleteTerms " + StringUtils::toString(maxBufferedDeleteTerms));
    }
}

int32_t IndexWriter::getMaxBufferedDeleteTerms() {
    ensureOpen();
    return docWriter->getMaxBufferedDeleteTerms();
}

void IndexWriter::setMergeFactor(int32_t mergeFactor) {
    getLogMergePolicy()->setMergeFactor(mergeFactor);
}

int32_t IndexWriter::getMergeFactor() {
    return getLogMergePolicy()->getMergeFactor();
}

void IndexWriter::setDefaultInfoStream(const InfoStreamPtr& infoStream) {
    IndexWriter::defaultInfoStream = infoStream;
}

InfoStreamPtr IndexWriter::getDefaultInfoStream() {
    return IndexWriter::defaultInfoStream;
}

void IndexWriter::setInfoStream(const InfoStreamPtr& infoStream) {
    ensureOpen();
    setMessageID(infoStream);
    docWriter->setInfoStream(infoStream);
    deleter->setInfoStream(infoStream);
    messageState();
}

void IndexWriter::messageState() {
    if (infoStream) {
        message(L"ramBufferSizeMB=" + StringUtils::toString(docWriter->getRAMBufferSizeMB()) +
                L" maxBufferedDocs=" + StringUtils::toString(docWriter->getMaxBufferedDocs()) +
                L" maxBuffereDeleteTerms=" + StringUtils::toString(docWriter->getMaxBufferedDeleteTerms()) +
                L" maxFieldLength=" + StringUtils::toString(maxFieldLength) +
                L" index=" + segString());
    }
}

InfoStreamPtr IndexWriter::getInfoStream() {
    ensureOpen();
    return infoStream;
}

bool IndexWriter::verbose() {
    return infoStream.get() != NULL;
}

void IndexWriter::setWriteLockTimeout(int64_t writeLockTimeout) {
    ensureOpen();
    this->writeLockTimeout = writeLockTimeout;
}

int64_t IndexWriter::getWriteLockTimeout() {
    ensureOpen();
    return writeLockTimeout;
}

void IndexWriter::setDefaultWriteLockTimeout(int64_t writeLockTimeout) {
    IndexWriter::WRITE_LOCK_TIMEOUT = writeLockTimeout;
}

int64_t IndexWriter::getDefaultWriteLockTimeout() {
    return IndexWriter::WRITE_LOCK_TIMEOUT;
}

void IndexWriter::close() {
    close(true);
}

void IndexWriter::close(bool waitForMerges) {
    // Ensure that only one thread actually gets to do the closing
    if (shouldClose()) {
        // If any methods have hit std::bad_alloc, then abort on close, in case the internal state of IndexWriter
        // or DocumentsWriter is corrupt
        if (hitOOM) {
            rollbackInternal();
        } else {
            closeInternal(waitForMerges);
        }
    }
}

bool IndexWriter::shouldClose() {
    SyncLock syncLock(this);
    while (true) {
        if (!closed) {
            if (!closing) {
                closing = true;
                return true;
            } else {
                // Another thread is presently trying to close; wait until it finishes one way (closes
                // successfully) or another (fails to close)
                doWait();
            }
        } else {
            return false;
        }
    }
}

void IndexWriter::closeInternal(bool waitForMerges) {
    docWriter->pauseAllThreads();

    LuceneException finally;
    try {
        if (infoStream) {
            message(L"now flush at close");
        }

        docWriter->close();

        // Only allow a new merge to be triggered if we are going to wait for merges
        if (!hitOOM) {
            flush(waitForMerges, true, true);
        }

        // Give merge scheduler last chance to run, in case any pending merges are waiting
        if (waitForMerges) {
            mergeScheduler->merge(shared_from_this());
        }

        mergePolicy->close();

        finishMerges(waitForMerges);
        stopMerges = true;

        mergeScheduler->close();

        if (infoStream) {
            message(L"now call final commit()");
        }

        if (!hitOOM) {
            commit(0);
        }

        if (infoStream) {
            message(L"at close: " + segString());
        }

        {
            SyncLock syncLock(this);
            readerPool->close();
            docWriter.reset();
            deleter->close();
        }

        if (writeLock) {
            writeLock->release(); // release write lock
            writeLock.reset();
        }

        {
            SyncLock syncLock(this);
            closed = true;
        }
    } catch (std::bad_alloc& oom) {
        finally = handleOOM(oom, L"closeInternal");
    } catch (LuceneException& e) {
        finally = e;
    }
    {
        SyncLock syncLock(this);
        closing = false;
        notifyAll();
        if (!closed) {
            if (docWriter) {
                docWriter->resumeAllThreads();
            }
            if (infoStream) {
                message(L"hit exception while closing");
            }
        }
    }
    finally.throwException();
}

bool IndexWriter::flushDocStores() {
    SyncLock syncLock(this);

    if (infoStream) {
        message(L"flushDocStores segment=" + docWriter->getDocStoreSegment());
    }

    bool useCompoundDocStore = false;

    if (infoStream) {
        message(L"closeDocStores segment=" + docWriter->getDocStoreSegment());
    }

    String docStoreSegment;

    bool success = false;
    LuceneException finally;
    try {
        docStoreSegment = docWriter->closeDocStore();
        success = true;
    } catch (LuceneException& e) {
        finally = e;
    }
    if (!success && infoStream) {
        message(L"hit exception closing doc store segment");
    }
    finally.throwException();

    if (infoStream) {
        message(L"flushDocStores files=" + StringUtils::toString(docWriter->closedFiles()));
    }

    useCompoundDocStore = mergePolicy->useCompoundDocStore(segmentInfos);
    HashSet<String> closedFiles(docWriter->closedFiles());

    if (useCompoundDocStore && !docStoreSegment.empty() && !closedFiles.empty()) {
        // Now build compound doc store file
        if (infoStream) {
            message(L"create compound file " + docStoreSegment + L"." + IndexFileNames::COMPOUND_FILE_STORE_EXTENSION());
        }

        success = false;

        int32_t numSegments = segmentInfos->size();
        String compoundFileName(docStoreSegment + L"." + IndexFileNames::COMPOUND_FILE_STORE_EXTENSION());

        try {
            CompoundFileWriterPtr cfsWriter(newLucene<CompoundFileWriter>(directory, compoundFileName));
            for (HashSet<String>::iterator file = closedFiles.begin(); file != closedFiles.end(); ++file) {
                cfsWriter->addFile(*file);
            }

            // Perform the merge
            cfsWriter->close();
            success = true;
        } catch (LuceneException& e) {
            finally = e;
        }

        if (!success) {
            if (infoStream) {
                message(L"hit exception building compound file doc store for segment " + docStoreSegment);
            }
            deleter->deleteFile(compoundFileName);
            docWriter->abort();
        }
        finally.throwException();

        for (int32_t i = 0; i < numSegments; ++i) {
            SegmentInfoPtr si(segmentInfos->info(i));
            if (si->getDocStoreOffset() != -1 && si->getDocStoreSegment() == docStoreSegment) {
                si->setDocStoreIsCompoundFile(true);
            }
        }

        checkpoint();

        // In case the files we just merged into a CFS were not previously checkpointed
        deleter->deleteNewFiles(docWriter->closedFiles());
    }

    return useCompoundDocStore;
}

DirectoryPtr IndexWriter::getDirectory() {
    ensureOpen(false); // Pass false because the flush during closing calls getDirectory
    return directory;
}

AnalyzerPtr IndexWriter::getAnalyzer() {
    ensureOpen();
    return analyzer;
}

int32_t IndexWriter::maxDoc() {
    SyncLock syncLock(this);
    int32_t count = docWriter ? docWriter->getNumDocsInRAM() : 0;
    for (int32_t i = 0; i < segmentInfos->size(); ++i) {
        count += segmentInfos->info(i)->docCount;
    }
    return count;
}

int32_t IndexWriter::numDocs() {
    SyncLock syncLock(this);
    int32_t count = docWriter ? docWriter->getNumDocsInRAM() : 0;
    for (int32_t i = 0; i < segmentInfos->size(); ++i) {
        SegmentInfoPtr info(segmentInfos->info(i));
        count += info->docCount - info->getDelCount();
    }
    return count;
}

bool IndexWriter::hasDeletions() {
    SyncLock syncLock(this);
    ensureOpen();
    if (docWriter->hasDeletes()) {
        return true;
    }
    for (int32_t i = 0; i < segmentInfos->size(); ++i) {
        if (segmentInfos->info(i)->hasDeletions()) {
            return true;
        }
    }
    return false;
}

void IndexWriter::addDocument(const DocumentPtr& doc) {
    addDocument(doc, analyzer);
}

void IndexWriter::addDocument(const DocumentPtr& doc, const AnalyzerPtr& analyzer) {
    ensureOpen();
    bool doFlush = false;
    bool success = false;
    try {
        LuceneException finally;
        try {
            doFlush = docWriter->addDocument(doc, analyzer);
            success = true;
        } catch (LuceneException& e) {
            finally = e;
        }
        if (!success) {
            if (infoStream) {
                message(L"hit exception adding document");
            }
            {
                SyncLock syncLock(this);
                // If docWriter has some aborted files that were never incref'd, then we clean them up here
                if (docWriter) {
                    HashSet<String> files(docWriter->abortedFiles());
                    if (files) {
                        deleter->deleteNewFiles(files);
                    }
                }
            }
        }
        finally.throwException();
        if (doFlush) {
            flush(true, false, false);
        }
    } catch (std::bad_alloc& oom) {
        boost::throw_exception(handleOOM(oom, L"addDocument"));
    }
}

void IndexWriter::deleteDocuments(const TermPtr& term) {
    ensureOpen();
    try {
        bool doFlush = docWriter->bufferDeleteTerm(term);
        if (doFlush) {
            flush(true, false, false);
        }
    } catch (std::bad_alloc& oom) {
        boost::throw_exception(handleOOM(oom, L"deleteDocuments(Term)"));
    }
}

void IndexWriter::deleteDocuments(Collection<TermPtr> terms) {
    ensureOpen();
    try {
        bool doFlush = docWriter->bufferDeleteTerms(terms);
        if (doFlush) {
            flush(true, false, false);
        }
    } catch (std::bad_alloc& oom) {
        boost::throw_exception(handleOOM(oom, L"deleteDocuments(VectorTerm)"));
    }
}

void IndexWriter::deleteDocuments(const QueryPtr& query) {
    ensureOpen();
    bool doFlush = docWriter->bufferDeleteQuery(query);
    if (doFlush) {
        flush(true, false, false);
    }
}

void IndexWriter::deleteDocuments(Collection<QueryPtr> queries) {
    ensureOpen();
    bool doFlush = docWriter->bufferDeleteQueries(queries);
    if (doFlush) {
        flush(true, false, false);
    }
}

void IndexWriter::updateDocument(const TermPtr& term, const DocumentPtr& doc) {
    ensureOpen();
    updateDocument(term, doc, getAnalyzer());
}

void IndexWriter::updateDocument(const TermPtr& term, const DocumentPtr& doc, const AnalyzerPtr& analyzer) {
    ensureOpen();
    try {
        bool doFlush = false;
        bool success = false;
        LuceneException finally;
        try {
            doFlush = docWriter->updateDocument(term, doc, analyzer);
            success = true;
        } catch (LuceneException& e) {
            finally = e;
        }
        if (!success) {
            if (infoStream) {
                message(L"hit exception updating document");
            }

            {
                SyncLock syncLock(this);
                // If docWriter has some aborted files that were never incref'd, then we clean them up here
                if (docWriter) {
                    HashSet<String> files(docWriter->abortedFiles());
                    if (files) {
                        deleter->deleteNewFiles(files);
                    }
                }
            }
        }
        finally.throwException();
        if (doFlush) {
            flush(true, false, false);
        }
    } catch (std::bad_alloc& oom) {
        boost::throw_exception(handleOOM(oom, L"updateDocument"));
    }
}

int32_t IndexWriter::getSegmentCount() {
    SyncLock syncLock(this);
    return segmentInfos->size();
}

int32_t IndexWriter::getNumBufferedDocuments() {
    SyncLock syncLock(this);
    return docWriter->getNumDocsInRAM();
}

int32_t IndexWriter::getDocCount(int32_t i) {
    SyncLock syncLock(this);
    return (i >= 0 && i < segmentInfos->size()) ? segmentInfos->info(i)->docCount : -1;
}

int32_t IndexWriter::getFlushCount() {
    SyncLock syncLock(this);
    return flushCount;
}

int32_t IndexWriter::getFlushDeletesCount() {
    SyncLock syncLock(this);
    return flushDeletesCount;
}

String IndexWriter::newSegmentName() {
    // Cannot synchronize on IndexWriter because that causes deadlock
    SyncLock segmentLock(segmentInfos);

    // Important to increment changeCount so that the segmentInfos is written on close.
    // Otherwise we could close, re-open and re-return the same segment name that was
    // previously returned which can cause problems at least with ConcurrentMergeScheduler.
    ++changeCount;
    return L"_" + StringUtils::toString(segmentInfos->counter++, StringUtils::CHARACTER_MAX_RADIX);
}

void IndexWriter::optimize() {
    optimize(true);
}

void IndexWriter::optimize(int32_t maxNumSegments) {
    optimize(maxNumSegments, true);
}

void IndexWriter::optimize(bool doWait) {
    optimize(1, doWait);
}

void IndexWriter::optimize(int32_t maxNumSegments, bool doWait) {
    ensureOpen();

    if (maxNumSegments < 1) {
        boost::throw_exception(IllegalArgumentException(L"maxNumSegments must be >= 1; got " + StringUtils::toString(maxNumSegments)));
    }

    if (infoStream) {
        message(L"optimize: index now " + segString());
    }

    flush(true, false, true);

    {
        SyncLock syncLock(this);

        resetMergeExceptions();
        segmentsToOptimize.clear();
        optimizeMaxNumSegments = maxNumSegments;
        int32_t numSegments = segmentInfos->size();
        for (int32_t i = 0; i < numSegments; ++i) {
            segmentsToOptimize.add(segmentInfos->info(i));
        }

        // Now mark all pending & running merges as optimize merge
        for (Collection<OneMergePtr>::iterator merge = pendingMerges.begin(); merge != pendingMerges.end(); ++merge) {
            (*merge)->optimize = true;
            (*merge)->maxNumSegmentsOptimize = maxNumSegments;
        }

        for (SetOneMerge::iterator merge = runningMerges.begin(); merge != runningMerges.end(); ++merge) {
            (*merge)->optimize = true;
            (*merge)->maxNumSegmentsOptimize = maxNumSegments;
        }
    }

    maybeMerge(maxNumSegments, true);

    if (doWait) {
        {
            SyncLock syncLock(this);
            while (true) {
                if (hitOOM) {
                    boost::throw_exception(IllegalStateException(L"this writer hit an OutOfMemoryError; cannot complete optimize"));
                }

                if (!mergeExceptions.empty()) {
                    // Forward any exceptions in background merge threads to the current thread
                    for (Collection<OneMergePtr>::iterator merge = mergeExceptions.begin(); merge != mergeExceptions.end(); ++merge) {
                        if ((*merge)->optimize) {
                            LuceneException err = (*merge)->getException();
                            if (!err.isNull()) {
                                boost::throw_exception(IOException(L"background merge hit exception: " + (*merge)->segString(directory)));
                            }
                        }
                    }
                }

                if (optimizeMergesPending()) {
                    IndexWriter::doWait();
                } else {
                    break;
                }
            }
        }

        // If close is called while we are still running, throw an exception so the calling thread will know the
        // optimize did not complete
        ensureOpen();
    }

    // NOTE: in the ConcurrentMergeScheduler case, when doWait is false, we can return immediately while background
    // threads accomplish the optimization
}

bool IndexWriter::optimizeMergesPending() {
    SyncLock syncLock(this);

    for (Collection<OneMergePtr>::iterator merge = pendingMerges.begin(); merge != pendingMerges.end(); ++merge) {
        if ((*merge)->optimize) {
            return true;
        }
    }

    for (SetOneMerge::iterator merge = runningMerges.begin(); merge != runningMerges.end(); ++merge) {
        if ((*merge)->optimize) {
            return true;
        }
    }

    return false;
}

void IndexWriter::expungeDeletes(bool doWait) {
    ensureOpen();

    if (infoStream) {
        message(L"expungeDeletes: index now " + segString());
    }

    MergeSpecificationPtr spec;

    {
        SyncLock syncLock(this);
        spec = mergePolicy->findMergesToExpungeDeletes(segmentInfos);
        for (Collection<OneMergePtr>::iterator merge = spec->merges.begin(); merge != spec->merges.end(); ++merge) {
            registerMerge(*merge);
        }
    }

    mergeScheduler->merge(shared_from_this());

    if (doWait) {
        {
            SyncLock syncLock(this);
            bool running = true;
            while (running) {
                if (hitOOM) {
                    boost::throw_exception(IllegalStateException(L"this writer hit an OutOfMemoryError; cannot complete expungeDeletes"));
                }

                // Check each merge that MergePolicy asked us to do, to see if any of them are still running and
                // if any of them have hit an exception.
                running = false;
                for (Collection<OneMergePtr>::iterator merge = spec->merges.begin(); merge != spec->merges.end(); ++merge) {
                    if (pendingMerges.contains(*merge) || runningMerges.contains(*merge)) {
                        running = true;
                    }
                    LuceneException err = (*merge)->getException();
                    if (!err.isNull()) {
                        boost::throw_exception(IOException(L"background merge hit exception: " + (*merge)->segString(directory)));
                    }
                }

                // If any of our merges are still running, wait
                if (running) {
                    IndexWriter::doWait();
                }
            }
        }
    }

    // NOTE: in the ConcurrentMergeScheduler case, when doWait is false, we can return immediately while background
    // threads accomplish the optimization
}

void IndexWriter::expungeDeletes() {
    expungeDeletes(true);
}

void IndexWriter::maybeMerge() {
    maybeMerge(false);
}

void IndexWriter::maybeMerge(bool optimize) {
    maybeMerge(1, optimize);
}

void IndexWriter::maybeMerge(int32_t maxNumSegmentsOptimize, bool optimize) {
    updatePendingMerges(maxNumSegmentsOptimize, optimize);
    mergeScheduler->merge(shared_from_this());
}

void IndexWriter::updatePendingMerges(int32_t maxNumSegmentsOptimize, bool optimize) {
    SyncLock syncLock(this);
    BOOST_ASSERT(!optimize || maxNumSegmentsOptimize > 0);

    if (stopMerges) {
        return;
    }

    // Do not start new merges if we've hit std::bad_alloc
    if (hitOOM) {
        return;
    }

    MergeSpecificationPtr spec;

    if (optimize) {
        spec = mergePolicy->findMergesForOptimize(segmentInfos, maxNumSegmentsOptimize, segmentsToOptimize);

        if (spec) {
            for (Collection<OneMergePtr>::iterator merge = spec->merges.begin(); merge != spec->merges.end(); ++merge) {
                (*merge)->optimize = true;
                (*merge)->maxNumSegmentsOptimize = maxNumSegmentsOptimize;
            }
        }
    } else {
        spec = mergePolicy->findMerges(segmentInfos);
    }

    if (spec) {
        for (Collection<OneMergePtr>::iterator merge = spec->merges.begin(); merge != spec->merges.end(); ++merge) {
            registerMerge(*merge);
        }
    }
}

OneMergePtr IndexWriter::getNextMerge() {
    SyncLock syncLock(this);
    if (pendingMerges.empty()) {
        return OneMergePtr();
    } else {
        // Advance the merge from pending to running
        OneMergePtr merge(pendingMerges.removeFirst());
        runningMerges.add(merge);
        return merge;
    }
}

OneMergePtr IndexWriter::getNextExternalMerge() {
    SyncLock syncLock(this);
    if (pendingMerges.empty()) {
        return OneMergePtr();
    } else {
        for (Collection<OneMergePtr>::iterator merge = pendingMerges.begin(); merge != pendingMerges.end(); ++merge) {
            if ((*merge)->isExternal) {
                // Advance the merge from pending to running
                OneMergePtr running(*merge);
                runningMerges.add(*merge);
                pendingMerges.remove(merge);
                return running;
            }
        }
    }

    // All existing merges do not involve external segments
    return OneMergePtr();
}

void IndexWriter::startTransaction(bool haveReadLock) {
    SyncLock syncLock(this);
    bool success = false;
    LuceneException finally;
    try {
        if (infoStream) {
            message(L"now start transaction");
        }

        BOOST_ASSERT(docWriter->getNumBufferedDeleteTerms() == 0); // calling startTransaction with buffered delete terms not supported
        BOOST_ASSERT(docWriter->getNumDocsInRAM() == 0); // calling startTransaction with buffered documents not supported

        ensureOpen();

        // If a transaction is trying to roll back (because addIndexes hit an exception) then wait here until that's done
        while (stopMerges) {
            doWait();
        }

        success = true;
    } catch (LuceneException& e) {
        finally = e;
    }

    // Release the write lock if our caller held it, on hitting an exception
    if (!success && haveReadLock) {
        releaseRead();
    }
    finally.throwException();

    if (haveReadLock) {
        upgradeReadToWrite();
    } else {
        acquireWrite();
    }

    success = false;

    try {
        localRollbackSegmentInfos = boost::dynamic_pointer_cast<SegmentInfos>(segmentInfos->clone());

        BOOST_ASSERT(!hasExternalSegments());

        localFlushedDocCount = docWriter->getFlushedDocCount();

        // We must "protect" our files at this point from deletion in case we need to rollback
        deleter->incRef(segmentInfos, false);

        success = true;
    } catch (LuceneException& e) {
        finally = e;
    }

    if (!success) {
        finishAddIndexes();
    }
    finally.throwException();
}

void IndexWriter::rollbackTransaction() {
    SyncLock syncLock(this);

    if (infoStream) {
        message(L"now rollback transaction");
    }

    if (docWriter) {
        docWriter->setFlushedDocCount(localFlushedDocCount);
    }

    // Must finish merges before rolling back segmentInfos so merges don't hit  exceptions on trying to commit
    // themselves, don't get files deleted out  from under them, etc.
    finishMerges(false);

    // Keep the same segmentInfos instance but replace all of its SegmentInfo instances.  This is so the next
    // attempt to commit using this instance of IndexWriter will always write to a new generation ("write once").
    segmentInfos->clear();
    segmentInfos->addAll(localRollbackSegmentInfos);
    localRollbackSegmentInfos.reset();

    // This must come after we rollback segmentInfos, so that if a commit() kicks off it does not see the
    // segmentInfos with external segments.
    finishAddIndexes();

    // Ask deleter to locate unreferenced files we had created & remove them
    deleter->checkpoint(segmentInfos, false);

    // Remove the incRef we did in startTransaction
    deleter->decRef(segmentInfos);

    // Also ask deleter to remove any newly created files that were never incref'd; this "garbage" is created
    // when a merge kicks off but aborts part way through before it had a chance to incRef the files it had
    // partially created
    deleter->refresh();

    notifyAll();

    BOOST_ASSERT(!hasExternalSegments());
}

void IndexWriter::commitTransaction() {
    SyncLock syncLock(this);

    if (infoStream) {
        message(L"now commit transaction");
    }

    // Give deleter a chance to remove files now
    checkpoint();

    // Remove the incRef we did in startTransaction.
    deleter->decRef(localRollbackSegmentInfos);

    localRollbackSegmentInfos.reset();

    BOOST_ASSERT(!hasExternalSegments());

    finishAddIndexes();
}

void IndexWriter::rollback() {
    ensureOpen();

    // Ensure that only one thread actually gets to do the closing
    if (shouldClose()) {
        rollbackInternal();
    }
}

void IndexWriter::rollbackInternal() {
    bool success = false;

    if (infoStream) {
        message(L"rollback");
    }

    docWriter->pauseAllThreads();
    LuceneException finally;
    try {
        finishMerges(false);

        // Must pre-close these two, in case they increment changeCount so that we can then set it to false before
        // calling closeInternal
        mergePolicy->close();
        mergeScheduler->close();

        {
            SyncLock syncLock(this);

            if (pendingCommit) {
                pendingCommit->rollbackCommit(directory);
                deleter->decRef(pendingCommit);
                pendingCommit.reset();
                notifyAll();
            }

            // Keep the same segmentInfos instance but replace all of its SegmentInfo instances.  This is so the next
            // attempt to commit using this instance of IndexWriter will always write to a new generation ("write once").
            segmentInfos->clear();
            segmentInfos->addAll(rollbackSegmentInfos);

            BOOST_ASSERT(!hasExternalSegments());

            docWriter->abort();

            bool test = testPoint(L"rollback before checkpoint");
            BOOST_ASSERT(test);

            // Ask deleter to locate unreferenced files & remove them
            deleter->checkpoint(segmentInfos, false);
            deleter->refresh();
        }

        // Don't bother saving any changes in our segmentInfos
        readerPool->clear(SegmentInfosPtr());

        lastCommitChangeCount = changeCount;

        success = true;
    } catch (std::bad_alloc& oom) {
        finally = handleOOM(oom, L"rollbackInternal");
    } catch (LuceneException& e) {
        finally = e;
    }
    {
        SyncLock syncLock(this);

        if (!success) {
            docWriter->resumeAllThreads();
            closing = false;
            notifyAll();
            if (infoStream) {
                message(L"hit exception during rollback");
            }
        }
    }
    finally.throwException();

    closeInternal(false);
}

void IndexWriter::deleteAll() {
    SyncLock syncLock(this);
    bool success = false;
    docWriter->pauseAllThreads();
    LuceneException finally;
    try {
        // Abort any running merges
        finishMerges(false);

        // Remove any buffered docs
        docWriter->abort();
        docWriter->setFlushedDocCount(0);

        // Remove all segments
        segmentInfos->clear();

        // Ask deleter to locate unreferenced files & remove them
        deleter->checkpoint(segmentInfos, false);
        deleter->refresh();

        // Don't bother saving any changes in our segmentInfos
        readerPool->clear(SegmentInfosPtr());

        // Mark that the index has changed
        ++changeCount;

        success = true;
    } catch (std::bad_alloc& oom) {
        finally = handleOOM(oom, L"deleteAll");
    } catch (LuceneException& e) {
        finally = e;
    }

    docWriter->resumeAllThreads();
    if (!success && infoStream) {
        message(L"hit exception during deleteAll");
    }

    finally.throwException();
}

void IndexWriter::finishMerges(bool waitForMerges) {
    SyncLock syncLock(this);
    if (!waitForMerges) {
        stopMerges = true;

        // Abort all pending and running merges
        for (Collection<OneMergePtr>::iterator merge = pendingMerges.begin(); merge != pendingMerges.end(); ++merge) {
            if (infoStream) {
                message(L"now abort pending merge " + (*merge)->segString(directory));
            }
            (*merge)->abort();
            mergeFinish(*merge);
        }
        pendingMerges.clear();

        for (SetOneMerge::iterator merge = runningMerges.begin(); merge != runningMerges.end(); ++merge) {
            if (infoStream) {
                message(L"now abort running merge " + (*merge)->segString(directory));
            }
            (*merge)->abort();
        }

        // Ensure any running addIndexes finishes.  It's fine if a new one attempts to start because its merges
        // will quickly see the stopMerges == true and abort.
        acquireRead();
        releaseRead();

        // These merges periodically check whether they have been aborted, and stop if so.  We wait here to make
        // sure they all stop.  It should not take very long because the merge threads periodically check if they
        // are aborted.
        while (!runningMerges.empty()) {
            if (infoStream) {
                message(L"now wait for " + StringUtils::toString(runningMerges.size()) + L" running merge to abort");
            }
            doWait();
        }

        stopMerges = false;
        notifyAll();

        BOOST_ASSERT(mergingSegments.empty());

        if (infoStream) {
            message(L"all running merges have aborted");
        }
    } else {
        // waitForMerges() will ensure any running addIndexes finishes.   It's fine if a new one attempts to start
        // because from our caller above the call will see that we are in the process of closing, and will throw
        // an AlreadyClosed exception.
        IndexWriter::waitForMerges();
    }
}

void IndexWriter::waitForMerges() {
    SyncLock syncLock(this);
    // Ensure any running addIndexes finishes.
    acquireRead();
    releaseRead();

    while (!pendingMerges.empty() || !runningMerges.empty()) {
        doWait();
    }

    // sanity check
    BOOST_ASSERT(mergingSegments.empty());
}

void IndexWriter::checkpoint() {
    SyncLock syncLock(this);
    ++changeCount;
    deleter->checkpoint(segmentInfos, false);
}

void IndexWriter::finishAddIndexes() {
    releaseWrite();
}

void IndexWriter::blockAddIndexes(bool includePendingClose) {
    acquireRead();

    bool success = false;
    LuceneException finally;
    try {
        // Make sure we are still open since we could have waited quite a while for last addIndexes to finish
        ensureOpen(includePendingClose);
        success = true;
    } catch (LuceneException& e) {
        finally = e;
    }

    if (!success) {
        releaseRead();
    }
    finally.throwException();
}

void IndexWriter::resumeAddIndexes() {
    releaseRead();
}

void IndexWriter::resetMergeExceptions() {
    SyncLock syncLock(this);
    mergeExceptions.clear();
    ++mergeGen;
}

void IndexWriter::noDupDirs(Collection<DirectoryPtr> dirs) {
    Collection<DirectoryPtr> dups(Collection<DirectoryPtr>::newInstance());

    for (Collection<DirectoryPtr>::iterator dir = dirs.begin(); dir != dirs.end(); ++dir) {
        for (Collection<DirectoryPtr>::iterator dup = dups.begin(); dup != dups.end(); ++dup) {
            if (*dup == *dir) {
                boost::throw_exception(IllegalArgumentException(L"Directory " + (*dir)->getLockID() + L" appears more than once"));
            }
        }
        if (*dir == directory) {
            boost::throw_exception(IllegalArgumentException(L"Cannot add directory to itself"));
        }
        dups.add(*dir);
    }
}

void IndexWriter::addIndexesNoOptimize(Collection<DirectoryPtr> dirs) {
    ensureOpen();

    noDupDirs(dirs);

    // Do not allow add docs or deletes while we are running
    docWriter->pauseAllThreads();

    LuceneException finally;
    try {
        if (infoStream) {
            message(L"flush at addIndexesNoOptimize");
        }
        flush(true, false, true);

        bool success = false;

        startTransaction(false);

        try {
            int32_t docCount = 0;

            {
                SyncLock syncLock(this);
                ensureOpen();

                for (Collection<DirectoryPtr>::iterator dir = dirs.begin(); dir != dirs.end(); ++dir) {
                    if (directory == *dir) {
                        // cannot add this index: segments may be deleted in merge before added
                        boost::throw_exception(IllegalArgumentException(L"Cannot add this index to itself"));
                    }

                    SegmentInfosPtr sis(newLucene<SegmentInfos>()); // read infos from dir
                    sis->read(*dir);

                    for (int32_t j = 0; j < sis->size(); ++j) {
                        SegmentInfoPtr info(sis->info(j));
                        BOOST_ASSERT(!segmentInfos->contains(info));
                        docCount += info->docCount;
                        segmentInfos->add(info); // add each info
                    }
                }
            }

            // Notify DocumentsWriter that the flushed count just increased
            docWriter->updateFlushedDocCount(docCount);

            maybeMerge();

            ensureOpen();

            // If after merging there remain segments in the index that are in a different directory, just copy these
            // over into our index.  This is necessary (before finishing the transaction) to avoid leaving the index
            // in an unusable (inconsistent) state.
            resolveExternalSegments();

            ensureOpen();

            success = true;
        } catch (LuceneException& e) {
            finally = e;
        }

        if (success) {
            commitTransaction();
        } else {
            rollbackTransaction();
        }
    } catch (std::bad_alloc& oom) {
        finally = handleOOM(oom, L"addIndexesNoOptimize");
    } catch (LuceneException& e) {
        finally = e;
    }
    if (docWriter) {
        docWriter->resumeAllThreads();
    }
    finally.throwException();
}

bool IndexWriter::hasExternalSegments() {
    return segmentInfos->hasExternalSegments(directory);
}

void IndexWriter::resolveExternalSegments() {
    bool any = false;
    bool done = false;

    while (!done) {
        SegmentInfoPtr info;
        OneMergePtr merge;

        {
            SyncLock syncLock(this);
            if (stopMerges) {
                boost::throw_exception(MergeAbortedException(L"rollback() was called or addIndexes* hit an unhandled exception"));
            }

            int32_t numSegments = segmentInfos->size();

            done = true;
            for (int32_t i = 0; i < numSegments; ++i) {
                info = segmentInfos->info(i);
                if (info->dir != directory) {
                    done = false;
                    OneMergePtr newMerge(newLucene<OneMerge>(segmentInfos->range(i, i + 1), boost::dynamic_pointer_cast<LogMergePolicy>(mergePolicy) && getUseCompoundFile()));

                    // Returns true if no running merge conflicts with this one (and, records this merge as
                    // pending), ie, this segment is not currently being merged
                    if (registerMerge(newMerge)) {
                        merge = newMerge;

                        // If this segment is not currently being merged, then advance it to running & run
                        // the merge ourself (below)
                        pendingMerges.remove(merge);
                        runningMerges.add(merge);
                        break;
                    }
                }
            }

            if (!done && !merge) {
                // We are not yet done (external segments still exist in segmentInfos), yet, all such segments
                // are currently "covered" by a pending or running merge.  We now try to grab any pending merge
                // that involves external segments
                merge = getNextExternalMerge();
            }

            if (!done && !merge) {
                // We are not yet done, and, all external segments fall under merges that the merge scheduler is
                // currently running.  So, we now wait and check back to see if the merge has completed.
                doWait();
            }
        }

        if (merge) {
            any = true;
            IndexWriter::merge(merge);
        }
    }

    if (any) {
        // Sometimes, on copying an external segment over, more merges may become necessary
        mergeScheduler->merge(shared_from_this());
    }
}

void IndexWriter::addIndexes(Collection<IndexReaderPtr> readers) {
    ensureOpen();

    // Do not allow add docs or deletes while we are running
    docWriter->pauseAllThreads();

    // We must pre-acquire a read lock here (and upgrade to write lock in startTransaction below) so that no
    // other addIndexes is allowed to start up after we have flushed & optimized but before we then start our
    // transaction.  This is because the merging below requires that only one segment is present in the index
    acquireRead();

    LuceneException finally;
    try {
        SegmentInfoPtr info;
        String mergedName;
        SegmentMergerPtr merger;

        bool success = false;

        try {
            flush(true, false, true);
            optimize(); // start with zero or 1 seg
            success = true;
        } catch (LuceneException& e) {
            finally = e;
        }

        // Take care to release the read lock if we hit an exception before starting the transaction
        if (!success) {
            releaseRead();
        }
        finally.throwException();

        // true means we already have a read lock; if this call hits an exception it will release the write lock
        startTransaction(true);

        try {
            mergedName = newSegmentName();
            merger = newLucene<SegmentMerger>(shared_from_this(), mergedName, OneMergePtr());

            SegmentReaderPtr sReader;

            {
                SyncLock syncLock(this);
                if (segmentInfos->size() == 1) { // add existing index, if any
                    sReader = readerPool->get(segmentInfos->info(0), true, BufferedIndexInput::BUFFER_SIZE, -1);
                }
            }

            success = false;

            try {
                if (sReader) {
                    merger->add(sReader);
                }

                for (Collection<IndexReaderPtr>::iterator i = readers.begin(); i != readers.end(); ++i) {
                    merger->add(*i);
                }

                int32_t docCount = merger->merge(); // merge 'em

                {
                    SyncLock syncLock(this);
                    segmentInfos->clear(); // pop old infos & add new
                    info = newLucene<SegmentInfo>(mergedName, docCount, directory, false, true, -1, L"", false, merger->hasProx());
                    setDiagnostics(info, L"addIndexes(Collection<IndexReaderPtr>)");
                    segmentInfos->add(info);
                }

                // Notify DocumentsWriter that the flushed count just increased
                docWriter->updateFlushedDocCount(docCount);

                success = true;
            } catch (LuceneException& e) {
                finally = e;
            }

            if (sReader) {
                readerPool->release(sReader);
            }
        } catch (LuceneException& e) {
            finally = e;
        }

        if (!success) {
            if (infoStream) {
                message(L"hit exception in addIndexes during merge");
            }
            rollbackTransaction();
        } else {
            commitTransaction();
        }

        finally.throwException();

        if (boost::dynamic_pointer_cast<LogMergePolicy>(mergePolicy) && getUseCompoundFile()) {
            HashSet<String> files;

            {
                SyncLock syncLock(this);
                // Must incRef our files so that if another thread is running merge/optimize, it doesn't delete our
                // segment's files before we have a change to finish making the compound file.
                if (segmentInfos->contains(info)) {
                    files = info->files();
                    deleter->incRef(files);
                }
            }

            if (files) {
                success = false;

                startTransaction(false);

                try {
                    merger->createCompoundFile(mergedName + L".cfs");

                    {
                        SyncLock syncLock(this);
                        info->setUseCompoundFile(true);
                    }

                    success = true;
                } catch (LuceneException& e) {
                    finally = e;
                }

                {
                    SyncLock syncLock(this);
                    deleter->decRef(files);
                }

                if (!success) {
                    if (infoStream) {
                        message(L"hit exception building compound file in addIndexes during merge");
                    }
                    rollbackTransaction();
                } else {
                    commitTransaction();
                }
            }
        }
    } catch (std::bad_alloc& oom) {
        finally = handleOOM(oom, L"addIndexes(Collection<IndexReaderPtr>)");
    } catch (LuceneException& e) {
        finally = e;
    }
    if (docWriter) {
        docWriter->resumeAllThreads();
    }
    finally.throwException();
}

void IndexWriter::doAfterFlush() {
    // override
}

void IndexWriter::doBeforeFlush() {
    // override
}

void IndexWriter::prepareCommit() {
    ensureOpen();
    prepareCommit(MapStringString());
}

void IndexWriter::prepareCommit(MapStringString commitUserData) {
    if (hitOOM) {
        boost::throw_exception(IllegalStateException(L"this writer hit an OutOfMemoryError; cannot commit"));
    }

    if (pendingCommit) {
        boost::throw_exception(IllegalStateException(L"prepareCommit was already called with no corresponding call to commit"));
    }

    if (infoStream) {
        message(L"prepareCommit: flush");
    }

    flush(true, true, true);

    startCommit(0, commitUserData);
}

void IndexWriter::commit(int64_t sizeInBytes) {
    SyncLock messageLock(commitLock);
    startCommit(sizeInBytes, MapStringString());
    finishCommit();
}

void IndexWriter::commit() {
    commit(MapStringString());
}

void IndexWriter::commit(MapStringString commitUserData) {
    ensureOpen();

    if (infoStream) {
        message(L"commit: start");
    }

    {
        SyncLock messageLock(commitLock);

        if (infoStream) {
            message(L"commit: enter lock");
        }

        if (!pendingCommit) {
            if (infoStream) {
                message(L"commit: now prepare");
            }
            prepareCommit(commitUserData);
        } else if (infoStream) {
            message(L"commit: already prepared");
        }

        finishCommit();
    }
}

void IndexWriter::finishCommit() {
    SyncLock syncLock(this);
    if (pendingCommit) {
        LuceneException finally;
        try {
            if (infoStream) {
                message(L"commit: pendingCommit != null");
            }
            pendingCommit->finishCommit(directory);
            if (infoStream) {
                message(L"commit: wrote segments file \"" + pendingCommit->getCurrentSegmentFileName() + L"\"");
            }
            lastCommitChangeCount = pendingCommitChangeCount;
            segmentInfos->updateGeneration(pendingCommit);
            segmentInfos->setUserData(pendingCommit->getUserData());
            setRollbackSegmentInfos(pendingCommit);
            deleter->checkpoint(pendingCommit, true);
        } catch (LuceneException& e) {
            finally = e;
        }

        deleter->decRef(pendingCommit);
        pendingCommit.reset();
        notifyAll();
        finally.throwException();
    } else if (infoStream) {
        message(L"commit: pendingCommit == null; skip");
    }

    if (infoStream) {
        message(L"commit: done");
    }
}

void IndexWriter::flush(bool triggerMerge, bool flushDocStores, bool flushDeletes) {
    // We can be called during close, when closing = true, so we must pass false to ensureOpen
    ensureOpen(false);
    if (doFlush(flushDocStores, flushDeletes) && triggerMerge) {
        maybeMerge();
    }
}

bool IndexWriter::doFlush(bool flushDocStores, bool flushDeletes) {
    TestScope testScope(L"IndexWriter", L"doFlush");
    SyncLock syncLock(this);
    bool success = false;
    LuceneException finally;
    try {
        try {
            success = doFlushInternal(flushDocStores, flushDeletes);
        } catch (LuceneException& e) {
            finally = e;
        }
        if (docWriter->doBalanceRAM()) {
            docWriter->balanceRAM();
        }
        finally.throwException();
    } catch (LuceneException& e) {
        finally = e;
    }
    docWriter->clearFlushPending();
    finally.throwException();
    return success;
}

bool IndexWriter::doFlushInternal(bool flushDocStores, bool flushDeletes) {
    SyncLock syncLock(this);
    if (hitOOM) {
        boost::throw_exception(IllegalStateException(L"this writer hit an OutOfMemoryError; cannot flush"));
    }

    ensureOpen(false);

    BOOST_ASSERT(testPoint(L"startDoFlush"));

    doBeforeFlush();

    ++flushCount;

    // If we are flushing because too many deletes accumulated, then we should apply the deletes to free RAM
    if (docWriter->doApplyDeletes()) {
        flushDeletes = true;
    }

    // Make sure no threads are actively adding a document. Returns true if docWriter is currently aborting, in
    // which case we skip flushing this segment
    if (infoStream) {
        message(L"flush: now pause all indexing threads");
    }
    if (docWriter->pauseAllThreads()) {
        docWriter->resumeAllThreads();
        return false;
    }

    bool flushDocs = false;

    LuceneException finally;
    try {
        SegmentInfoPtr newSegment;

        int32_t numDocs = docWriter->getNumDocsInRAM();

        // Always flush docs if there are any
        flushDocs = (numDocs > 0);

        String docStoreSegment(docWriter->getDocStoreSegment());

        BOOST_ASSERT(!docStoreSegment.empty() || numDocs == 0);

        if (docStoreSegment.empty()) {
            flushDocStores = false;
        }

        int32_t docStoreOffset = docWriter->getDocStoreOffset();

        bool docStoreIsCompoundFile = false;

        if (infoStream) {
            message(L" flush: segment=" + docWriter->getSegment() +
                    L" docStoreSegment=" + StringUtils::toString(docWriter->getDocStoreSegment()) +
                    L" docStoreOffset=" + StringUtils::toString(docStoreOffset) +
                    L" flushDocs=" + StringUtils::toString(flushDocs) +
                    L" flushDeletes=" + StringUtils::toString(flushDeletes) +
                    L" flushDocStores=" + StringUtils::toString(flushDocStores) +
                    L" numDocs=" + StringUtils::toString(numDocs) +
                    L" numBufDelTerms=" + StringUtils::toString(docWriter->getNumBufferedDeleteTerms()));
            message(L" index before flush " + segString());
        }

        // Check if the doc stores must be separately flushed because other segments, besides the one we are
        // about to flush, reference it
        if (flushDocStores && (!flushDocs || docWriter->getSegment() != docWriter->getDocStoreSegment())) {
            // We must separately flush the doc store
            if (infoStream) {
                message(L" flush shared docStore segment " + docStoreSegment);
            }

            docStoreIsCompoundFile = IndexWriter::flushDocStores();
            flushDocStores = false;
        }

        String segment(docWriter->getSegment());

        // If we are flushing docs, segment must not be null
        BOOST_ASSERT(!segment.empty() || !flushDocs);

        if (flushDocs) {
            bool success = false;
            int32_t flushedDocCount;

            try {
                flushedDocCount = docWriter->flush(flushDocStores);
                if (infoStream) {
                    message(L"flushedFiles=" + StringUtils::toString(docWriter->getFlushedFiles()));
                }
                success = true;
            } catch (LuceneException& e) {
                finally = e;
            }

            if (!success) {
                if (infoStream) {
                    message(L"hit exception flushing segment " + segment);
                }
                deleter->refresh(segment);
            }

            finally.throwException();

            if (docStoreOffset == 0 && flushDocStores) {
                // This means we are flushing private doc stores with this segment, so it will not be shared
                // with other segments
                BOOST_ASSERT(!docStoreSegment.empty());
                BOOST_ASSERT(docStoreSegment == segment);
                docStoreOffset = -1;
                docStoreIsCompoundFile = false;
                docStoreSegment.clear();
            }

            // Create new SegmentInfo, but do not add to our segmentInfos until deletes are flushed successfully.
            newSegment = newLucene<SegmentInfo>(segment, flushedDocCount, directory, false, true, docStoreOffset, docStoreSegment, docStoreIsCompoundFile, docWriter->hasProx());
            setDiagnostics(newSegment, L"flush");
        }

        docWriter->pushDeletes();

        if (flushDocs) {
            segmentInfos->add(newSegment);
            checkpoint();
        }

        if (flushDocs && mergePolicy->useCompoundFile(segmentInfos, newSegment)) {
            // Now build compound file
            bool success = false;
            try {
                docWriter->createCompoundFile(segment);
                success = true;
            } catch (LuceneException& e) {
                finally = e;
            }

            if (!success) {
                if (infoStream) {
                    message(L"hit exception creating compound file for newly flushed segment " + segment);
                }
                deleter->deleteFile(segment + L"." + IndexFileNames::COMPOUND_FILE_EXTENSION());
            }

            finally.throwException();

            newSegment->setUseCompoundFile(true);
            checkpoint();
        }

        if (flushDeletes) {
            applyDeletes();
        }

        if (flushDocs) {
            checkpoint();
        }

        doAfterFlush();
    } catch (std::bad_alloc& oom) {
        finally = handleOOM(oom, L"doFlush");
        flushDocs = false;
    } catch (LuceneException& e) {
        finally = e;
    }
    docWriter->resumeAllThreads();
    finally.throwException();

    return flushDocs;
}

int64_t IndexWriter::ramSizeInBytes() {
    ensureOpen();
    return docWriter->getRAMUsed();
}

int32_t IndexWriter::numRamDocs() {
    SyncLock syncLock(this);
    ensureOpen();
    return docWriter->getNumDocsInRAM();
}

int32_t IndexWriter::ensureContiguousMerge(const OneMergePtr& merge) {
    int32_t first = segmentInfos->find(merge->segments->info(0));
    if (first == -1) {
        boost::throw_exception(MergeException(L"Could not find segment " + merge->segments->info(0)->name + L" in current index " + segString()));
    }

    int32_t numSegments = segmentInfos->size();
    int32_t numSegmentsToMerge = merge->segments->size();

    for (int32_t i = 0; i < numSegmentsToMerge; ++i) {
        SegmentInfoPtr info(merge->segments->info(i));

        if (first + i >= numSegments || !segmentInfos->info(first + i)->equals(info)) {
            if (!segmentInfos->contains(info)) {
                boost::throw_exception(MergeException(L"MergePolicy selected a segment (" + info->name + L") that is not in the current index " + segString()));
            } else {
                boost::throw_exception(MergeException(L"MergePolicy selected non-contiguous segments to merge (" + merge->segString(directory) + L" vs " + segString() + L"), which IndexWriter (currently) cannot handle"));
            }
        }
    }

    return first;
}

void IndexWriter::commitMergedDeletes(const OneMergePtr& merge, const SegmentReaderPtr& mergeReader) {
    SyncLock syncLock(this);
    BOOST_ASSERT(testPoint(L"startCommitMergeDeletes"));

    SegmentInfosPtr sourceSegments(merge->segments);

    if (infoStream) {
        message(L"commitMergeDeletes " + merge->segString(directory));
    }

    // Carefully merge deletes that occurred after we started merging
    int32_t docUpto = 0;
    int32_t delCount = 0;

    for (int32_t i = 0; i < sourceSegments->size(); ++i) {
        SegmentInfoPtr info(sourceSegments->info(i));
        int32_t docCount = info->docCount;
        SegmentReaderPtr previousReader(merge->readersClone[i]);
        SegmentReaderPtr currentReader(merge->readers[i]);
        if (previousReader->hasDeletions()) {
            // There were deletes on this segment when the merge started.  The merge has collapsed away those deletes,
            // but if new deletes were flushed since the merge started, we must now carefully keep any newly flushed
            // deletes but mapping them to the new docIDs.

            if (currentReader->numDeletedDocs() > previousReader->numDeletedDocs()) {
                // This means this segment has had new deletes committed since we started the merge, so we must merge them
                for (int32_t j = 0; j < docCount; ++j) {
                    if (previousReader->isDeleted(j)) {
                        BOOST_ASSERT(currentReader->isDeleted(j));
                    } else {
                        if (currentReader->isDeleted(j)) {
                            mergeReader->doDelete(docUpto);
                            ++delCount;
                        }
                        ++docUpto;
                    }
                }
            } else {
                docUpto += docCount - previousReader->numDeletedDocs();
            }
        } else if (currentReader->hasDeletions()) {
            // This segment had no deletes before but now it does
            for (int32_t j = 0; j < docCount; ++j) {
                if (currentReader->isDeleted(j)) {
                    mergeReader->doDelete(docUpto);
                    ++delCount;
                }
                ++docUpto;
            }
        } else {
            // No deletes before or after
            docUpto += info->docCount;
        }
    }

    BOOST_ASSERT(mergeReader->numDeletedDocs() == delCount);

    mergeReader->_hasChanges = (delCount > 0);
}

bool IndexWriter::commitMerge(const OneMergePtr& merge, const SegmentMergerPtr& merger, int32_t mergedDocCount, const SegmentReaderPtr& mergedReader) {
    SyncLock syncLock(this);
    BOOST_ASSERT(testPoint(L"startCommitMerge"));

    if (hitOOM) {
        boost::throw_exception(IllegalStateException(L"this writer hit an OutOfMemoryError; cannot complete merge"));
    }

    if (infoStream) {
        message(L"commitMerge: " + merge->segString(directory) + L" index=" + segString());
    }

    BOOST_ASSERT(merge->registerDone);

    // If merge was explicitly aborted, or, if rollback() or rollbackTransaction() had been called since our merge
    // started (which results in an unqualified deleter.refresh() call that will remove any index file that current
    // segments does not reference), we abort this merge
    if (merge->isAborted()) {
        if (infoStream) {
            message(L"commitMerge: skipping merge " + merge->segString(directory) + L": it was aborted");
        }
        return false;
    }

    int32_t start = ensureContiguousMerge(merge);

    commitMergedDeletes(merge, mergedReader);
    docWriter->remapDeletes(segmentInfos, merger->getDocMaps(), merger->getDelCounts(), merge, mergedDocCount);

    // If the doc store we are using has been closed and is in now compound format (but wasn't when we started),
    // then we will switch to the compound format as well
    setMergeDocStoreIsCompoundFile(merge);

    merge->info->setHasProx(merger->hasProx());

    segmentInfos->remove(start, start + merge->segments->size());
    BOOST_ASSERT(!segmentInfos->contains(merge->info));
    segmentInfos->add(start, merge->info);

    closeMergeReaders(merge, false);

    // Must note the change to segmentInfos so any commits in-flight don't lose it
    checkpoint();

    // If the merged segments had pending changes, clear them so that they don't bother writing
    // them to disk, updating SegmentInfo, etc.
    readerPool->clear(merge->segments);

    if (merge->optimize) {
        // cascade the optimize
        segmentsToOptimize.add(merge->info);
    }
    return true;
}

LuceneException IndexWriter::handleMergeException(const LuceneException& exc, const OneMergePtr& merge) {
    if (infoStream) {
        message(L"handleMergeException: merge=" + merge->segString(directory) + L" exc=" + exc.getError());
    }

    // Set the exception on the merge, so if optimize() is waiting on us it sees the root cause exception
    merge->setException(exc);
    addMergeException(merge);

    switch (exc.getType()) {
    case LuceneException::MergeAborted:
        // We can ignore this exception (it happens when close(false) or rollback is called), unless the
        // merge involves segments from external directories, in which case we must throw it so, for
        // example, the rollbackTransaction code in addIndexes* is executed.
        if (merge->isExternal) {
            return exc;
        }
        break;
    case LuceneException::IO:
    case LuceneException::Runtime:
        return exc;
    default:
        return RuntimeException(); // Should not get here
    }
    return LuceneException();
}

void IndexWriter::merge(const OneMergePtr& merge) {
    bool success = false;

    try {
        LuceneException finally;
        try {
            try {
                mergeInit(merge);
                if (infoStream) {
                    message(L"now merge\n merge=" + merge->segString(directory) + L"\n index=" + segString());
                }

                mergeMiddle(merge);
                mergeSuccess(merge);
                success = true;
            } catch (LuceneException& e) {
                finally = handleMergeException(e, merge);
            }

            {
                SyncLock syncLock(this);
                mergeFinish(merge);

                if (!success) {
                    if (infoStream) {
                        message(L"hit exception during merge");
                    }

                    if (merge->info && !segmentInfos->contains(merge->info)) {
                        deleter->refresh(merge->info->name);
                    }
                }

                // This merge (and, generally, any change to the segments) may now enable
                // new merges, so we call merge policy & update pending merges.
                if (success && !merge->isAborted() && !closed && !closing) {
                    updatePendingMerges(merge->maxNumSegmentsOptimize, merge->optimize);
                }
            }
        } catch (LuceneException& e) {
            finally = e;
        }
        finally.throwException();
    } catch (std::bad_alloc& oom) {
        boost::throw_exception(handleOOM(oom, L"merge"));
    }
}

void IndexWriter::mergeSuccess(const OneMergePtr& merge) {
    // override
}

bool IndexWriter::registerMerge(const OneMergePtr& merge) {
    SyncLock syncLock(this);

    if (merge->registerDone) {
        return true;
    }

    if (stopMerges) {
        merge->abort();
        boost::throw_exception(MergeAbortedException(L"merge is aborted: " + merge->segString(directory)));
    }

    int32_t count = merge->segments->size();
    bool isExternal = false;
    for (int32_t i = 0; i < count; ++i) {
        SegmentInfoPtr info(merge->segments->info(i));
        if (mergingSegments.contains(info)) {
            return false;
        }
        if (!segmentInfos->contains(info)) {
            return false;
        }
        if (info->dir != directory) {
            isExternal = true;
        }
        if (segmentsToOptimize.contains(info)) {
            merge->optimize = true;
            merge->maxNumSegmentsOptimize = optimizeMaxNumSegments;
        }
    }

    ensureContiguousMerge(merge);

    pendingMerges.add(merge);

    if (infoStream) {
        message(L"add merge to pendingMerges: " + merge->segString(directory) + L" [total " + StringUtils::toString(pendingMerges.size()) + L" pending]");
    }

    merge->mergeGen = mergeGen;
    merge->isExternal = isExternal;

    // OK it does not conflict; now record that this merge is running (while synchronized)
    // to avoid race condition where two conflicting merges from different threads, start
    for (int32_t i = 0; i < count; ++i) {
        mergingSegments.add(merge->segments->info(i));
    }

    // Merge is now registered
    merge->registerDone = true;
    return true;
}

void IndexWriter::mergeInit(const OneMergePtr& merge) {
    SyncLock syncLock(this);
    bool success = false;
    LuceneException finally;
    try {
        _mergeInit(merge);
        success = true;
    } catch (LuceneException& e) {
        finally = e;
    }

    if (!success) {
        mergeFinish(merge);
    }
    finally.throwException();
}

void IndexWriter::_mergeInit(const OneMergePtr& merge) {
    SyncLock syncLock(this);
    bool test = testPoint(L"startMergeInit");
    BOOST_ASSERT(test);

    BOOST_ASSERT(merge->registerDone);
    BOOST_ASSERT(!merge->optimize || merge->maxNumSegmentsOptimize > 0);

    if (hitOOM) {
        boost::throw_exception(IllegalStateException(L"this writer hit an OutOfMemoryError; cannot merge"));
    }

    if (merge->info) {
        // mergeInit already done
        return;
    }

    if (merge->isAborted()) {
        return;
    }

    applyDeletes();

    SegmentInfosPtr sourceSegments(merge->segments);
    int32_t end = sourceSegments->size();

    // Check whether this merge will allow us to skip merging the doc stores (stored field & vectors).
    // This is a very substantial optimization (saves tons of IO).
    DirectoryPtr lastDir(directory);
    String lastDocStoreSegment;
    int32_t next = -1;

    bool mergeDocStores = false;
    bool doFlushDocStore = false;
    String currentDocStoreSegment(docWriter->getDocStoreSegment());

    // Test each segment to be merged: check if we need to flush/merge doc stores
    for (int32_t i = 0; i < end; ++i) {
        SegmentInfoPtr si(sourceSegments->info(i));

        // If it has deletions we must merge the doc stores
        if (si->hasDeletions()) {
            mergeDocStores = true;
        }

        // If it has its own (private) doc stores we must merge the doc stores
        if (si->getDocStoreOffset() == -1) {
            mergeDocStores = true;
        }

        // If it has a different doc store segment than previous segments, we must merge the doc stores
        String docStoreSegment(si->getDocStoreSegment());
        if (docStoreSegment.empty()) {
            mergeDocStores = true;
        } else if (lastDocStoreSegment.empty()) {
            lastDocStoreSegment = docStoreSegment;
        } else if (lastDocStoreSegment != docStoreSegment) {
            mergeDocStores = true;
        }

        // Segments' docScoreOffsets must be in-order, contiguous.  For the default merge policy now
        // this will always be the case but for an arbitrary merge policy this may not be the case
        if (next == -1) {
            next = si->getDocStoreOffset() + si->docCount;
        } else if (next != si->getDocStoreOffset()) {
            mergeDocStores = true;
        } else {
            next = si->getDocStoreOffset() + si->docCount;
        }

        // If the segment comes from a different directory we must merge
        if (lastDir != si->dir) {
            mergeDocStores = true;
        }

        // If the segment is referencing the current "live" doc store outputs then we must merge
        if (si->getDocStoreOffset() != -1 && !currentDocStoreSegment.empty() && si->getDocStoreSegment() == currentDocStoreSegment) {
            doFlushDocStore = true;
        }
    }

    // if a mergedSegmentWarmer is installed, we must merge the doc stores because we will open a full
    // SegmentReader on the merged segment
    if (!mergeDocStores && mergedSegmentWarmer && !currentDocStoreSegment.empty() && !lastDocStoreSegment.empty() && lastDocStoreSegment == currentDocStoreSegment) {
        mergeDocStores = true;
    }

    int32_t docStoreOffset;
    String docStoreSegment;
    bool docStoreIsCompoundFile;

    if (mergeDocStores) {
        docStoreOffset = -1;
        docStoreSegment.clear();
        docStoreIsCompoundFile = false;
    } else {
        SegmentInfoPtr si(sourceSegments->info(0));
        docStoreOffset = si->getDocStoreOffset();
        docStoreSegment = si->getDocStoreSegment();
        docStoreIsCompoundFile = si->getDocStoreIsCompoundFile();
    }

    if (mergeDocStores && doFlushDocStore) {
        // SegmentMerger intends to merge the doc stores (stored fields, vectors), and at
        // least one of the segments to be merged refers to the currently live doc stores.
        if (infoStream) {
            message(L"now flush at merge");
        }
        doFlush(true, false);
    }

    merge->mergeDocStores = mergeDocStores;

    // Bind a new segment name here so even with ConcurrentMergePolicy we keep deterministic segment names.
    merge->info = newLucene<SegmentInfo>(newSegmentName(), 0, directory, false, true, docStoreOffset, docStoreSegment, docStoreIsCompoundFile, false);

    MapStringString details(MapStringString::newInstance());
    details.put(L"optimize", StringUtils::toString(merge->optimize));
    details.put(L"mergeFactor", StringUtils::toString(end));
    details.put(L"mergeDocStores", StringUtils::toString(mergeDocStores));
    setDiagnostics(merge->info, L"merge", details);

    // Also enroll the merged segment into mergingSegments; this prevents it from getting
    // selected for a merge after our merge is done but while we are building the CFS
    mergingSegments.add(merge->info);
}

void IndexWriter::setDiagnostics(const SegmentInfoPtr& info, const String& source) {
    setDiagnostics(info, source, MapStringString());
}

void IndexWriter::setDiagnostics(const SegmentInfoPtr& info, const String& source, MapStringString details) {
    MapStringString diagnostics(MapStringString::newInstance());
    diagnostics.put(L"source", source);
    diagnostics.put(L"lucene.version", Constants::LUCENE_VERSION);
    diagnostics.put(L"os", Constants::OS_NAME);
    if (details) {
        diagnostics.putAll(details.begin(), details.end());
    }
    info->setDiagnostics(diagnostics);
}

void IndexWriter::mergeFinish(const OneMergePtr& merge) {
    SyncLock syncLock(this);
    // Optimize, addIndexes or finishMerges may be waiting on merges to finish.
    notifyAll();

    // It's possible we are called twice, eg if there was an exception inside mergeInit
    if (merge->registerDone) {
        SegmentInfosPtr sourceSegments(merge->segments);
        int32_t end = sourceSegments->size();
        for (int32_t i = 0; i < end; ++i) {
            mergingSegments.remove(sourceSegments->info(i));
        }

        mergingSegments.remove(merge->info);
        merge->registerDone = false;
    }

    runningMerges.remove(merge);
}

void IndexWriter::setMergeDocStoreIsCompoundFile(const OneMergePtr& merge) {
    SyncLock syncLock(this);

    String mergeDocStoreSegment(merge->info->getDocStoreSegment());
    if (!mergeDocStoreSegment.empty() && !merge->info->getDocStoreIsCompoundFile()) {
        int32_t size = segmentInfos->size();
        for (int32_t i = 0; i < size; ++i) {
            SegmentInfoPtr info(segmentInfos->info(i));
            String docStoreSegment(info->getDocStoreSegment());
            if (!docStoreSegment.empty() && docStoreSegment == mergeDocStoreSegment && info->getDocStoreIsCompoundFile()) {
                merge->info->setDocStoreIsCompoundFile(true);
                break;
            }
        }
    }
}

void IndexWriter::closeMergeReaders(const OneMergePtr& merge, bool suppressExceptions) {
    SyncLock syncLock(this);

    int32_t numSegments = merge->segments->size();
    if (suppressExceptions) {
        // Suppress any new exceptions so we throw the original cause
        for (int32_t i = 0; i < numSegments; ++i) {
            if (merge->readers[i]) {
                try {
                    readerPool->release(merge->readers[i], false);
                } catch (...) {
                }
                merge->readers[i].reset();
            }

            if (merge->readersClone[i]) {
                try {
                    merge->readersClone[i]->close();
                } catch (...) {
                }
                // This was a private clone and we had the only reference
                BOOST_ASSERT(merge->readersClone[i]->getRefCount() == 0);
                merge->readersClone[i].reset();
            }
        }
    } else {
        for (int32_t i = 0; i < numSegments; ++i) {
            if (merge->readers[i]) {
                readerPool->release(merge->readers[i], true);
                merge->readers[i].reset();
            }

            if (merge->readersClone[i]) {
                merge->readersClone[i]->close();
                // This was a private clone and we had the only reference
                BOOST_ASSERT(merge->readersClone[i]->getRefCount() == 0);
                merge->readersClone[i].reset();
            }
        }
    }
}

int32_t IndexWriter::mergeMiddle(const OneMergePtr& merge) {
    merge->checkAborted(directory);

    String mergedName(merge->info->name);
    int32_t mergedDocCount = 0;

    SegmentInfosPtr sourceSegments(merge->segments);
    int32_t numSegments = sourceSegments->size();

    if (infoStream) {
        message(L"merging " + merge->segString(directory));
    }

    SegmentMergerPtr merger(newLucene<SegmentMerger>(shared_from_this(), mergedName, merge));

    merge->readers = Collection<SegmentReaderPtr>::newInstance(numSegments);
    merge->readersClone = Collection<SegmentReaderPtr>::newInstance(numSegments);

    bool mergeDocStores = false;

    String currentDocStoreSegment;
    {
        SyncLock syncLock(this);
        currentDocStoreSegment = docWriter->getDocStoreSegment();
    }

    bool currentDSSMerged = false;

    LuceneException finally;
    // This is try/finally to make sure merger's readers are closed
    bool success = false;
    try {
        int32_t totDocCount = 0;
        for (int32_t i = 0; i < numSegments; ++i) {
            SegmentInfoPtr info(sourceSegments->info(i));

            // Hold onto the "live" reader; we will use this to commit merged deletes
            merge->readers[i] = readerPool->get(info, merge->mergeDocStores, MERGE_READ_BUFFER_SIZE, -1);
            SegmentReaderPtr reader(merge->readers[i]);

            // We clone the segment readers because other deletes may come in while we're merging so we need readers that will not change
            merge->readersClone[i] = boost::dynamic_pointer_cast<SegmentReader>(reader->clone(true));
            SegmentReaderPtr clone(merge->readersClone[i]);
            merger->add(clone);

            if (clone->hasDeletions()) {
                mergeDocStores = true;
            }

            if (info->getDocStoreOffset() != -1 && !currentDocStoreSegment.empty()) {
                currentDSSMerged = currentDSSMerged || (currentDocStoreSegment == info->getDocStoreSegment());
            }

            totDocCount += clone->numDocs();
        }

        if (infoStream) {
            message(L"merge: total " + StringUtils::toString(totDocCount) + L" docs");
        }

        merge->checkAborted(directory);

        // If deletions have arrived and it has now become necessary to merge doc stores, go and open them
        if (mergeDocStores && !merge->mergeDocStores) {
            merge->mergeDocStores = true;

            {
                SyncLock syncLock(this);
                if (currentDSSMerged) {
                    if (infoStream) {
                        message(L"now flush at mergeMiddle");
                    }
                    doFlush(true, false);
                }
            }

            for (Collection<SegmentReaderPtr>::iterator reader = merge->readersClone.begin(); reader != merge->readersClone.end(); ++reader) {
                (*reader)->openDocStores();
            }

            // Clear DSS
            merge->info->setDocStore(-1, L"", false);
        }

        // This is where all the work happens
        merge->info->docCount = merger->merge(merge->mergeDocStores);
        mergedDocCount = merge->info->docCount;

        BOOST_ASSERT(mergedDocCount == totDocCount);

        if (merge->useCompoundFile) {
            success = false;

            String compoundFileName(IndexFileNames::segmentFileName(mergedName, IndexFileNames::COMPOUND_FILE_EXTENSION()));

            try {
                if (infoStream) {
                    message(L"create compound file " + compoundFileName);
                }
                merger->createCompoundFile(compoundFileName);
                success = true;
            } catch (IOException& ioe) {
                SyncLock syncLock(this);
                if (merge->isAborted()) {
                    // This can happen if rollback or close(false) is called - fall through to logic
                    // below to remove the partially created CFS
                } else {
                    finally = handleMergeException(ioe, merge);
                }
            } catch (LuceneException& e) {
                finally = handleMergeException(e, merge);
            }

            if (!success) {
                if (infoStream) {
                    message(L"hit exception creating compound file during merge");
                }
                {
                    SyncLock syncLock(this);
                    deleter->deleteFile(compoundFileName);
                    deleter->deleteNewFiles(merger->getMergedFiles());
                }
            }

            finally.throwException();

            success = false;

            {
                SyncLock syncLock(this);

                // delete new non cfs files directly: they were never registered with IFD
                deleter->deleteNewFiles(merger->getMergedFiles());

                if (merge->isAborted()) {
                    if (infoStream) {
                        message(L"abort merge after building CFS");
                    }
                    deleter->deleteFile(compoundFileName);
                    boost::throw_exception(TemporaryException());
                }
            }

            merge->info->setUseCompoundFile(true);
        }

        int32_t termsIndexDivisor = -1;
        bool loadDocStores = false;

        // if the merged segment warmer was not installed when this merge was started, causing us
        // to not force the docStores to close, we can't warm it now
        bool canWarm = (merge->info->getDocStoreSegment().empty() || currentDocStoreSegment.empty() || merge->info->getDocStoreSegment() == currentDocStoreSegment);

        if (poolReaders && mergedSegmentWarmer && canWarm) {
            // Load terms index & doc stores so the segment warmer can run searches, load documents/term vectors
            termsIndexDivisor = readerTermsIndexDivisor;
            loadDocStores = true;
        }

        SegmentReaderPtr mergedReader(readerPool->get(merge->info, loadDocStores, BufferedIndexInput::BUFFER_SIZE, termsIndexDivisor));

        try {
            if (poolReaders && mergedSegmentWarmer) {
                mergedSegmentWarmer->warm(mergedReader);
            }
            if (!commitMerge(merge, merger, mergedDocCount, mergedReader)) {
                // commitMerge will return false if this merge was aborted
                boost::throw_exception(TemporaryException());
            }
        } catch (LuceneException& e) {
            finally = e;
        }

        {
            SyncLock syncLock(this);
            readerPool->release(mergedReader);
        }

        finally.throwException();

        success = true;
    } catch (LuceneException& e) {
        finally = e;
    }

    // Readers are already closed in commitMerge if we didn't hit an exc
    if (!success) {
        closeMergeReaders(merge, true);
    }

    // has this merge been aborted?
    if (finally.getType() == LuceneException::Temporary) {
        return 0;
    }

    finally.throwException();

    return mergedDocCount;
}

void IndexWriter::addMergeException(const OneMergePtr& merge) {
    SyncLock syncLock(this);
    BOOST_ASSERT(!merge->getException().isNull());
    if (!mergeExceptions.contains(merge) && mergeGen == merge->mergeGen) {
        mergeExceptions.add(merge);
    }
}

bool IndexWriter::applyDeletes() {
    TestScope testScope(L"IndexWriter", L"applyDeletes");
    SyncLock syncLock(this);
    BOOST_ASSERT(testPoint(L"startApplyDeletes"));
    ++flushDeletesCount;
    bool success = false;
    bool changed = false;

    LuceneException finally;
    try {
        changed = docWriter->applyDeletes(segmentInfos);
        success = true;
    } catch (LuceneException& e) {
        finally = e;
    }

    if (!success && infoStream) {
        message(L"hit exception flushing deletes");
    }

    finally.throwException();

    if (changed) {
        checkpoint();
    }
    return changed;
}

int32_t IndexWriter::getBufferedDeleteTermsSize() {
    SyncLock syncLock(this);
    return docWriter->getBufferedDeleteTerms().size();
}

int32_t IndexWriter::getNumBufferedDeleteTerms() {
    SyncLock syncLock(this);
    return docWriter->getNumBufferedDeleteTerms();
}

SegmentInfoPtr IndexWriter::newestSegment() {
    return !segmentInfos->empty() ? segmentInfos->info(segmentInfos->size() - 1) : SegmentInfoPtr();
}

String IndexWriter::segString() {
    return segString(segmentInfos);
}

String IndexWriter::segString(const SegmentInfosPtr& infos) {
    SyncLock syncLock(this);
    StringStream buffer;
    int32_t count = infos->size();
    for (int32_t i = 0; i < count; ++i) {
        if (i > 0) {
            buffer << L" ";
        }
        SegmentInfoPtr info(infos->info(i));
        buffer << info->segString(directory);
        if (info->dir != directory) {
            buffer << L"**";
        }
    }
    return buffer.str();
}

bool IndexWriter::startSync(const String& fileName, HashSet<String> pending) {
    SyncLock syncedLock(&synced);
    if (!synced.contains(fileName)) {
        if (!syncing.contains(fileName)) {
            syncing.add(fileName);
            return true;
        } else {
            pending.add(fileName);
            return false;
        }
    } else {
        return false;
    }
}

void IndexWriter::finishSync(const String& fileName, bool success) {
    SyncLock syncedLock(&synced);
    BOOST_ASSERT(syncing.contains(fileName));
    syncing.remove(fileName);
    if (success) {
        synced.add(fileName);
    }
    synced.notifyAll();
}

bool IndexWriter::waitForAllSynced(HashSet<String> syncing) {
    SyncLock syncedLock(&synced);
    for (HashSet<String>::iterator fileName = syncing.begin(); fileName != syncing.end(); ++fileName) {
        while (!synced.contains(*fileName)) {
            if (!syncing.contains(*fileName)) {
                // There was an error because a file that was previously syncing failed to appear in synced
                return false;
            } else {
                synced.wait();
            }
        }
    }
    return true;
}

void IndexWriter::doWait() {
    SyncLock syncLock(this);
    // NOTE: the callers of this method should in theory be able to do simply wait(), but, as a defense against
    // thread timing hazards where notifyAll() fails to be called, we wait for at most 1 second and then return
    // so caller can check if wait conditions are satisfied
    wait(1000);
}

void IndexWriter::startCommit(int64_t sizeInBytes, MapStringString commitUserData) {
    BOOST_ASSERT(testPoint(L"startStartCommit"));

    if (hitOOM) {
        boost::throw_exception(IllegalStateException(L"this writer hit an OutOfMemoryError; cannot commit"));
    }

    try {
        if (infoStream) {
            message(L"startCommit(): start sizeInBytes=" + StringUtils::toString(sizeInBytes));
        }

        SegmentInfosPtr toSync;
        int64_t myChangeCount = 0;
        LuceneException finally;

        {
            SyncLock syncLock(this);

            // Wait for any running addIndexes to complete first, then block any from running
            // until we've copied the segmentInfos we intend to sync
            blockAddIndexes(false);

            // On commit the segmentInfos must never reference a segment in another directory
            BOOST_ASSERT(!hasExternalSegments());

            try {
                BOOST_ASSERT(lastCommitChangeCount <= changeCount);
                myChangeCount = changeCount;

                if (changeCount == lastCommitChangeCount) {
                    if (infoStream) {
                        message(L" skip startCommit(): no changes pending");
                    }
                    boost::throw_exception(TemporaryException());
                }

                // First, we clone & incref the segmentInfos we intend to sync, then, without locking, we sync() each
                // file referenced by toSync, in the background.  Multiple threads can be doing this at once, if say
                // a large merge and a small merge finish at the same time

                if (infoStream) {
                    message(L"startCommit index=" + segString(segmentInfos) + L" changeCount=" + StringUtils::toString(changeCount));
                }

                readerPool->commit();

                // It's possible another flush (that did not close the open do stores) snook in after the flush we
                // just did, so we remove any tail segments referencing the open doc store from the SegmentInfos
                // we are about to sync (the main SegmentInfos will keep them)
                toSync = boost::dynamic_pointer_cast<SegmentInfos>(segmentInfos->clone());

                String dss(docWriter->getDocStoreSegment());
                if (!dss.empty()) {
                    while (true) {
                        String dss2(toSync->info(toSync->size() - 1)->getDocStoreSegment());
                        if (dss2.empty() || dss2 != dss) {
                            break;
                        }
                        toSync->remove(toSync->size() - 1);
                        ++changeCount;
                    }
                }

                if (commitUserData) {
                    toSync->setUserData(commitUserData);
                }

                deleter->incRef(toSync, false);

                HashSet<String> files(toSync->files(directory, false));
                for (HashSet<String>::iterator fileName = files.begin(); fileName != files.end(); ++fileName) {
                    BOOST_ASSERT(directory->fileExists(*fileName));

                    // If this trips it means we are missing a call to .checkpoint somewhere, because by the
                    // time we are called, deleter should know about every file referenced by the current head
                    // segmentInfos
                    BOOST_ASSERT(deleter->exists(*fileName));
                }
            } catch (LuceneException& e) {
                finally = e;
            }
            resumeAddIndexes();

            // no changes pending?
            if (finally.getType() == LuceneException::Temporary) {
                return;
            }

            finally.throwException();
        }

        BOOST_ASSERT(testPoint(L"midStartCommit"));

        bool setPending = false;

        try {
            // Loop until all files toSync references are sync'd
            while (true) {
                HashSet<String> pending(HashSet<String>::newInstance());
                HashSet<String> files(toSync->files(directory, false));
                for (HashSet<String>::iterator fileName = files.begin(); fileName != files.end(); ++fileName) {
                    if (startSync(*fileName, pending)) {
                        bool success = false;
                        try {
                            // Because we incRef'd this commit point above, the file had better exist
                            BOOST_ASSERT(directory->fileExists(*fileName));

                            if (infoStream) {
                                message(L"now sync " + *fileName);
                            }
                            directory->sync(*fileName);
                            success = true;
                        } catch (LuceneException& e) {
                            finally = e;
                        }
                        finishSync(*fileName, success);
                        finally.throwException();
                    }
                }

                // All files that I require are either synced or being synced by other threads.  If they are being
                // synced, we must at this point block until they are done.  If this returns false, that means an
                // error in another thread resulted in failing to actually sync one of our files, so we repeat
                if (waitForAllSynced(pending)) {
                    break;
                }
            }

            BOOST_ASSERT(testPoint(L"midStartCommit2"));

            {
                SyncLock syncLock(this);

                // If someone saved a newer version of segments file since I first started syncing
                // my version, I can safely skip saving myself since I've been superseded

                while (true) {
                    if (myChangeCount <= lastCommitChangeCount) {
                        if (infoStream) {
                            message(L"sync superseded by newer infos");
                        }
                        break;
                    } else if (!pendingCommit) {
                        // My turn to commit
                        if (segmentInfos->getGeneration() > toSync->getGeneration()) {
                            toSync->updateGeneration(segmentInfos);
                        }

                        bool success = false;
                        try {
                            // Exception here means nothing is prepared (this method unwinds
                            // everything it did on an exception)
                            try {
                                toSync->prepareCommit(directory);
                            } catch (LuceneException& e) {
                                finally = e;
                            }

                            // Have our master segmentInfos record the generations we just prepared.  We do this on
                            // error or success so we don't double-write a segments_N file.
                            segmentInfos->updateGeneration(toSync);
                            finally.throwException();

                            BOOST_ASSERT(!pendingCommit);
                            setPending = true;
                            pendingCommit = toSync;
                            pendingCommitChangeCount = myChangeCount;
                            success = true;
                        } catch (LuceneException& e) {
                            finally = e;
                        }

                        if (!success && infoStream) {
                            message(L"hit exception committing segments file");
                        }
                        finally.throwException();
                        break;
                    } else {
                        // Must wait for other commit to complete
                        doWait();
                    }
                }
            }

            if (infoStream) {
                message(L"done all syncs");
            }
            BOOST_ASSERT(testPoint(L"midStartCommitSuccess"));
        } catch (LuceneException& e) {
            finally = e;
        }

        {
            SyncLock syncLock(this);
            if (!setPending) {
                deleter->decRef(toSync);
            }
        }
        finally.throwException();
    } catch (std::bad_alloc& oom) {
        boost::throw_exception(handleOOM(oom, L"startCommit"));
    }
    BOOST_ASSERT(testPoint(L"finishStartCommit"));
}

bool IndexWriter::isLocked(const DirectoryPtr& directory) {
    return directory->makeLock(WRITE_LOCK_NAME)->isLocked();
}

void IndexWriter::unlock(const DirectoryPtr& directory) {
    directory->makeLock(IndexWriter::WRITE_LOCK_NAME)->release();
}

void IndexWriter::setMergedSegmentWarmer(const IndexReaderWarmerPtr& warmer) {
    mergedSegmentWarmer = warmer;
}

IndexReaderWarmerPtr IndexWriter::getMergedSegmentWarmer() {
    return mergedSegmentWarmer;
}

LuceneException IndexWriter::handleOOM(const std::bad_alloc& oom, const String& location) {
    if (infoStream) {
        message(L"hit OutOfMemoryError inside " + location);
    }
    hitOOM = true;
    return OutOfMemoryError();
}

bool IndexWriter::testPoint(const String& name) {
    return true;
}

bool IndexWriter::nrtIsCurrent(const SegmentInfosPtr& infos) {
    SyncLock syncLock(this);
    if (!infos->equals(segmentInfos)) {
        // if any structural changes (new segments), we are stale
        return false;
    } else if (infos->getGeneration() != segmentInfos->getGeneration()) {
        // if any commit took place since we were opened, we are stale
        return false;
    } else {
        return !docWriter->anyChanges();
    }
}

bool IndexWriter::isClosed() {
    SyncLock syncLock(this);
    return closed;
}

ReaderPool::ReaderPool(const IndexWriterPtr& writer) {
    readerMap = MapSegmentInfoSegmentReader::newInstance();
    _indexWriter = writer;
}

ReaderPool::~ReaderPool() {
}

void ReaderPool::clear(const SegmentInfosPtr& infos) {
    SyncLock syncLock(this);
    if (!infos) {
        for (MapSegmentInfoSegmentReader::iterator ent = readerMap.begin(); ent != readerMap.end(); ++ent) {
            ent->second->_hasChanges = false;
        }
    } else {
        for (int32_t i = 0; i < infos->size(); ++i) {
            MapSegmentInfoSegmentReader::iterator ent = readerMap.find(infos->info(i));
            if (ent != readerMap.end()) {
                ent->second->_hasChanges = false;
            }
        }
    }
}

bool ReaderPool::infoIsLive(const SegmentInfoPtr& info) {
    SyncLock syncLock(this);
    IndexWriterPtr indexWriter(_indexWriter);
    int32_t idx = indexWriter->segmentInfos->find(info);
    BOOST_ASSERT(idx != -1);
    BOOST_ASSERT(indexWriter->segmentInfos->info(idx) == info);
    return true;
}

SegmentInfoPtr ReaderPool::mapToLive(const SegmentInfoPtr& info) {
    SyncLock syncLock(this);
    IndexWriterPtr indexWriter(_indexWriter);
    int32_t idx = indexWriter->segmentInfos->find(info);
    SegmentInfoPtr _info(info);
    if (idx != -1) {
        _info = indexWriter->segmentInfos->info(idx);
    }
    return _info;
}

void ReaderPool::release(const SegmentReaderPtr& sr) {
    release(sr, false);
}

void ReaderPool::release(const SegmentReaderPtr& sr, bool drop) {
    SyncLock syncLock(this);
    IndexWriterPtr indexWriter(_indexWriter);

    bool pooled = readerMap.contains(sr->getSegmentInfo());

    BOOST_ASSERT(!pooled || readerMap.get(sr->getSegmentInfo()) == sr);

    // Drop caller's ref; for an external reader (not pooled), this decRef will close it
    sr->decRef();

    if (pooled && (drop || (!indexWriter->poolReaders && sr->getRefCount() == 1))) {
        // We invoke deleter.checkpoint below, so we must be sync'd on IW if there are changes
        BOOST_ASSERT(!sr->_hasChanges || holdsLock());

        // Discard (don't save) changes when we are dropping the reader; this is used only on the
        // sub-readers after a successful merge.
        sr->_hasChanges = sr->_hasChanges && !drop;

        bool hasChanges = sr->_hasChanges;

        // Drop our ref - this will commit any pending changes to the dir
        sr->close();

        // We are the last ref to this reader; since we're not pooling readers, we release it
        readerMap.remove(sr->getSegmentInfo());

        if (hasChanges) {
            // Must checkpoint with deleter, because this segment reader will have created new
            // _X_N.del file.
            indexWriter->deleter->checkpoint(indexWriter->segmentInfos, false);
        }
    }
}

void ReaderPool::close() {
    SyncLock syncLock(this);
    IndexWriterPtr indexWriter(_indexWriter);

    // We invoke deleter.checkpoint below, so we must be sync'd on IW
    BOOST_ASSERT(holdsLock());

    for (MapSegmentInfoSegmentReader::iterator iter = readerMap.begin(); iter != readerMap.end(); ++iter) {
        if (iter->second->_hasChanges) {
            BOOST_ASSERT(infoIsLive(iter->second->getSegmentInfo()));
            iter->second->doCommit(MapStringString());

            // Must checkpoint with deleter, because this segment reader will have created
            // new _X_N.del file.
            indexWriter->deleter->checkpoint(indexWriter->segmentInfos, false);
        }

        // NOTE: it is allowed that this decRef does not actually close the SR; this can happen when a
        // near real-time reader is kept open after the IndexWriter instance is closed
        iter->second->decRef();
    }
    readerMap.clear();
}

void ReaderPool::commit() {
    SyncLock syncLock(this);
    IndexWriterPtr indexWriter(_indexWriter);

    // We invoke deleter.checkpoint below, so we must be sync'd on IW
    BOOST_ASSERT(holdsLock());

    for (MapSegmentInfoSegmentReader::iterator ent = readerMap.begin(); ent != readerMap.end(); ++ent) {
        if (ent->second->_hasChanges) {
            BOOST_ASSERT(infoIsLive(ent->second->getSegmentInfo()));
            ent->second->doCommit(MapStringString());

            // Must checkpoint with deleter, because this segment reader will have created
            // new _X_N.del file.
            indexWriter->deleter->checkpoint(indexWriter->segmentInfos, false);
        }
    }
}

IndexReaderPtr ReaderPool::getReadOnlyClone(const SegmentInfoPtr& info, bool doOpenStores, int32_t termInfosIndexDivisor) {
    SyncLock syncLock(this);
    SegmentReaderPtr sr(get(info, doOpenStores, BufferedIndexInput::BUFFER_SIZE, termInfosIndexDivisor));
    IndexReaderPtr clone;
    LuceneException finally;
    try {
        clone = boost::dynamic_pointer_cast<IndexReader>(sr->clone(true));
    } catch (LuceneException& e) {
        finally = e;
    }
    sr->decRef();
    finally.throwException();
    return clone;
}

SegmentReaderPtr ReaderPool::get(const SegmentInfoPtr& info, bool doOpenStores) {
    return get(info, doOpenStores, BufferedIndexInput::BUFFER_SIZE, IndexWriterPtr(_indexWriter)->readerTermsIndexDivisor);
}

SegmentReaderPtr ReaderPool::get(const SegmentInfoPtr& info, bool doOpenStores, int32_t readBufferSize, int32_t termsIndexDivisor) {
    SyncLock syncLock(this);
    IndexWriterPtr indexWriter(_indexWriter);
    if (indexWriter->poolReaders) {
        readBufferSize = BufferedIndexInput::BUFFER_SIZE;
    }

    SegmentReaderPtr sr(readerMap.get(info));
    if (!sr) {
        // Returns a ref, which we xfer to readerMap
        sr = SegmentReader::get(false, info->dir, info, readBufferSize, doOpenStores, termsIndexDivisor);
        if (info->dir == indexWriter->directory) {
            // Only pool if reader is not external
            readerMap.put(info, sr);
        }
    } else {
        if (doOpenStores) {
            sr->openDocStores();
        }
        if (termsIndexDivisor != -1 && !sr->termsIndexLoaded()) {
            // If this reader was originally opened because we needed to merge it, we didn't load the terms
            // index.  But now, if the caller wants the terms index (eg because it's doing deletes, or an NRT
            // reader is being opened) we ask the reader to load its terms index.
            sr->loadTermsIndex(termsIndexDivisor);
        }
    }

    // Return a ref to our caller
    if (info->dir == indexWriter->directory) {
        // Only incRef if we pooled (reader is not external)
        sr->incRef();
    }
    return sr;
}

SegmentReaderPtr ReaderPool::getIfExists(const SegmentInfoPtr& info) {
    SyncLock syncLock(this);
    SegmentReaderPtr sr(readerMap.get(info));
    if (sr) {
        sr->incRef();
    }
    return sr;
}

IndexReaderWarmer::~IndexReaderWarmer() {
}

}
