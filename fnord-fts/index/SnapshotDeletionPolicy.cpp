/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "SnapshotDeletionPolicy.h"
#include "_SnapshotDeletionPolicy.h"

namespace Lucene {

SnapshotDeletionPolicy::SnapshotDeletionPolicy(const IndexDeletionPolicyPtr& primary) {
    this->primary = primary;
}

SnapshotDeletionPolicy::~SnapshotDeletionPolicy() {
}

void SnapshotDeletionPolicy::onInit(Collection<IndexCommitPtr> commits) {
    SyncLock syncLock(this);
    primary->onInit(wrapCommits(commits));
    lastCommit = commits[commits.size() - 1];
}

void SnapshotDeletionPolicy::onCommit(Collection<IndexCommitPtr> commits) {
    SyncLock syncLock(this);
    primary->onCommit(wrapCommits(commits));
    lastCommit = commits[commits.size() - 1];
}

IndexCommitPtr SnapshotDeletionPolicy::snapshot() {
    SyncLock syncLock(this);
    if (!lastCommit) {
        boost::throw_exception(IllegalStateException(L"no index commits to snapshot"));
    }
    if (_snapshot.empty()) {
        _snapshot = lastCommit->getSegmentsFileName();
    } else {
        boost::throw_exception(IllegalStateException(L"snapshot is already set; please call release() first"));
    }
    return lastCommit;
}

void SnapshotDeletionPolicy::release() {
    SyncLock syncLock(this);
    if (!_snapshot.empty()) {
        _snapshot.clear();
    } else {
        boost::throw_exception(IllegalStateException(L"snapshot was not set; please call snapshot() first"));
    }
}

Collection<IndexCommitPtr> SnapshotDeletionPolicy::wrapCommits(Collection<IndexCommitPtr> commits) {
    Collection<IndexCommitPtr> myCommits(Collection<IndexCommitPtr>::newInstance());
    for (Collection<IndexCommitPtr>::iterator commit = commits.begin(); commit != commits.end(); ++commit) {
        myCommits.add(newLucene<MyCommitPoint>(shared_from_this(), *commit));
    }
    return myCommits;
}

MyCommitPoint::MyCommitPoint(const SnapshotDeletionPolicyPtr& deletionPolicy, const IndexCommitPtr& cp) {
    this->_deletionPolicy = deletionPolicy;
    this->cp = cp;
}

MyCommitPoint::~MyCommitPoint() {
}

String MyCommitPoint::toString() {
    return L"SnapshotDeletionPolicy.SnapshotCommitPoint(" + cp->toString() + L")";
}

String MyCommitPoint::getSegmentsFileName() {
    return cp->getSegmentsFileName();
}

HashSet<String> MyCommitPoint::getFileNames() {
    return cp->getFileNames();
}

DirectoryPtr MyCommitPoint::getDirectory() {
    return cp->getDirectory();
}

void MyCommitPoint::deleteCommit() {
    SnapshotDeletionPolicyPtr deletionPolicy(_deletionPolicy);
    SyncLock policyLock(deletionPolicy);
    // Suppress the delete request if this commit point is our current snapshot.
    if (deletionPolicy->_snapshot.empty() || deletionPolicy->_snapshot != getSegmentsFileName()) {
        cp->deleteCommit();
    }
}

bool MyCommitPoint::isDeleted() {
    return cp->isDeleted();
}

int64_t MyCommitPoint::getVersion() {
    return cp->getVersion();
}

int64_t MyCommitPoint::getGeneration() {
    return cp->getGeneration();
}

MapStringString MyCommitPoint::getUserData() {
    return cp->getUserData();
}

bool MyCommitPoint::isOptimized() {
    return cp->isOptimized();
}

}
