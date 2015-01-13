/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "MergePolicy.h"
#include "SegmentInfos.h"
#include "SegmentInfo.h"
#include "StringUtils.h"

namespace Lucene {

MergePolicy::MergePolicy(const IndexWriterPtr& writer) {
    this->_writer = writer;
}

MergePolicy::~MergePolicy() {
}

OneMerge::OneMerge(const SegmentInfosPtr& segments, bool useCompoundFile) {
    mergeDocStores = false;
    optimize = false;
    registerDone = false;
    mergeGen = 0;
    isExternal = false;
    maxNumSegmentsOptimize = 0;
    aborted = false;

    if (segments->empty()) {
        boost::throw_exception(RuntimeException(L"segments must include at least one segment"));
    }
    this->segments = segments;
    this->useCompoundFile = useCompoundFile;
}

OneMerge::~OneMerge() {
}

void OneMerge::setException(const LuceneException& error) {
    SyncLock syncLock(this);
    this->error = error;
}

LuceneException OneMerge::getException() {
    SyncLock syncLock(this);
    return error;
}

void OneMerge::abort() {
    SyncLock syncLock(this);
    aborted = true;
}

bool OneMerge::isAborted() {
    SyncLock syncLock(this);
    return aborted;
}

void OneMerge::checkAborted(const DirectoryPtr& dir) {
    SyncLock syncLock(this);
    if (aborted) {
        boost::throw_exception(MergeAbortedException(L"merge is aborted: " + segString(dir)));
    }
}

String OneMerge::segString(const DirectoryPtr& dir) {
    StringStream buffer;
    int32_t numSegments = segments->size();
    for (int32_t i = 0; i < numSegments; ++i) {
        if (i > 0) {
            buffer << L" ";
        }
        buffer << segments->info(i)->segString(dir);
    }
    if (info) {
        buffer << L" into " + info->name;
    }
    if (optimize) {
        buffer << L" [optimize]";
    }
    if (mergeDocStores) {
        buffer << L" [mergeDocStores]";
    }
    return buffer.str();
}

MergeSpecification::MergeSpecification() {
    merges = Collection<OneMergePtr>::newInstance();
}

MergeSpecification::~MergeSpecification() {
}

void MergeSpecification::add(const OneMergePtr& merge) {
    merges.add(merge);
}

String MergeSpecification::segString(const DirectoryPtr& dir) {
    String seg(L"MergeSpec:\n");
    int32_t i = 1;
    for (Collection<OneMergePtr>::iterator merge = merges.begin(); merge != merges.end(); ++merge) {
        seg += L"  " + StringUtils::toString(i++) + L": " + (*merge)->segString(dir);
    }
    return seg;
}

}
