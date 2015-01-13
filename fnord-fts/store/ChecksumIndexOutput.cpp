/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "ChecksumIndexOutput.h"

namespace Lucene {

ChecksumIndexOutput::ChecksumIndexOutput(const IndexOutputPtr& main) {
    this->main = main;
}

ChecksumIndexOutput::~ChecksumIndexOutput() {
}

void ChecksumIndexOutput::writeByte(uint8_t b) {
    checksum.process_byte(b);
    main->writeByte(b);
}

void ChecksumIndexOutput::writeBytes(const uint8_t* b, int32_t offset, int32_t length) {
    checksum.process_bytes(b + offset, length);
    main->writeBytes(b, offset, length);
}

int64_t ChecksumIndexOutput::getChecksum() {
    return checksum.checksum();
}

void ChecksumIndexOutput::flush() {
    main->flush();
}

void ChecksumIndexOutput::close() {
    main->close();
}

int64_t ChecksumIndexOutput::getFilePointer() {
    return main->getFilePointer();
}

void ChecksumIndexOutput::seek(int64_t pos) {
    boost::throw_exception(RuntimeException(L"Seek not allowed"));
}

void ChecksumIndexOutput::prepareCommit() {
    int64_t checksum = getChecksum();

    // Intentionally write a mismatched checksum.  This is because we want to 1) test, as best we can, that we
    // are able to write a long to the file, but 2) not actually "commit" the file yet.  This (prepare commit)
    // is phase 1 of a two-phase commit.
    int64_t pos = main->getFilePointer();
    main->writeLong(checksum - 1);
    main->flush();
    main->seek(pos);
}

void ChecksumIndexOutput::finishCommit() {
    main->writeLong(getChecksum());
}

int64_t ChecksumIndexOutput::length() {
    return main->length();
}

}
