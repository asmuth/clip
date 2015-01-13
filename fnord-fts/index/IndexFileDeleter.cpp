/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include <boost/algorithm/string.hpp>
#include "IndexFileDeleter.h"
#include "IndexFileNameFilter.h"
#include "IndexFileNames.h"
#include "IndexDeletionPolicy.h"
#include "SegmentInfos.h"
#include "SegmentInfo.h"
#include "Directory.h"
#include "DocumentsWriter.h"
#include "InfoStream.h"
#include "DateTools.h"
#include "LuceneThread.h"
#include "MiscUtils.h"
#include "StringUtils.h"

namespace Lucene {

/// Change to true to see details of reference counts when infoStream != null
bool IndexFileDeleter::VERBOSE_REF_COUNTS = false;

IndexFileDeleter::IndexFileDeleter(const DirectoryPtr& directory, const IndexDeletionPolicyPtr& policy, const SegmentInfosPtr& segmentInfos, const InfoStreamPtr& infoStream, const DocumentsWriterPtr& docWriter, HashSet<String> synced) {
    this->lastFiles = Collection< HashSet<String> >::newInstance();
    this->commits = Collection<IndexCommitPtr>::newInstance();
    this->commitsToDelete = Collection<CommitPointPtr>::newInstance();
    this->refCounts = MapStringRefCount::newInstance();
    this->docWriter = docWriter;
    this->infoStream = infoStream;
    this->synced = synced;

    if (infoStream) {
        message(L"init: current segments file is \"" + segmentInfos->getCurrentSegmentFileName());
    }

    this->policy = policy;
    this->directory = directory;

    // First pass: walk the files and initialize our ref counts
    int64_t currentGen = segmentInfos->getGeneration();
    IndexFileNameFilterPtr filter(IndexFileNameFilter::getFilter());

    HashSet<String> files(directory->listAll());
    CommitPointPtr currentCommitPoint;

    for (HashSet<String>::iterator fileName = files.begin(); fileName != files.end(); ++fileName) {
        if (filter->accept(L"", *fileName) && *fileName != IndexFileNames::SEGMENTS_GEN()) {
            // Add this file to refCounts with initial count 0
            getRefCount(*fileName);

            if (boost::starts_with(*fileName, IndexFileNames::SEGMENTS())) {
                // This is a commit (segments or segments_N), and it's valid (<= the max gen).
                // Load it, then incref all files it refers to
                if (infoStream) {
                    message(L"init: load commit \"" + *fileName + L"\"");
                }
                SegmentInfosPtr sis(newLucene<SegmentInfos>());
                try {
                    sis->read(directory, *fileName);
                } catch (IOException& e) {
                    if (SegmentInfos::generationFromSegmentsFileName(*fileName) <= currentGen) {
                        boost::throw_exception(e);
                    } else {
                        // Most likely we are opening an index that has an aborted "future" commit,
                        // so suppress exc in this case
                        sis.reset();
                    }
                } catch (...) {
                    if (infoStream) {
                        message(L"init: hit exception when loading commit \"" + *fileName + L"\"; skipping this commit point");
                    }
                    sis.reset();
                }
                if (sis) {
                    CommitPointPtr commitPoint(newLucene<CommitPoint>(commitsToDelete, directory, sis));
                    if (sis->getGeneration() == segmentInfos->getGeneration()) {
                        currentCommitPoint = commitPoint;
                    }
                    commits.add(commitPoint);
                    incRef(sis, true);

                    if (!lastSegmentInfos || sis->getGeneration() > lastSegmentInfos->getGeneration()) {
                        lastSegmentInfos = sis;
                    }
                }
            }
        }
    }

    if (!currentCommitPoint) {
        // We did not in fact see the segments_N file corresponding to the segmentInfos that was passed
        // in.  Yet, it must exist, because our caller holds the write lock.  This can happen when the
        // directory listing was stale (eg when index accessed via NFS client with stale directory listing
        // cache).  So we try now to explicitly open this commit point.
        SegmentInfosPtr sis(newLucene<SegmentInfos>());
        try {
            sis->read(directory, segmentInfos->getCurrentSegmentFileName());
        } catch (LuceneException&) {
            boost::throw_exception(CorruptIndexException(L"failed to locate current segments_N file"));
        }
        if (infoStream) {
            message(L"forced open of current segments file " + segmentInfos->getCurrentSegmentFileName());
        }
        currentCommitPoint = newLucene<CommitPoint>(commitsToDelete, directory, sis);
        commits.add(currentCommitPoint);
        incRef(sis, true);
    }

    // We keep commits list in sorted order (oldest to newest)
    std::sort(commits.begin(), commits.end(), luceneCompare<IndexCommitPtr>());

    // Now delete anything with ref count at 0.  These are presumably abandoned files eg due to crash of IndexWriter.
    for (MapStringRefCount::iterator entry = refCounts.begin(); entry != refCounts.end(); ++entry) {
        if (entry->second->count == 0) {
            if (infoStream) {
                message(L"init: removing unreferenced file \"" + entry->first + L"\"");
            }
            deleteFile(entry->first);
        }
    }

    // Finally, give policy a chance to remove things on startup
    policy->onInit(commits);

    // Always protect the incoming segmentInfos since sometime it may not be the most recent commit
    checkpoint(segmentInfos, false);

    startingCommitDeleted = currentCommitPoint->isDeleted();

    deleteCommits();
}

IndexFileDeleter::~IndexFileDeleter() {
}

void IndexFileDeleter::setInfoStream(const InfoStreamPtr& infoStream) {
    this->infoStream = infoStream;
}

void IndexFileDeleter::message(const String& message) {
    if (infoStream) {
        *infoStream << L"IFD [" << DateTools::timeToString(MiscUtils::currentTimeMillis(), DateTools::RESOLUTION_SECOND);
        *infoStream << L"; " << StringUtils::toString(LuceneThread::currentId()) << L"]: " << message << L"\n";
    }
}

SegmentInfosPtr IndexFileDeleter::getLastSegmentInfos() {
    return lastSegmentInfos;
}

void IndexFileDeleter::deleteCommits() {
    if (!commitsToDelete.empty()) {
        // First decref all files that had been referred to by the now-deleted commits
        for (Collection<CommitPointPtr>::iterator commit = commitsToDelete.begin(); commit != commitsToDelete.end(); ++commit) {
            if (infoStream) {
                message(L"deleteCommits: now decRef commit \"" + (*commit)->getSegmentsFileName() + L"\"");
            }
            for (HashSet<String>::iterator file = (*commit)->files.begin(); file != (*commit)->files.end(); ++file) {
                decRef(*file);
            }
        }
        commitsToDelete.clear();

        // Now compact commits to remove deleted ones (preserving the sort)
        int32_t size = commits.size();
        int32_t readFrom = 0;
        int32_t writeTo = 0;
        while (readFrom < size) {
            CommitPointPtr commit(boost::dynamic_pointer_cast<CommitPoint>(commits[readFrom]));
            if (!commit->deleted) {
                if (writeTo != readFrom) {
                    commits[writeTo] = commits[readFrom];
                }
                ++writeTo;
            }
            ++readFrom;
        }

        while (size > writeTo) {
            commits.removeLast();
            --size;
        }
    }
}

void IndexFileDeleter::refresh(const String& segmentName) {
    HashSet<String> files(directory->listAll());
    IndexFileNameFilterPtr filter(IndexFileNameFilter::getFilter());
    String segmentPrefix1(segmentName + L".");
    String segmentPrefix2(segmentName + L"_");

    for (HashSet<String>::iterator fileName = files.begin(); fileName != files.end(); ++fileName) {
        if (filter->accept(L"", *fileName) &&
                (segmentName.empty() || boost::starts_with(*fileName, segmentPrefix1) || boost::starts_with(*fileName, segmentPrefix2)) &&
                !refCounts.contains(*fileName) && *fileName != IndexFileNames::SEGMENTS_GEN()) {
            // Unreferenced file, so remove it
            if (infoStream) {
                message(L"refresh [prefix=" + segmentName + L"]: removing newly created unreferenced file \"" + *fileName + L"\"");
            }
            deleteFile(*fileName);
        }
    }
}

void IndexFileDeleter::refresh() {
    refresh(L"");
}

void IndexFileDeleter::close() {
    // DecRef old files from the last checkpoint, if any
    for (Collection< HashSet<String> >::iterator file = lastFiles.begin(); file != lastFiles.end(); ++file) {
        decRef(*file);
    }
    lastFiles.clear();
    deletePendingFiles();
}

void IndexFileDeleter::deletePendingFiles() {
    if (deletable) {
        HashSet<String> oldDeletable(deletable);
        deletable.reset();
        for (HashSet<String>::iterator fileName = oldDeletable.begin(); fileName != oldDeletable.end(); ++fileName) {
            if (infoStream) {
                message(L"delete pending file " + *fileName);
            }
            deleteFile(*fileName);
        }
    }
}

void IndexFileDeleter::checkpoint(const SegmentInfosPtr& segmentInfos, bool isCommit) {
    if (infoStream) {
        message(L"now checkpoint \"" + segmentInfos->getCurrentSegmentFileName() + L"\" [" + StringUtils::toString(segmentInfos->size()) + L" segments; isCommit = " + StringUtils::toString(isCommit) + L"]");
    }

    // Try again now to delete any previously un-deletable files (because they were in use, on Windows)
    deletePendingFiles();

    // Incref the files
    incRef(segmentInfos, isCommit);

    if (isCommit) {
        // Append to our commits list
        commits.add(newLucene<CommitPoint>(commitsToDelete, directory, segmentInfos));

        // Tell policy so it can remove commits
        policy->onCommit(commits);

        // Decref files for commits that were deleted by the policy
        deleteCommits();
    } else {
        HashSet<String> docWriterFiles;
        if (docWriter) {
            docWriterFiles = docWriter->openFiles();
            if (docWriterFiles) {
                // We must incRef these files before decRef'ing last files to make sure we
                // don't accidentally delete them
                incRef(docWriterFiles);
            }
        }

        // DecRef old files from the last checkpoint, if any
        for (Collection< HashSet<String> >::iterator file = lastFiles.begin(); file != lastFiles.end(); ++file) {
            decRef(*file);
        }
        lastFiles.clear();

        // Save files so we can decr on next checkpoint/commit
        lastFiles.add(segmentInfos->files(directory, false));

        if (docWriterFiles) {
            lastFiles.add(docWriterFiles);
        }
    }
}

void IndexFileDeleter::incRef(const SegmentInfosPtr& segmentInfos, bool isCommit) {
    // If this is a commit point, also incRef the segments_N file
    HashSet<String> files(segmentInfos->files(directory, isCommit));
    for (HashSet<String>::iterator fileName = files.begin(); fileName != files.end(); ++fileName) {
        incRef(*fileName);
    }
}

void IndexFileDeleter::incRef(HashSet<String> files) {
    for (HashSet<String>::iterator file = files.begin(); file != files.end(); ++file) {
        incRef(*file);
    }
}

void IndexFileDeleter::incRef(const String& fileName) {
    RefCountPtr rc(getRefCount(fileName));
    if (infoStream && VERBOSE_REF_COUNTS) {
        message(L"  IncRef \"" + fileName + L"\": pre-incr count is " + StringUtils::toString(rc->count));
    }
    rc->IncRef();
}

void IndexFileDeleter::decRef(HashSet<String> files) {
    for (HashSet<String>::iterator file = files.begin(); file != files.end(); ++file) {
        decRef(*file);
    }
}

void IndexFileDeleter::decRef(const String& fileName) {
    RefCountPtr rc(getRefCount(fileName));
    if (infoStream && VERBOSE_REF_COUNTS) {
        message(L"  DecRef \"" + fileName + L"\": pre-decr count is " + StringUtils::toString(rc->count));
    }
    if (rc->DecRef() == 0) {
        // This file is no longer referenced by any past commit points nor by the in-memory SegmentInfos
        deleteFile(fileName);
        refCounts.remove(fileName);

        if (synced) {
            SyncLock syncLock(&synced);
            synced.remove(fileName);
        }
    }
}

void IndexFileDeleter::decRef(const SegmentInfosPtr& segmentInfos) {
    decRef(segmentInfos->files(directory, false));
}

bool IndexFileDeleter::exists(const String& fileName) {
    return refCounts.contains(fileName) ? getRefCount(fileName)->count > 0 : false;
}

RefCountPtr IndexFileDeleter::getRefCount(const String& fileName) {
    RefCountPtr rc;
    MapStringRefCount::iterator ref = refCounts.find(fileName);
    if (ref == refCounts.end()) {
        rc = newLucene<RefCount>(fileName);
        refCounts.put(fileName, rc);
    } else {
        rc = ref->second;
    }
    return rc;
}

void IndexFileDeleter::deleteFiles(HashSet<String> files) {
    for (HashSet<String>::iterator file = files.begin(); file != files.end(); ++file) {
        deleteFile(*file);
    }
}

void IndexFileDeleter::deleteNewFiles(HashSet<String> files) {
    for (HashSet<String>::iterator fileName = files.begin(); fileName != files.end(); ++fileName) {
        if (!refCounts.contains(*fileName)) {
            if (infoStream) {
                message(L"delete new file \"" + *fileName + L"\"");
            }
            deleteFile(*fileName);
        }
    }
}

void IndexFileDeleter::deleteFile(const String& fileName) {
    try {
        if (infoStream) {
            message(L"delete \"" + fileName + L"\"");
        }
        directory->deleteFile(fileName);
    } catch (IOException& e) { // if delete fails
        if (directory->fileExists(fileName)) { // if delete fails
            // Some operating systems (eg. Windows) don't permit a file to be deleted while it is opened
            // for read (eg. by another process or thread). So we assume that when a delete fails it is
            // because the file is open in another process, and queue the file for subsequent deletion.
            if (infoStream) {
                message(L"IndexFileDeleter: unable to remove file \"" + fileName + L"\": " + e.getError() + L"; Will re-try later.");
            }
            if (!deletable) {
                deletable = HashSet<String>::newInstance();
            }
            deletable.add(fileName); // add to deletable
        }
    }
}

RefCount::RefCount(const String& fileName) {
    initDone = false;
    count = 0;
    this->fileName = fileName;
}

RefCount::~RefCount() {
}

int32_t RefCount::IncRef() {
    if (!initDone) {
        initDone = true;
    } else {
        BOOST_ASSERT(count > 0);
    }
    return ++count;
}

int32_t RefCount::DecRef() {
    BOOST_ASSERT(count > 0);
    return --count;
}

CommitPoint::CommitPoint(Collection<CommitPointPtr> commitsToDelete, const DirectoryPtr& directory, const SegmentInfosPtr& segmentInfos) {
    deleted = false;

    this->directory = directory;
    this->commitsToDelete = commitsToDelete;
    userData = segmentInfos->getUserData();
    segmentsFileName = segmentInfos->getCurrentSegmentFileName();
    version = segmentInfos->getVersion();
    generation = segmentInfos->getGeneration();
    HashSet<String> files(segmentInfos->files(directory, true));
    this->files = HashSet<String>::newInstance(files.begin(), files.end());
    gen = segmentInfos->getGeneration();
    _isOptimized = (segmentInfos->size() == 1 && !segmentInfos->info(0)->hasDeletions());

    BOOST_ASSERT(!segmentInfos->hasExternalSegments(directory));
}

CommitPoint::~CommitPoint() {
}

String CommitPoint::toString() {
    return L"IndexFileDeleter::CommitPoint(" + segmentsFileName + L")";
}

bool CommitPoint::isOptimized() {
    return _isOptimized;
}

String CommitPoint::getSegmentsFileName() {
    return segmentsFileName;
}

HashSet<String> CommitPoint::getFileNames() {
    return files;
}

DirectoryPtr CommitPoint::getDirectory() {
    return directory;
}

int64_t CommitPoint::getVersion() {
    return version;
}

int64_t CommitPoint::getGeneration() {
    return generation;
}

MapStringString CommitPoint::getUserData() {
    return userData;
}

void CommitPoint::deleteCommit() {
    if (!deleted) {
        deleted = true;
        commitsToDelete.add(shared_from_this());
    }
}

bool CommitPoint::isDeleted() {
    return deleted;
}

int32_t CommitPoint::compareTo(const LuceneObjectPtr& other) {
    CommitPointPtr otherCommit(boost::static_pointer_cast<CommitPoint>(other));
    if (gen < otherCommit->gen) {
        return -1;
    }
    if (gen > otherCommit->gen) {
        return 1;
    }
    return 0;
}

}
