/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "Directory.h"
#include "LockFactory.h"
#include "BufferedIndexOutput.h"
#include "IndexFileNameFilter.h"
#include "IndexInput.h"
#include "IndexOutput.h"

namespace Lucene {

Directory::Directory() {
    isOpen = true;
}

Directory::~Directory() {
}

void Directory::close() {
    // override
}

void Directory::sync(const String& name) {
}

IndexInputPtr Directory::openInput(const String& name, int32_t bufferSize) {
    return openInput(name);
}

LockPtr Directory::makeLock(const String& name) {
    return lockFactory->makeLock(name);
}

void Directory::clearLock(const String& name) {
    if (lockFactory) {
        lockFactory->clearLock(name);
    }
}

void Directory::setLockFactory(const LockFactoryPtr& lockFactory) {
    BOOST_ASSERT(lockFactory);
    this->lockFactory = lockFactory;
    this->lockFactory->setLockPrefix(getLockID());
}

LockFactoryPtr Directory::getLockFactory() {
    return lockFactory;
}

String Directory::getLockID() {
    return toString();
}

String Directory::toString() {
    return LuceneObject::toString() + L" lockFactory=" + getLockFactory()->toString();
}

void Directory::copy(const DirectoryPtr& src, const DirectoryPtr& dest, bool closeDirSrc) {
    HashSet<String> files(src->listAll());

    ByteArray buf(ByteArray::newInstance(BufferedIndexOutput::BUFFER_SIZE));

    for (HashSet<String>::iterator file = files.begin(); file != files.end(); ++file) {
        if (!IndexFileNameFilter::accept(L"", *file)) {
            continue;
        }

        IndexOutputPtr os;
        IndexInputPtr is;

        LuceneException finally;
        try {
            // create file in dest directory
            os = dest->createOutput(*file);
            // read current file
            is = src->openInput(*file);
            // and copy to dest directory
            int64_t len = is->length();
            int64_t readCount = 0;
            while (readCount < len) {
                int32_t toRead = readCount + BufferedIndexOutput::BUFFER_SIZE > len ? (int32_t)(len - readCount) : BufferedIndexOutput::BUFFER_SIZE;
                is->readBytes(buf.get(), 0, toRead);
                os->writeBytes(buf.get(), toRead);
                readCount += toRead;
            }
        } catch (LuceneException& e) {
            finally = e;
        }
        // graceful cleanup
        try {
            if (os) {
                os->close();
            }
        } catch (...) {
        }
        try {
            if (is) {
                is->close();
            }
        } catch (...) {
        }
        finally.throwException();
    }
    if (closeDirSrc) {
        src->close();
    }
}

void Directory::ensureOpen() {
    if (!isOpen) {
        boost::throw_exception(AlreadyClosedException(L"This directory is closed"));
    }
}

}
