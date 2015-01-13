/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include <boost/filesystem/fstream.hpp>
#include "FileReader.h"
#include "MiscUtils.h"
#include "FileUtils.h"
#include "StringUtils.h"

namespace Lucene {

const int32_t FileReader::FILE_EOF = Reader::READER_EOF;
const int32_t FileReader::FILE_ERROR = -1;

FileReader::FileReader(const String& fileName) {
    this->file = newInstance<boost::filesystem::ifstream>(fileName, std::ios::binary | std::ios::in);
    if (!file->is_open()) {
        boost::throw_exception(FileNotFoundException(fileName));
    }
    _length = FileUtils::fileLength(fileName);
}

FileReader::~FileReader() {
}

int32_t FileReader::read() {
    wchar_t buffer;
    return read(&buffer, 0, 1) == FILE_EOF ? FILE_EOF : buffer;
}

int32_t FileReader::read(wchar_t* buffer, int32_t offset, int32_t length) {
    try {
        if (file->eof()) {
            return FILE_EOF;
        }
        if (!fileBuffer) {
            fileBuffer = ByteArray::newInstance(length);
        }
        if (length > fileBuffer.size()) {
            fileBuffer.resize(length);
        }
        file->read((char*)fileBuffer.get(), length);
        int32_t readLength = file->gcount();
        MiscUtils::arrayCopy(fileBuffer.get(), 0, buffer, offset, readLength);
        return readLength == 0 ? FILE_EOF : readLength;
    } catch (...) {
        return FILE_ERROR;
    }
}

void FileReader::close() {
    file->close();
}

bool FileReader::markSupported() {
    return false;
}

void FileReader::reset() {
    file->clear();
    file->seekg((std::streamoff)0);
}

int64_t FileReader::length() {
    return _length;
}

}
