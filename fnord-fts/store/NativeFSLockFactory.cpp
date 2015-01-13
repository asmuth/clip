/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include <boost/filesystem/fstream.hpp>
#include <boost/interprocess/sync/file_lock.hpp>
#include "NativeFSLockFactory.h"
#include "_NativeFSLockFactory.h"
#include "Random.h"
#include "FileUtils.h"
#include "StringUtils.h"

namespace Lucene {

NativeFSLockFactory::NativeFSLockFactory(const String& lockDirName) {
    setLockDir(lockDirName);
}

NativeFSLockFactory::~NativeFSLockFactory() {
}

LockPtr NativeFSLockFactory::makeLock(const String& lockName) {
    SyncLock syncLock(this);
    return newLucene<NativeFSLock>(lockDir, lockPrefix.empty() ? lockName : lockPrefix + L"-" + lockName);
}

void NativeFSLockFactory::clearLock(const String& lockName) {
    // note that this isn't strictly required anymore because the existence of these files does not mean
    // they are locked, but still do this in case people really want to see the files go away

    if (FileUtils::isDirectory(lockDir)) {
        String lockPath(FileUtils::joinPath(lockDir, lockPrefix.empty() ? lockName : lockPrefix + L"-" + lockName));
        if (FileUtils::fileExists(lockPath) && !FileUtils::removeFile(lockPath)) {
            boost::throw_exception(IOException(L"Failed to delete: " + lockPath));
        }
    }
}

NativeFSLock::NativeFSLock(const String& lockDir, const String& lockFileName) {
    this->lockDir = lockDir;
    path = FileUtils::joinPath(lockDir, lockFileName);
}

NativeFSLock::~NativeFSLock() {
    try {
        release();
    } catch (...) {
    }
}

SynchronizePtr NativeFSLock::LOCK_HELD_LOCK() {
    static SynchronizePtr _LOCK_HELD_LOCK;
    if (!_LOCK_HELD_LOCK) {
        _LOCK_HELD_LOCK = newInstance<Synchronize>();
    }
    return _LOCK_HELD_LOCK;
}

HashSet<String> NativeFSLock::LOCK_HELD() {
    static HashSet<String> _LOCK_HELD;
    if (!_LOCK_HELD) {
        _LOCK_HELD = HashSet<String>::newInstance();
    }
    return _LOCK_HELD;
}

bool NativeFSLock::lockExists() {
    SyncLock syncLock(this);
    return lock.get() != NULL;
}

bool NativeFSLock::obtain() {
    SyncLock syncLock(this);

    if (lockExists())
        // our instance is already locked
    {
        return false;
    }

    // ensure that lockdir exists and is a directory
    if (!FileUtils::fileExists(lockDir)) {
        if (!FileUtils::createDirectory(lockDir)) {
            boost::throw_exception(IOException(L"Cannot create directory: " + lockDir));
        }
    } else if (!FileUtils::isDirectory(lockDir)) {
        boost::throw_exception(IOException(L"Found regular file where directory expected: " + lockDir));
    }

    bool markedHeld = false;

    // make sure nobody else in-process has this lock held already and mark it held if not
    {
        SyncLock heldLock(LOCK_HELD_LOCK());
        if (LOCK_HELD().contains(path))
            // someone else already has the lock
        {
            return false;
        } else {
            // this "reserves" the fact that we are the one thread trying to obtain this lock, so we own the
            // only instance of a channel against this file
            LOCK_HELD().add(path);
            markedHeld = true;
        }
    }

    try {
        // we can get intermittent "access denied" here, so we treat this as failure to acquire the lock
        boost::filesystem::ofstream f(path, std::ios::binary | std::ios::out);

        if (f.is_open()) {
            std::string lockpath;

            // file_lock only accepts char* filenames and we cannot losslessly convert Unicode paths to
            // char*. The usual way to work around this is to use 8.3 short names.
#if defined(_WIN32) || defined(_WIN64)
            wchar_t pathOut[MAX_PATH+1];
            if (::GetShortPathNameW(path.c_str(), pathOut, MAX_PATH+1) != 0) {
                lockpath = boost::filesystem::path(pathOut).string();
            } else
#endif // Windows
            {
                lockpath = boost::filesystem::path(path).string();
            }
            lock = newInstance<boost::interprocess::file_lock>(lockpath.c_str());
            lock->lock();
        }
    } catch (...) {
        lock.reset();
    }

    if (markedHeld && !lockExists()) {
        SyncLock heldLock(LOCK_HELD_LOCK());
        LOCK_HELD().remove(path);
    }

    return lockExists();
}

void NativeFSLock::release() {
    SyncLock syncLock(this);

    if (lockExists()) {
        try {
            lock->unlock();
            lock.reset();
        } catch (...) {
        }

        {
            SyncLock heldLock(LOCK_HELD_LOCK());
            LOCK_HELD().remove(path);
        }

        // we don't care anymore if the file cannot be deleted because it's held up by another process
        // (eg. AntiVirus). NativeFSLock does not depend on the existence/absence of the lock file
        FileUtils::removeFile(path);
    } else {
        // if we don't hold the lock, and somebody still called release(), for example as a result of
        // calling IndexWriter.unlock(), we should attempt to obtain the lock and release it.  If the
        // obtain fails, it means the lock cannot be released, and we should throw a proper exception
        // rather than silently failing/not doing anything.
        bool obtained = false;
        LuceneException finally;
        try {
            obtained = obtain();
            if (!obtained) {
                boost::throw_exception(LockReleaseFailedException(L"Cannot forcefully unlock a NativeFSLock which is held by another indexer component: " + path));
            }
        } catch (LuceneException& e) {
            finally = e;
        }
        if (obtained) {
            release();
        }
        finally.throwException();
    }
}

bool NativeFSLock::isLocked() {
    SyncLock syncLock(this);

    // the test for is islocked is not directly possible with native file locks

    // first a shortcut, if a lock reference in this instance is available
    if (lockExists()) {
        return true;
    }

    // look if lock file is present; if not, there can definitely be no lock!
    if (!FileUtils::fileExists(path)) {
        return false;
    }

    // try to obtain and release (if was locked) the lock
    try {
        bool obtained = obtain();
        if (obtained) {
            release();
        }
        return !obtained;
    } catch (LuceneException&) {
        return false;
    }
}

String NativeFSLock::toString() {
    return getClassName() + L"@" + path;
}

}
