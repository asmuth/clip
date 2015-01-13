/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "TermVectorsTermsWriter.h"
#include "TermVectorsTermsWriterPerThread.h"
#include "TermVectorsTermsWriterPerField.h"
#include "TermVectorsReader.h"
#include "TermsHashPerField.h"
#include "TermsHashPerThread.h"
#include "RAMOutputStream.h"
#include "IndexWriter.h"
#include "IndexFileNames.h"
#include "SegmentWriteState.h"
#include "Directory.h"
#include "MiscUtils.h"
#include "StringUtils.h"

namespace Lucene {

TermVectorsTermsWriter::TermVectorsTermsWriter(const DocumentsWriterPtr& docWriter) {
    this->freeCount = 0;
    this->lastDocID = 0;
    this->allocCount = 0;
    this->_docWriter = docWriter;
    this->docFreeList = Collection<TermVectorsTermsWriterPerDocPtr>::newInstance(1);
}

TermVectorsTermsWriter::~TermVectorsTermsWriter() {
}

TermsHashConsumerPerThreadPtr TermVectorsTermsWriter::addThread(const TermsHashPerThreadPtr& perThread) {
    return newLucene<TermVectorsTermsWriterPerThread>(perThread, shared_from_this());
}

void TermVectorsTermsWriter::createPostings(Collection<RawPostingListPtr> postings, int32_t start, int32_t count) {
    int32_t end = start + count;
    for (int32_t i = start; i < end; ++i) {
        postings[i] = newLucene<TermVectorsTermsWriterPostingList>();
    }
}

void TermVectorsTermsWriter::flush(MapTermsHashConsumerPerThreadCollectionTermsHashConsumerPerField threadsAndFields, const SegmentWriteStatePtr& state) {
    SyncLock syncLock(this);

    // NOTE: it's possible that all documents seen in this segment hit non-aborting exceptions, in which case we will
    // not have yet init'd the TermVectorsWriter.  This is actually OK (unlike in the stored fields case) because,
    // although IieldInfos.hasVectors() will return true, the TermVectorsReader gracefully handles non-existence of
    // the term vectors files.
    if (tvx) {
        if (state->numDocsInStore > 0) {
            // In case there are some final documents that we didn't see (because they hit a non-aborting exception)
            fill(state->numDocsInStore - DocumentsWriterPtr(_docWriter)->getDocStoreOffset());
        }

        tvx->flush();
        tvd->flush();
        tvf->flush();
    }

    for (MapTermsHashConsumerPerThreadCollectionTermsHashConsumerPerField::iterator entry = threadsAndFields.begin(); entry != threadsAndFields.end(); ++entry) {
        for (Collection<TermsHashConsumerPerFieldPtr>::iterator field = entry->second.begin(); field != entry->second.end(); ++field) {
            TermVectorsTermsWriterPerFieldPtr perField(boost::static_pointer_cast<TermVectorsTermsWriterPerField>(*field));
            TermsHashPerFieldPtr(perField->_termsHashPerField)->reset();
            perField->shrinkHash();
        }

        TermVectorsTermsWriterPerThreadPtr perThread(boost::static_pointer_cast<TermVectorsTermsWriterPerThread>(entry->first));
        TermsHashPerThreadPtr(perThread->_termsHashPerThread)->reset(true);
    }
}

void TermVectorsTermsWriter::closeDocStore(const SegmentWriteStatePtr& state) {
    SyncLock syncLock(this);
    if (tvx) {
        DocumentsWriterPtr docWriter(_docWriter);

        // At least one doc in this run had term vectors enabled
        fill(state->numDocsInStore - docWriter->getDocStoreOffset());
        tvx->close();
        tvf->close();
        tvd->close();
        tvx.reset();
        BOOST_ASSERT(!state->docStoreSegmentName.empty());
        String fileName(state->docStoreSegmentName + L"." + IndexFileNames::VECTORS_INDEX_EXTENSION());
        if (4 + ((int64_t)state->numDocsInStore) * 16 != state->directory->fileLength(fileName)) {
            boost::throw_exception(RuntimeException(L"after flush: tvx size mismatch: " + StringUtils::toString(state->numDocsInStore) +
                                                    L" docs vs " + StringUtils::toString(state->directory->fileLength(fileName)) +
                                                    L" length in bytes of " + fileName + L" file exists?=" +
                                                    StringUtils::toString(state->directory->fileExists(fileName))));
        }

        state->flushedFiles.add(state->docStoreSegmentName + L"." + IndexFileNames::VECTORS_INDEX_EXTENSION());
        state->flushedFiles.add(state->docStoreSegmentName + L"." + IndexFileNames::VECTORS_FIELDS_EXTENSION());
        state->flushedFiles.add(state->docStoreSegmentName + L"." + IndexFileNames::VECTORS_DOCUMENTS_EXTENSION());

        docWriter->removeOpenFile(state->docStoreSegmentName + L"." + IndexFileNames::VECTORS_INDEX_EXTENSION());
        docWriter->removeOpenFile(state->docStoreSegmentName + L"." + IndexFileNames::VECTORS_FIELDS_EXTENSION());
        docWriter->removeOpenFile(state->docStoreSegmentName + L"." + IndexFileNames::VECTORS_DOCUMENTS_EXTENSION());

        lastDocID = 0;
    }
}

TermVectorsTermsWriterPerDocPtr TermVectorsTermsWriter::getPerDoc() {
    SyncLock syncLock(this);
    if (freeCount == 0) {
        if (++allocCount > docFreeList.size()) {
            // Grow our free list up front to make sure we have enough space to recycle all outstanding
            // PerDoc instances
            BOOST_ASSERT(allocCount == 1 + docFreeList.size());
            docFreeList.resize(MiscUtils::getNextSize(allocCount));
        }
        return newLucene<TermVectorsTermsWriterPerDoc>(shared_from_this());
    } else {
        return docFreeList[--freeCount];
    }
}

void TermVectorsTermsWriter::fill(int32_t docID) {
    int32_t docStoreOffset = DocumentsWriterPtr(_docWriter)->getDocStoreOffset();
    int32_t end = docID + docStoreOffset;
    if (lastDocID < end) {
        int64_t tvfPosition = tvf->getFilePointer();
        while (lastDocID < end) {
            tvx->writeLong(tvd->getFilePointer());
            tvd->writeVInt(0);
            tvx->writeLong(tvfPosition);
            ++lastDocID;
        }
    }
}

void TermVectorsTermsWriter::initTermVectorsWriter() {
    SyncLock syncLock(this);
    if (!tvx) {
        DocumentsWriterPtr docWriter(_docWriter);

        String docStoreSegment(docWriter->getDocStoreSegment());
        if (docStoreSegment.empty()) {
            return;
        }

        // If we hit an exception while init'ing the term vector output files, we must abort this segment
        // because those files will be in an unknown state
        tvx = docWriter->directory->createOutput(docStoreSegment + L"." + IndexFileNames::VECTORS_INDEX_EXTENSION());
        tvd = docWriter->directory->createOutput(docStoreSegment + L"." + IndexFileNames::VECTORS_DOCUMENTS_EXTENSION());
        tvf = docWriter->directory->createOutput(docStoreSegment + L"." + IndexFileNames::VECTORS_FIELDS_EXTENSION());

        tvx->writeInt(TermVectorsReader::FORMAT_CURRENT);
        tvd->writeInt(TermVectorsReader::FORMAT_CURRENT);
        tvf->writeInt(TermVectorsReader::FORMAT_CURRENT);

        docWriter->addOpenFile(docStoreSegment + L"." + IndexFileNames::VECTORS_INDEX_EXTENSION());
        docWriter->addOpenFile(docStoreSegment + L"." + IndexFileNames::VECTORS_FIELDS_EXTENSION());
        docWriter->addOpenFile(docStoreSegment + L"." + IndexFileNames::VECTORS_DOCUMENTS_EXTENSION());

        lastDocID = 0;
    }
}

void TermVectorsTermsWriter::finishDocument(const TermVectorsTermsWriterPerDocPtr& perDoc) {
    SyncLock syncLock(this);
    DocumentsWriterPtr docWriter(_docWriter);

    BOOST_ASSERT(IndexWriterPtr(docWriter->_writer)->testPoint(L"TermVectorsTermsWriter.finishDocument start"));

    initTermVectorsWriter();

    fill(perDoc->docID);

    // Append term vectors to the real outputs
    tvx->writeLong(tvd->getFilePointer());
    tvx->writeLong(tvf->getFilePointer());
    tvd->writeVInt(perDoc->numVectorFields);

    if (perDoc->numVectorFields > 0) {
        for (int32_t i = 0; i < perDoc->numVectorFields; ++i) {
            tvd->writeVInt(perDoc->fieldNumbers[i]);
        }
        BOOST_ASSERT(perDoc->fieldPointers[0] == 0);
        int64_t lastPos = perDoc->fieldPointers[0];
        for (int32_t i = 1; i < perDoc->numVectorFields; ++i) {
            int64_t pos = perDoc->fieldPointers[i];
            tvd->writeVLong(pos - lastPos);
            lastPos = pos;
        }
        perDoc->perDocTvf->writeTo(tvf);
        perDoc->numVectorFields = 0;
    }

    BOOST_ASSERT(lastDocID == perDoc->docID + docWriter->getDocStoreOffset());

    ++lastDocID;

    perDoc->reset();
    free(perDoc);
    BOOST_ASSERT(IndexWriterPtr(docWriter->_writer)->testPoint(L"TermVectorsTermsWriter.finishDocument end"));
}

bool TermVectorsTermsWriter::freeRAM() {
    // We don't hold any state beyond one doc, so we don't free persistent RAM here
    return false;
}

void TermVectorsTermsWriter::abort() {
    if (tvx) {
        try {
            tvx->close();
        } catch (...) {
        }
        tvx.reset();
    }
    if (tvd) {
        try {
            tvd->close();
        } catch (...) {
        }
        tvd.reset();
    }
    if (tvf) {
        try {
            tvf->close();
        } catch (...) {
        }
        tvf.reset();
    }
    lastDocID = 0;
}

void TermVectorsTermsWriter::free(const TermVectorsTermsWriterPerDocPtr& doc) {
    SyncLock syncLock(this);
    BOOST_ASSERT(freeCount < docFreeList.size());
    docFreeList[freeCount++] = doc;
}

int32_t TermVectorsTermsWriter::bytesPerPosting() {
    return (RawPostingList::BYTES_SIZE + 3 * DocumentsWriter::INT_NUM_BYTE);
}

TermVectorsTermsWriterPerDoc::TermVectorsTermsWriterPerDoc(const TermVectorsTermsWriterPtr& termsWriter) {
    this->_termsWriter = termsWriter;
    buffer = DocumentsWriterPtr(termsWriter->_docWriter)->newPerDocBuffer();
    perDocTvf = newLucene<RAMOutputStream>(buffer);
    numVectorFields = 0;
    fieldNumbers = Collection<int32_t>::newInstance(1);
    fieldPointers = Collection<int64_t>::newInstance(1);
}

TermVectorsTermsWriterPerDoc::~TermVectorsTermsWriterPerDoc() {
}

void TermVectorsTermsWriterPerDoc::reset() {
    perDocTvf->reset();
    buffer->recycle();
    numVectorFields = 0;
}

void TermVectorsTermsWriterPerDoc::abort() {
    reset();
    TermVectorsTermsWriterPtr(_termsWriter)->free(shared_from_this());
}

void TermVectorsTermsWriterPerDoc::addField(int32_t fieldNumber) {
    if (numVectorFields == fieldNumbers.size()) {
        fieldNumbers.resize(MiscUtils::getNextSize(fieldNumbers.size()));
        fieldPointers.resize(MiscUtils::getNextSize(fieldPointers.size()));
    }
    fieldNumbers[numVectorFields] = fieldNumber;
    fieldPointers[numVectorFields] = perDocTvf->getFilePointer();
    ++numVectorFields;
}

int64_t TermVectorsTermsWriterPerDoc::sizeInBytes() {
    return buffer->getSizeInBytes();
}

void TermVectorsTermsWriterPerDoc::finish() {
    TermVectorsTermsWriterPtr(_termsWriter)->finishDocument(shared_from_this());
}

TermVectorsTermsWriterPostingList::TermVectorsTermsWriterPostingList() {
    freq = 0;
    lastOffset = 0;
    lastPosition = 0;
}

TermVectorsTermsWriterPostingList::~TermVectorsTermsWriterPostingList() {
}

}
