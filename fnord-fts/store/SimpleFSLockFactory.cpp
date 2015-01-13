/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include <boost/filesystem/fstream.hpp>
#include "SimpleFSLockFactory.h"
#include "_SimpleFSLockFactory.h"
#include "FileUtils.h"
#include "StringUtils.h"

namespace Lucene {

SimpleFSLockFactory::SimpleFSLockFactory() {
}

SimpleFSLockFactory::SimpleFSLockFactory(const String& lockDir) {
    setLockDir(lockDir);
}

SimpleFSLockFactory::~SimpleFSLockFactory() {
}

LockPtr SimpleFSLockFactory::makeLock(const String& lockName) {
    return newLucene<SimpleFSLock>(lockDir, lockPrefix.empty() ? lockName : lockPrefix + L"-" + lockName);
}

void SimpleFSLockFactory::clearLock(const String& lockName) {
    if (FileUtils::isDirectory(lockDir)) {
        String lockPath(FileUtils::joinPath(lockDir, lockPrefix.empty() ? lockName : lockPrefix + L"-" + lockName));
        if (FileUtils::fileExists(lockPath) && !FileUtils::removeFile(lockPath)) {
            boost::throw_exception(IOException(L"Cannot delete " + lockPath));
        }
    }
}

SimpleFSLock::SimpleFSLock(const String& lockDir, const String& lockFileName) {
    this->lockDir = lockDir;
    this->lockFile = lockFile;
}

SimpleFSLock::~SimpleFSLock() {
}

bool SimpleFSLock::obtain() {
    // Ensure that lockDir exists and is a directory
    if (!FileUtils::fileExists(lockDir)) {
        if (!FileUtils::createDirectory(lockDir)) {
            boost::throw_exception(RuntimeException(L"Cannot create directory: " + lockDir));
        }
    } else if (!FileUtils::isDirectory(lockDir)) {
        boost::throw_exception(RuntimeException(L"Found regular file where directory expected: " + lockDir));
    }
    boost::filesystem::ofstream f;
    try {
        f.open(FileUtils::joinPath(lockDir, lockFile), std::ios::binary | std::ios::out);
    } catch (...) {
    }
    return f.is_open();
}

void SimpleFSLock::release() {
    String path(FileUtils::joinPath(lockDir, lockFile));
    if (FileUtils::fileExists(path) && !FileUtils::removeFile(path)) {
        boost::throw_exception(LockReleaseFailedException(L"failed to delete " + path));
    }
}

bool SimpleFSLock::isLocked() {
    return FileUtils::fileExists(FileUtils::joinPath(lockDir, lockFile));
}

String SimpleFSLock::toString() {
    return getClassName() + L"@" + FileUtils::joinPath(lockDir, lockFile);
}

}
