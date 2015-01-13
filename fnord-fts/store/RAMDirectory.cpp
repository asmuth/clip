/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "RAMDirectory.h"
#include "RAMFile.h"
#include "RAMInputStream.h"
#include "RAMOutputStream.h"
#include "SingleInstanceLockFactory.h"
#include "LuceneThread.h"
#include "MiscUtils.h"

namespace Lucene {

RAMDirectory::RAMDirectory() {
    this->fileMap = MapStringRAMFile::newInstance();
    this->_sizeInBytes = 0;
    this->copyDirectory = false;
    this->closeDir = false;
    setLockFactory(newLucene<SingleInstanceLockFactory>());
}

RAMDirectory::RAMDirectory(const DirectoryPtr& dir) {
    this->fileMap = MapStringRAMFile::newInstance();
    this->_sizeInBytes = 0;
    this->copyDirectory = true;
    this->_dirSource = dir;
    this->closeDir = false;
    setLockFactory(newLucene<SingleInstanceLockFactory>());
}

RAMDirectory::RAMDirectory(const DirectoryPtr& dir, bool closeDir) {
    this->fileMap = MapStringRAMFile::newInstance();
    this->_sizeInBytes = 0;
    this->copyDirectory = true;
    this->_dirSource = dir;
    this->closeDir = closeDir;
    setLockFactory(newLucene<SingleInstanceLockFactory>());
}

RAMDirectory::~RAMDirectory() {
}

void RAMDirectory::initialize() {
    if (copyDirectory) {
        Directory::copy(DirectoryPtr(_dirSource), shared_from_this(), closeDir);
    }
}

HashSet<String> RAMDirectory::listAll() {
    SyncLock syncLock(this);
    ensureOpen();
    HashSet<String> result(HashSet<String>::newInstance());
    for (MapStringRAMFile::iterator fileName = fileMap.begin(); fileName != fileMap.end(); ++fileName) {
        result.add(fileName->first);
    }
    return result;
}

bool RAMDirectory::fileExists(const String& name) {
    ensureOpen();
    SyncLock syncLock(this);
    return fileMap.contains(name);
}

uint64_t RAMDirectory::fileModified(const String& name) {
    ensureOpen();
    SyncLock syncLock(this);
    MapStringRAMFile::iterator ramFile = fileMap.find(name);
    if (ramFile == fileMap.end()) {
        boost::throw_exception(FileNotFoundException(name));
    }
    return ramFile->second->getLastModified();
}

void RAMDirectory::touchFile(const String& name) {
    ensureOpen();
    RAMFilePtr file;
    {
        SyncLock syncLock(this);
        MapStringRAMFile::iterator ramFile = fileMap.find(name);
        if (ramFile == fileMap.end()) {
            boost::throw_exception(FileNotFoundException(name));
        }
        file = ramFile->second;
    }
    int64_t ts1 = MiscUtils::currentTimeMillis();
    while (ts1 == MiscUtils::currentTimeMillis()) {
        LuceneThread::threadSleep(1);
    }
    file->setLastModified(MiscUtils::currentTimeMillis());
}

int64_t RAMDirectory::fileLength(const String& name) {
    ensureOpen();
    SyncLock syncLock(this);
    MapStringRAMFile::iterator ramFile = fileMap.find(name);
    if (ramFile == fileMap.end()) {
        boost::throw_exception(FileNotFoundException(name));
    }
    return ramFile->second->getLength();
}

int64_t RAMDirectory::sizeInBytes() {
    SyncLock syncLock(this);
    ensureOpen();
    return _sizeInBytes;
}

void RAMDirectory::deleteFile(const String& name) {
    SyncLock syncLock(this);
    ensureOpen();
    MapStringRAMFile::iterator ramFile = fileMap.find(name);
    if (ramFile == fileMap.end()) {
        boost::throw_exception(FileNotFoundException(name));
    }
    _sizeInBytes -= ramFile->second->getSizeInBytes();
    fileMap.remove(name);
}

IndexOutputPtr RAMDirectory::createOutput(const String& name) {
    ensureOpen();
    RAMFilePtr file(newLucene<RAMFile>(shared_from_this()));
    {
        SyncLock syncLock(this);
        MapStringRAMFile::iterator existing = fileMap.find(name);
        if (existing != fileMap.end()) {
            _sizeInBytes -= existing->second->getSizeInBytes();
            existing->second->_directory.reset();
        }
        fileMap.put(name, file);
    }
    return newLucene<RAMOutputStream>(file);
}

IndexInputPtr RAMDirectory::openInput(const String& name) {
    ensureOpen();
    RAMFilePtr file;
    {
        SyncLock syncLock(this);
        MapStringRAMFile::iterator ramFile = fileMap.find(name);
        if (ramFile == fileMap.end()) {
            boost::throw_exception(FileNotFoundException(name));
        }
        file = ramFile->second;
    }
    return newLucene<RAMInputStream>(file);
}

void RAMDirectory::close() {
    isOpen = false;
    fileMap.reset();
}

}
