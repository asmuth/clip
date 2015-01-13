/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "ChecksumIndexInput.h"

namespace Lucene {

ChecksumIndexInput::ChecksumIndexInput(const IndexInputPtr& main) {
    this->main = main;
}

ChecksumIndexInput::~ChecksumIndexInput() {
}

uint8_t ChecksumIndexInput::readByte() {
    uint8_t b = main->readByte();
    checksum.process_byte(b);
    return b;
}

void ChecksumIndexInput::readBytes(uint8_t* b, int32_t offset, int32_t length) {
    main->readBytes(b, offset, length);
    checksum.process_bytes(b + offset, length);
}

int64_t ChecksumIndexInput::getChecksum() {
    return checksum.checksum();
}

void ChecksumIndexInput::close() {
    main->close();
}

int64_t ChecksumIndexInput::getFilePointer() {
    return main->getFilePointer();
}

void ChecksumIndexInput::seek(int64_t pos) {
    boost::throw_exception(RuntimeException(L"Seek not allowed"));
}

int64_t ChecksumIndexInput::length() {
    return main->length();
}

LuceneObjectPtr ChecksumIndexInput::clone(const LuceneObjectPtr& other) {
    LuceneObjectPtr clone = IndexInput::clone(other ? other : newLucene<ChecksumIndexInput>(main));
    ChecksumIndexInputPtr cloneIndexInput(boost::dynamic_pointer_cast<ChecksumIndexInput>(clone));
    cloneIndexInput->main = main;
    cloneIndexInput->checksum = checksum;
    return cloneIndexInput;
}

}
