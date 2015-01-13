/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "MMapDirectory.h"
#include "_MMapDirectory.h"
#include "SimpleFSDirectory.h"
#include "_SimpleFSDirectory.h"
#include "MiscUtils.h"
#include "FileUtils.h"
#include "StringUtils.h"

namespace Lucene {

MMapDirectory::MMapDirectory(const String& path, const LockFactoryPtr& lockFactory) : FSDirectory(path, lockFactory) {
}

MMapDirectory::~MMapDirectory() {
}

IndexInputPtr MMapDirectory::openInput(const String& name, int32_t bufferSize) {
    ensureOpen();
    return newLucene<MMapIndexInput>(FileUtils::joinPath(directory, name));
}

IndexOutputPtr MMapDirectory::createOutput(const String& name) {
    initOutput(name);
    return newLucene<SimpleFSIndexOutput>(FileUtils::joinPath(directory, name));
}

MMapIndexInput::MMapIndexInput(const String& path) {
    _length = path.empty() ? 0 : (int32_t)FileUtils::fileLength(path);
    bufferPosition = 0;
    if (!path.empty()) {
        try {
            file.open(boost::filesystem::wpath(path), _length);
        } catch (...) {
            boost::throw_exception(FileNotFoundException(path));
        }
    }
    isClone = false;
}

MMapIndexInput::~MMapIndexInput() {
}

uint8_t MMapIndexInput::readByte() {
    try {
        return file.data()[bufferPosition++];
    } catch (...) {
        boost::throw_exception(IOException(L"Read past EOF"));
        return 0;
    }
}

void MMapIndexInput::readBytes(uint8_t* b, int32_t offset, int32_t length) {
    try {
        MiscUtils::arrayCopy(file.data(), bufferPosition, b, offset, length);
        bufferPosition += length;
    } catch (...) {
        boost::throw_exception(IOException(L"Read past EOF"));
    }
}

int64_t MMapIndexInput::getFilePointer() {
    return bufferPosition;
}

void MMapIndexInput::seek(int64_t pos) {
    bufferPosition = (int32_t)pos;
}

int64_t MMapIndexInput::length() {
    return (int64_t)_length;
}

void MMapIndexInput::close() {
    if (isClone || !file.is_open()) {
        return;
    }
    _length = 0;
    bufferPosition = 0;
    file.close();
}

LuceneObjectPtr MMapIndexInput::clone(const LuceneObjectPtr& other) {
    if (!file.is_open()) {
        boost::throw_exception(AlreadyClosedException(L"MMapIndexInput already closed"));
    }
    LuceneObjectPtr clone = IndexInput::clone(other ? other : newLucene<MMapIndexInput>());
    MMapIndexInputPtr cloneIndexInput(boost::dynamic_pointer_cast<MMapIndexInput>(clone));
    cloneIndexInput->_length = _length;
    cloneIndexInput->file = file;
    cloneIndexInput->bufferPosition = bufferPosition;
    cloneIndexInput->isClone = true;
    return cloneIndexInput;
}

}
