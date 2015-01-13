/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <boost/asio.hpp>
#include <boost/any.hpp>
#include <boost/thread/thread.hpp>
#include "fnord-fts/util/LuceneObject.h"

namespace Lucene {

typedef std::shared_ptr<boost::asio::io_service::work> workPtr;

/// A Future represents the result of an asynchronous computation. Methods are provided to check if the computation
/// is complete, to wait for its completion, and to retrieve the result of the computation. The result can only be
/// retrieved using method get when the computation has completed, blocking if necessary until it is ready.
class Future : public LuceneObject {
public:
    virtual ~Future();

protected:
    boost::any value;

public:
    void set(const boost::any& value) {
        SyncLock syncLock(this);
        this->value = value;
    }

    template <typename TYPE>
    TYPE get() {
        SyncLock syncLock(this);
        while (value.empty()) {
            wait(10);
        }
        return value.empty() ? TYPE() : boost::any_cast<TYPE>(value);
    }
};

/// Utility class to handle a pool of threads.
class ThreadPool : public LuceneObject {
public:
    ThreadPool();
    virtual ~ThreadPool();

    LUCENE_CLASS(ThreadPool);

protected:
    boost::asio::io_service io_service;
    workPtr work;
    boost::thread_group threadGroup;

    static const int32_t THREADPOOL_SIZE;

public:
    /// Get singleton thread pool instance.
    static ThreadPoolPtr getInstance();

    template <typename FUNC>
    FuturePtr scheduleTask(FUNC func) {
        FuturePtr future(newInstance<Future>());
        io_service.post(boost::bind(&ThreadPool::execute<FUNC>, this, func, future));
        return future;
    }

protected:
    // this will be executed when one of the threads is available
    template <typename FUNC>
    void execute(FUNC func, const FuturePtr& future) {
        future->set(func());
        future->notifyAll();
    }
};

}

#endif
