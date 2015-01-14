/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef CONCURRENTMERGESCHEDULER_H
#define CONCURRENTMERGESCHEDULER_H

#include "fnord-fts/index/MergeScheduler.h"

namespace fnord {
namespace fts {

/// A {@link MergeScheduler} that runs each merge using a separate thread, up until a
/// maximum number of threads ({@link #setMaxThreadCount}) at which when a merge is needed,
/// the thread(s) that are updating the index will pause until one or more merges completes.
/// This is a simple way to use concurrency in the indexing process without having to create
/// and manage application level threads.
class ConcurrentMergeScheduler : public MergeScheduler {
public:
    ConcurrentMergeScheduler();
    virtual ~ConcurrentMergeScheduler();

    LUCENE_CLASS(ConcurrentMergeScheduler);

protected:
    int32_t mergeThreadPriority;

    SetMergeThread mergeThreads;

    /// Max number of threads allowed to be merging at once
    int32_t maxThreadCount;

    DirectoryPtr dir;

    bool closed;
    IndexWriterWeakPtr _writer;

    static Collection<ConcurrentMergeSchedulerPtr> allInstances;

    bool suppressExceptions;
    static bool anyExceptions;

public:
    virtual void initialize();

    /// Sets the max # simultaneous threads that may be running.  If a merge is necessary yet
    /// we already have this many threads running, the incoming thread (that is calling
    /// add/updateDocument) will block until a merge thread has completed.
    virtual void setMaxThreadCount(int32_t count);

    /// Get the max # simultaneous threads that may be running. @see #setMaxThreadCount.
    virtual int32_t getMaxThreadCount();

    /// Return the priority that merge threads run at.  By default the priority is 1 plus the
    /// priority of (ie, slightly higher priority than) the first thread that calls merge.
    virtual int32_t getMergeThreadPriority();

    /// Set the priority that merge threads run at.
    virtual void setMergeThreadPriority(int32_t pri);

    virtual void close();

    virtual void sync();

    virtual void merge(const IndexWriterPtr& writer);

    /// Used for testing
    static bool anyUnhandledExceptions();
    static void clearUnhandledExceptions();

    /// Used for testing
    void setSuppressExceptions();
    void clearSuppressExceptions();

    /// Used for testing
    static void setTestMode();

protected:
    virtual bool verbose();
    virtual void message(const String& message);
    virtual void initMergeThreadPriority();
    virtual int32_t mergeThreadCount();

    /// Does the actual merge, by calling {@link IndexWriter#merge}
    virtual void doMerge(const OneMergePtr& merge);

    virtual MergeThreadPtr getMergeThread(const IndexWriterPtr& writer, const OneMergePtr& merge);

    /// Called when an exception is hit in a background merge thread
    virtual void handleMergeException(const LuceneException& exc);

    virtual void addMyself();

    friend class MergeThread;
};

}

}
#endif
