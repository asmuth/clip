/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "CompoundFileWriter.h"
#include "SegmentMerger.h"
#include "Directory.h"
#include "IndexInput.h"
#include "IndexOutput.h"
#include "StringUtils.h"

namespace Lucene {

CompoundFileWriter::CompoundFileWriter(const DirectoryPtr& dir, const String& name, const CheckAbortPtr& checkAbort) {
    if (!dir) {
        boost::throw_exception(IllegalArgumentException(L"directory cannot be empty"));
    }
    if (name.empty()) {
        boost::throw_exception(IllegalArgumentException(L"name cannot be empty"));
    }
    this->checkAbort = checkAbort;
    _directory = dir;
    fileName = name;
    ids = HashSet<String>::newInstance();
    entries = Collection<FileEntry>::newInstance();
    merged = false;
}

CompoundFileWriter::~CompoundFileWriter() {
}

DirectoryPtr CompoundFileWriter::getDirectory() {
    return DirectoryPtr(_directory);
}

String CompoundFileWriter::getName() {
    return fileName;
}

void CompoundFileWriter::addFile(const String& file) {
    if (merged) {
        boost::throw_exception(IllegalStateException(L"Can't add extensions after merge has been called"));
    }

    if (file.empty()) {
        boost::throw_exception(IllegalArgumentException(L"file cannot be empty"));
    }

    if (!ids.add(file)) {
        boost::throw_exception(IllegalArgumentException(L"File " + file + L" already added"));
    }

    FileEntry entry;
    entry.file = file;
    entries.add(entry);
}

void CompoundFileWriter::close() {
    if (merged) {
        boost::throw_exception(IllegalStateException(L"Merge already performed"));
    }

    if (entries.empty()) {
        boost::throw_exception(IllegalStateException(L"No entries to merge have been defined"));
    }

    merged = true;

    DirectoryPtr directory(_directory);

    // open the compound stream
    IndexOutputPtr os;
    LuceneException finally;
    try {
        os = directory->createOutput(fileName);

        // Write the number of entries
        os->writeVInt(entries.size());

        // Write the directory with all offsets at 0. Remember the positions of directory entries so that we
        // can adjust the offsets later
        int64_t totalSize = 0;
        for (Collection<FileEntry>::iterator fe = entries.begin(); fe != entries.end(); ++fe) {
            fe->directoryOffset = os->getFilePointer();
            os->writeLong(0); // for now
            os->writeString(fe->file);
            totalSize += directory->fileLength(fe->file);
        }

        // Pre-allocate size of file as optimization - this can potentially help IO performance as we write the
        // file and also later during searching.  It also uncovers a disk-full situation earlier and hopefully
        // without actually filling disk to 100%
        int64_t finalLength = totalSize + os->getFilePointer();
        os->setLength(finalLength);

        // Open the files and copy their data into the stream. Remember the locations of each file's data section.
        ByteArray buffer(ByteArray::newInstance(16384));
        for (Collection<FileEntry>::iterator fe = entries.begin(); fe != entries.end(); ++fe) {
            fe->dataOffset = os->getFilePointer();
            copyFile(*fe, os, buffer);
        }

        // Write the data offsets into the directory of the compound stream
        for (Collection<FileEntry>::iterator fe = entries.begin(); fe != entries.end(); ++fe) {
            os->seek(fe->directoryOffset);
            os->writeLong(fe->dataOffset);
        }

        BOOST_ASSERT(finalLength == os->length());

        // Close the output stream. Set the os to null before trying to close so that if an exception occurs during
        // the close, the finally clause below will not attempt to close the stream the second time.
        IndexOutputPtr tmp(os);
        os.reset();
        tmp->close();
    } catch (LuceneException& e) {
        finally = e;
    }

    if (os) {
        try {
            os->close();
        } catch (LuceneException&) {
        }
    }
    finally.throwException();
}

void CompoundFileWriter::copyFile(const FileEntry& source, const IndexOutputPtr& os, ByteArray buffer) {
    IndexInputPtr is;
    DirectoryPtr directory(_directory);
    LuceneException finally;
    try {
        int64_t startPtr = os->getFilePointer();

        is = directory->openInput(source.file);
        int64_t length = is->length();
        int64_t remainder = length;
        int64_t chunk = buffer.size();

        while (remainder > 0) {
            int32_t len = (int32_t)std::min(chunk, remainder);
            is->readBytes(buffer.get(), 0, len, false);
            os->writeBytes(buffer.get(), len);
            remainder -= len;
            if (checkAbort) {
                // Roughly every 2 MB we will check if it's time to abort
                checkAbort->work(80);
            }
        }

        // Verify that remainder is 0
        if (remainder != 0) {
            boost::throw_exception(IOException(L"Non-zero remainder length after copying: " + StringUtils::toString(remainder) +
                                               L" (id: " + source.file + L", length: " + StringUtils::toString(length) +
                                               L", buffer size: " + StringUtils::toString(chunk) + L")"));
        }

        // Verify that the output length diff is equal to original file
        int64_t endPtr = os->getFilePointer();
        int64_t diff = endPtr - startPtr;
        if (diff != length) {
            boost::throw_exception(IOException(L"Difference in the output file offsets " + StringUtils::toString(diff) +
                                               L" does not match the original file length " + StringUtils::toString(length)));
        }
    } catch (LuceneException& e) {
        finally = e;
    }

    if (is) {
        is->close();
    }
    finally.throwException();
}

}
