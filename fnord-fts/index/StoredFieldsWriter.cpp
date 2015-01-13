/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "StoredFieldsWriter.h"
#include "StoredFieldsWriterPerThread.h"
#include "RAMOutputStream.h"
#include "SegmentWriteState.h"
#include "FieldsWriter.h"
#include "IndexFileNames.h"
#include "IndexWriter.h"
#include "Directory.h"
#include "MiscUtils.h"
#include "StringUtils.h"

namespace Lucene {

StoredFieldsWriter::StoredFieldsWriter(const DocumentsWriterPtr& docWriter, const FieldInfosPtr& fieldInfos) {
    lastDocID = 0;
    docFreeList = Collection<StoredFieldsWriterPerDocPtr>::newInstance(1);
    freeCount = 0;
    allocCount = 0;

    this->_docWriter = docWriter;
    this->fieldInfos = fieldInfos;
}

StoredFieldsWriter::~StoredFieldsWriter() {
}

StoredFieldsWriterPerThreadPtr StoredFieldsWriter::addThread(const DocStatePtr& docState) {
    return newLucene<StoredFieldsWriterPerThread>(docState, shared_from_this());
}

void StoredFieldsWriter::flush(const SegmentWriteStatePtr& state) {
    SyncLock syncLock(this);
    if (state->numDocsInStore > 0) {
        // It's possible that all documents seen in this segment hit non-aborting exceptions,
        // in which case we will not have yet init'd the FieldsWriter
        initFieldsWriter();

        // Fill fdx file to include any final docs that we skipped because they hit non-aborting
        // exceptions
        fill(state->numDocsInStore - DocumentsWriterPtr(_docWriter)->getDocStoreOffset());
    }

    if (fieldsWriter) {
        fieldsWriter->flush();
    }
}

void StoredFieldsWriter::initFieldsWriter() {
    if (!fieldsWriter) {
        DocumentsWriterPtr docWriter(_docWriter);
        String docStoreSegment(docWriter->getDocStoreSegment());
        if (!docStoreSegment.empty()) {
            fieldsWriter = newLucene<FieldsWriter>(docWriter->directory, docStoreSegment, fieldInfos);
            docWriter->addOpenFile(docStoreSegment + L"." + IndexFileNames::FIELDS_EXTENSION());
            docWriter->addOpenFile(docStoreSegment + L"." + IndexFileNames::FIELDS_INDEX_EXTENSION());
            lastDocID = 0;
        }
    }
}

void StoredFieldsWriter::closeDocStore(const SegmentWriteStatePtr& state) {
    SyncLock syncLock(this);
    int32_t inc = state->numDocsInStore - lastDocID;
    if (inc > 0) {
        initFieldsWriter();
        fill(state->numDocsInStore - DocumentsWriterPtr(_docWriter)->getDocStoreOffset());
    }

    if (fieldsWriter) {
        fieldsWriter->close();
        fieldsWriter.reset();
        lastDocID = 0;
        BOOST_ASSERT(!state->docStoreSegmentName.empty());
        state->flushedFiles.add(state->docStoreSegmentName + L"." + IndexFileNames::FIELDS_EXTENSION());
        state->flushedFiles.add(state->docStoreSegmentName + L"." + IndexFileNames::FIELDS_INDEX_EXTENSION());

        DocumentsWriterPtr docWriter(state->_docWriter);
        docWriter->removeOpenFile(state->docStoreSegmentName + L"." + IndexFileNames::FIELDS_EXTENSION());
        docWriter->removeOpenFile(state->docStoreSegmentName + L"." + IndexFileNames::FIELDS_INDEX_EXTENSION());

        String fileName(state->docStoreSegmentName + L"." + IndexFileNames::FIELDS_INDEX_EXTENSION());

        if (4 + ((int64_t)state->numDocsInStore) * 8 != state->directory->fileLength(fileName)) {
            boost::throw_exception(RuntimeException(L"after flush: fdx size mismatch: " + StringUtils::toString(state->numDocsInStore) +
                                                    L" docs vs " + StringUtils::toString(state->directory->fileLength(fileName)) +
                                                    L" length in bytes of " + fileName + L" file exists?=" +
                                                    StringUtils::toString(state->directory->fileExists(fileName))));
        }
    }
}

StoredFieldsWriterPerDocPtr StoredFieldsWriter::getPerDoc() {
    SyncLock syncLock(this);
    if (freeCount == 0) {
        ++allocCount;
        if (allocCount > docFreeList.size()) {
            // Grow our free list up front to make sure we have enough space to recycle all
            // outstanding StoredFieldsWriterPerDoc instances
            BOOST_ASSERT(allocCount == docFreeList.size() + 1);
            docFreeList.resize(MiscUtils::getNextSize(allocCount));
        }
        return newLucene<StoredFieldsWriterPerDoc>(shared_from_this());
    } else {
        return docFreeList[--freeCount];
    }
}

void StoredFieldsWriter::abort() {
    SyncLock syncLock(this);
    if (fieldsWriter) {
        try {
            fieldsWriter->close();
        } catch (...) {
        }
        fieldsWriter.reset();
        lastDocID = 0;
    }
}

void StoredFieldsWriter::fill(int32_t docID) {
    int32_t docStoreOffset = DocumentsWriterPtr(_docWriter)->getDocStoreOffset();

    // We must "catch up" for all docs before us that had no stored fields
    int32_t end = docID + docStoreOffset;
    while (lastDocID < end) {
        fieldsWriter->skipDocument();
        ++lastDocID;
    }
}

void StoredFieldsWriter::finishDocument(const StoredFieldsWriterPerDocPtr& perDoc) {
    SyncLock syncLock(this);
    IndexWriterPtr writer(DocumentsWriterPtr(_docWriter)->_writer);
    BOOST_ASSERT(writer->testPoint(L"StoredFieldsWriter.finishDocument start"));
    initFieldsWriter();

    fill(perDoc->docID);

    // Append stored fields to the real FieldsWriter
    fieldsWriter->flushDocument(perDoc->numStoredFields, perDoc->fdt);
    ++lastDocID;
    perDoc->reset();
    free(perDoc);
    BOOST_ASSERT(writer->testPoint(L"StoredFieldsWriter.finishDocument end"));
}

bool StoredFieldsWriter::freeRAM() {
    return false;
}

void StoredFieldsWriter::free(const StoredFieldsWriterPerDocPtr& perDoc) {
    SyncLock syncLock(this);
    BOOST_ASSERT(freeCount < docFreeList.size());
    BOOST_ASSERT(perDoc->numStoredFields == 0);
    BOOST_ASSERT(perDoc->fdt->length() == 0);
    BOOST_ASSERT(perDoc->fdt->getFilePointer() == 0);
    docFreeList[freeCount++] = perDoc;
}

StoredFieldsWriterPerDoc::StoredFieldsWriterPerDoc(const StoredFieldsWriterPtr& fieldsWriter) {
    this->_fieldsWriter = fieldsWriter;
    buffer = DocumentsWriterPtr(fieldsWriter->_docWriter)->newPerDocBuffer();
    fdt = newLucene<RAMOutputStream>(buffer);
    numStoredFields = 0;
}

StoredFieldsWriterPerDoc::~StoredFieldsWriterPerDoc() {
}

void StoredFieldsWriterPerDoc::reset() {
    fdt->reset();
    buffer->recycle();
    numStoredFields = 0;
}

void StoredFieldsWriterPerDoc::abort() {
    reset();
    StoredFieldsWriterPtr(_fieldsWriter)->free(shared_from_this());
}

int64_t StoredFieldsWriterPerDoc::sizeInBytes() {
    return buffer->getSizeInBytes();
}

void StoredFieldsWriterPerDoc::finish() {
    StoredFieldsWriterPtr(_fieldsWriter)->finishDocument(shared_from_this());
}

}
