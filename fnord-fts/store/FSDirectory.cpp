/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include <boost/filesystem/fstream.hpp>
#include "FSDirectory.h"
#include "NativeFSLockFactory.h"
#include "SimpleFSDirectory.h"
#include "BufferedIndexInput.h"
#include "LuceneThread.h"
#include "FileUtils.h"
#include "StringUtils.h"

extern "C"
{
#include "../util/md5/md5.h"
}

namespace Lucene {

/// Default read chunk size.  This is a conditional default based on operating system.
#ifdef LPP_BUILD_64
const int32_t FSDirectory::DEFAULT_READ_CHUNK_SIZE = INT_MAX;
#else
const int32_t FSDirectory::DEFAULT_READ_CHUNK_SIZE = 100 * 1024 * 1024; // 100mb
#endif

FSDirectory::FSDirectory(const String& path, const LockFactoryPtr& lockFactory) {
    checked = false;
    chunkSize = DEFAULT_READ_CHUNK_SIZE;

    LockFactoryPtr _lockFactory(lockFactory);

    // new ctors use always NativeFSLockFactory as default
    if (!_lockFactory) {
        _lockFactory = newLucene<NativeFSLockFactory>();
    }
    directory = path;

    if (FileUtils::fileExists(directory) && !FileUtils::isDirectory(directory)) {
        boost::throw_exception(NoSuchDirectoryException(L"File '" + directory + L"' exists but is not a directory"));
    }

    setLockFactory(_lockFactory);

    // for filesystem based LockFactory, delete the lockPrefix if the locks are placed
    // in index dir. if no index dir is given, set ourselves
    FSLockFactoryPtr lf(boost::dynamic_pointer_cast<FSLockFactory>(_lockFactory));

    if (lf) {
        if (lf->getLockDir().empty()) {
            lf->setLockDir(directory);
            lf->setLockPrefix(L"");
        } else if (lf->getLockDir() == directory) {
            lf->setLockPrefix(L"");
        }
    }
}

FSDirectory::~FSDirectory() {
}

FSDirectoryPtr FSDirectory::open(const String& path) {
    return open(path, LockFactoryPtr());
}

FSDirectoryPtr FSDirectory::open(const String& path, const LockFactoryPtr& lockFactory) {
    return newLucene<SimpleFSDirectory>(path, lockFactory);
}

void FSDirectory::createDir() {
    if (!checked) {
        if (!FileUtils::fileExists(directory) && !FileUtils::createDirectory(directory)) {
            boost::throw_exception(IOException(L"Cannot create directory: " + directory));
        }
        checked = true;
    }
}

void FSDirectory::initOutput(const String& name) {
    ensureOpen();
    createDir();
    String path(FileUtils::joinPath(directory, name));
    if (FileUtils::fileExists(path) && !FileUtils::removeFile(path)) { // delete existing, if any
        boost::throw_exception(IOException(L"Cannot overwrite: " + name));
    }
}

HashSet<String> FSDirectory::listAll(const String& dir) {
    if (!FileUtils::fileExists(dir)) {
        boost::throw_exception(NoSuchDirectoryException(L"Directory '" + dir + L"' does not exist"));
    } else if (!FileUtils::isDirectory(dir)) {
        boost::throw_exception(NoSuchDirectoryException(L"File '" + dir + L"' exists but is not a directory"));
    }

    HashSet<String> result(HashSet<String>::newInstance());

    // Exclude subdirs
    if (!FileUtils::listDirectory(dir, true, result)) {
        boost::throw_exception(IOException(L"Directory '" + dir + L"' exists and is a directory, but cannot be listed"));
    }

    return result;
}

HashSet<String> FSDirectory::listAll() {
    ensureOpen();
    return listAll(directory);
}

bool FSDirectory::fileExists(const String& name) {
    ensureOpen();
    return FileUtils::fileExists(FileUtils::joinPath(directory, name));
}

uint64_t FSDirectory::fileModified(const String& name) {
    ensureOpen();
    return FileUtils::fileModified(FileUtils::joinPath(directory, name));
}

uint64_t FSDirectory::fileModified(const String& directory, const String& name) {
    return FileUtils::fileModified(FileUtils::joinPath(directory, name));
}

void FSDirectory::touchFile(const String& name) {
    ensureOpen();
    FileUtils::touchFile(FileUtils::joinPath(directory, name));
}

void FSDirectory::deleteFile(const String& name) {
    ensureOpen();
    if (!FileUtils::removeFile(FileUtils::joinPath(directory, name))) {
        boost::throw_exception(IOException(L"Cannot delete: " + name));
    }
}

int64_t FSDirectory::fileLength(const String& name) {
    ensureOpen();
    return FileUtils::fileLength(FileUtils::joinPath(directory, name));
}

void FSDirectory::sync(const String& name) {
    ensureOpen();
    String path(FileUtils::joinPath(directory, name));
    bool success = false;

    for (int32_t retryCount = 0; retryCount < 5; ++retryCount) {
        boost::filesystem::ofstream syncFile;
        try {
            syncFile.open(path, std::ios::binary | std::ios::in | std::ios::out);
        } catch (...) {
        }

        if (syncFile.is_open()) {
            syncFile.close();
            success = true;
            break;
        }

        LuceneThread::threadSleep(5); // pause 5 msec
    }

    if (!success) {
        boost::throw_exception(IOException(L"Sync failure: " + path));
    }
}

IndexInputPtr FSDirectory::openInput(const String& name) {
    ensureOpen();
    return openInput(name, BufferedIndexInput::BUFFER_SIZE);
}

IndexInputPtr FSDirectory::openInput(const String& name, int32_t bufferSize) {
    return Directory::openInput(name, bufferSize);
}

String FSDirectory::getLockID() {
    ensureOpen();
    md5_state_t state;
    md5_byte_t digest[16];

    md5_init(&state);
    md5_append(&state, (const md5_byte_t*)StringUtils::toUTF8(directory).c_str(), directory.size());
    md5_finish(&state, digest);

    static const wchar_t* hexDigits = L"0123456789abcdef";

    String lockID(L"lucene-");
    for (int32_t i = 0; i < 16; ++i) {
        lockID += hexDigits[(digest[i] >> 4) & 0x0f];
        lockID += hexDigits[digest[i] & 0x0f];
    }

    return lockID;
}

void FSDirectory::close() {
    SyncLock syncLock(this);
    isOpen = false;
}

String FSDirectory::toString() {
    return getClassName() + L"@" + directory + L" lockFactory=" + getLockFactory()->toString();
}

String FSDirectory::getFile() {
    ensureOpen();
    return directory;
}

void FSDirectory::setReadChunkSize(int32_t chunkSize) {
#ifndef LPP_BUILD_64
    this->chunkSize = chunkSize;
#endif
}

int32_t FSDirectory::getReadChunkSize() {
    return chunkSize;
}

}
