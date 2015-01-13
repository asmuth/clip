/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "RAMFile.h"
#include "RAMDirectory.h"
#include "MiscUtils.h"

namespace Lucene {

RAMFile::RAMFile() {
    this->buffers = Collection<ByteArray>::newInstance();
    this->length = 0;
    this->sizeInBytes = 0;
    this->lastModified = MiscUtils::currentTimeMillis();
}

RAMFile::RAMFile(const RAMDirectoryPtr& directory) {
    this->buffers = Collection<ByteArray>::newInstance();
    this->length = 0;
    this->sizeInBytes = 0;
    this->_directory = directory;
    this->lastModified = MiscUtils::currentTimeMillis();
}

RAMFile::~RAMFile() {
}

int64_t RAMFile::getLength() {
    SyncLock syncLock(this);
    return length;
}

void RAMFile::setLength(int64_t length) {
    SyncLock syncLock(this);
    this->length = length;
}

int64_t RAMFile::getLastModified() {
    SyncLock syncLock(this);
    return lastModified;
}

void RAMFile::setLastModified(int64_t lastModified) {
    SyncLock syncLock(this);
    this->lastModified = lastModified;
}

ByteArray RAMFile::addBuffer(int32_t size) {
    ByteArray buffer(newBuffer(size));
    {
        SyncLock syncLock(this);
        buffers.add(buffer);
        sizeInBytes += size;
    }

    RAMDirectoryPtr directory(_directory.lock());
    if (directory) {
        SyncLock dirLock(directory);
        directory->_sizeInBytes += size;
    }
    return buffer;
}

ByteArray RAMFile::getBuffer(int32_t index) {
    SyncLock syncLock(this);
    return buffers[index];
}

int32_t RAMFile::numBuffers() {
    SyncLock syncLock(this);
    return buffers.size();
}

ByteArray RAMFile::newBuffer(int32_t size) {
    return ByteArray::newInstance(size);
}

int64_t RAMFile::getSizeInBytes() {
    SyncLock syncLock(this);
    return sizeInBytes;
}

}
