/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "DocFieldProcessorPerThread.h"
#include "DocFieldProcessorPerField.h"
#include "DocFieldProcessor.h"
#include "DocFieldConsumer.h"
#include "DocFieldConsumerPerThread.h"
#include "DocFieldConsumerPerField.h"
#include "DocumentsWriterThreadState.h"
#include "DocumentsWriter.h"
#include "StoredFieldsWriter.h"
#include "StoredFieldsWriterPerThread.h"
#include "SegmentWriteState.h"
#include "FieldInfo.h"
#include "FieldInfos.h"
#include "Fieldable.h"
#include "IndexWriter.h"
#include "Document.h"
#include "InfoStream.h"
#include "MiscUtils.h"
#include "StringUtils.h"

namespace Lucene {

DocFieldProcessorPerThread::DocFieldProcessorPerThread(const DocumentsWriterThreadStatePtr& threadState, const DocFieldProcessorPtr& docFieldProcessor) {
    _fields = Collection<DocFieldProcessorPerFieldPtr>::newInstance(1);
    fieldHash = Collection<DocFieldProcessorPerFieldPtr>::newInstance(2);
    hashMask = 1;
    fieldGen = 0;
    fieldCount = 0;
    totalFieldCount = 0;

    this->docState = threadState->docState;
    this->_docFieldProcessor = docFieldProcessor;
    this->fieldInfos = docFieldProcessor->fieldInfos;

    docFreeList = Collection<DocFieldProcessorPerThreadPerDocPtr>::newInstance(1);
    freeCount = 0;
    allocCount = 0;
}

DocFieldProcessorPerThread::~DocFieldProcessorPerThread() {
}

void DocFieldProcessorPerThread::initialize() {
    DocFieldProcessorPtr docFieldProcessor(_docFieldProcessor);
    consumer = docFieldProcessor->consumer->addThread(shared_from_this());
    fieldsWriter = docFieldProcessor->fieldsWriter->addThread(docState);
}

void DocFieldProcessorPerThread::abort() {
    for (Collection<DocFieldProcessorPerFieldPtr>::iterator field = fieldHash.begin(); field != fieldHash.end(); ++field) {
        DocFieldProcessorPerFieldPtr current(*field);
        while (current) {
            DocFieldProcessorPerFieldPtr next(current->next);
            current->abort();
            current = next;
        }
    }
    fieldsWriter->abort();
    consumer->abort();
}

Collection<DocFieldConsumerPerFieldPtr> DocFieldProcessorPerThread::fields() {
    Collection<DocFieldConsumerPerFieldPtr> fields(Collection<DocFieldConsumerPerFieldPtr>::newInstance());
    for (Collection<DocFieldProcessorPerFieldPtr>::iterator field = fieldHash.begin(); field != fieldHash.end(); ++field) {
        DocFieldProcessorPerFieldPtr current(*field);
        while (current) {
            fields.add(current->consumer);
            current = current->next;
        }
    }
    BOOST_ASSERT(fields.size() == totalFieldCount);
    return fields;
}

void DocFieldProcessorPerThread::trimFields(const SegmentWriteStatePtr& state) {
    for (Collection<DocFieldProcessorPerFieldPtr>::iterator perField = fieldHash.begin(); perField != fieldHash.end(); ++perField) {
        DocFieldProcessorPerFieldPtr current(*perField);
        DocFieldProcessorPerFieldPtr lastPerField;

        while (current) {
            if (current->lastGen == -1) {
                // This field was not seen since the previous flush, so, free up its resources now

                // Unhash
                if (!lastPerField) {
                    *perField = current->next;
                } else {
                    lastPerField->next = current->next;
                }

                DocumentsWriterPtr docWriter(state->_docWriter);
                if (docWriter->infoStream) {
                    *(docWriter->infoStream) << L"  purge field=" << current->fieldInfo->name << L"\n";
                }

                --totalFieldCount;
            } else {
                // Reset
                current->lastGen = -1;
                lastPerField = current;
            }

            current = current->next;
        }
    }
}

void DocFieldProcessorPerThread::rehash() {
    int32_t newHashSize = (fieldHash.size() * 2);
    BOOST_ASSERT(newHashSize > fieldHash.size());

    Collection<DocFieldProcessorPerFieldPtr> newHashArray(Collection<DocFieldProcessorPerFieldPtr>::newInstance(newHashSize));

    // Rehash
    int32_t newHashMask = newHashSize - 1;
    for (Collection<DocFieldProcessorPerFieldPtr>::iterator fp0 = fieldHash.begin(); fp0 != fieldHash.end(); ++fp0) {
        DocFieldProcessorPerFieldPtr current(*fp0);
        while (current) {
            int32_t hashPos2 = StringUtils::hashCode(current->fieldInfo->name) & newHashMask;
            DocFieldProcessorPerFieldPtr nextFP0(current->next);
            current->next = newHashArray[hashPos2];
            newHashArray[hashPos2] = current;
            current = nextFP0;
        }
    }

    fieldHash = newHashArray;
    hashMask = newHashMask;
}

struct lessFieldInfoName {
    inline bool operator()(const DocFieldProcessorPerFieldPtr& first, const DocFieldProcessorPerFieldPtr& second) const {
        return (first->fieldInfo->name < second->fieldInfo->name);
    }
};

DocWriterPtr DocFieldProcessorPerThread::processDocument() {
    consumer->startDocument();
    fieldsWriter->startDocument();

    DocumentPtr doc(docState->doc);

    DocFieldProcessorPtr docFieldProcessor(_docFieldProcessor);
    DocumentsWriterPtr docWriter(docFieldProcessor->_docWriter);
    bool testPoint = IndexWriterPtr(docWriter->_writer)->testPoint(L"DocumentsWriter.ThreadState.init start");
    BOOST_ASSERT(testPoint);

    fieldCount = 0;
    int32_t thisFieldGen = fieldGen++;

    Collection<FieldablePtr> docFields(doc->getFields());

    // Absorb any new fields first seen in this document.
    // Also absorb any changes to fields we had already seen before (eg suddenly turning on norms or
    // vectors, etc.)
    for (Collection<FieldablePtr>::iterator field = docFields.begin(); field != docFields.end(); ++field) {
        String fieldName((*field)->name());

        // Make sure we have a PerField allocated
        int32_t hashPos = StringUtils::hashCode(fieldName) & hashMask;

        DocFieldProcessorPerFieldPtr fp(fieldHash[hashPos]);
        while (fp && fp->fieldInfo->name != fieldName) {
            fp = fp->next;
        }

        if (!fp) {
            FieldInfoPtr fi(fieldInfos->add(fieldName, (*field)->isIndexed(), (*field)->isTermVectorStored(),
                                            (*field)->isStorePositionWithTermVector(), (*field)->isStoreOffsetWithTermVector(),
                                            (*field)->getOmitNorms(), false, (*field)->getOmitTermFreqAndPositions()));

            fp = newLucene<DocFieldProcessorPerField>(shared_from_this(), fi);
            fp->next = fieldHash[hashPos];
            fieldHash[hashPos] = fp;
            ++totalFieldCount;

            if (totalFieldCount >= fieldHash.size() / 2) {
                rehash();
            }
        } else {
            fp->fieldInfo->update((*field)->isIndexed(), (*field)->isTermVectorStored(),
                                  (*field)->isStorePositionWithTermVector(), (*field)->isStoreOffsetWithTermVector(),
                                  (*field)->getOmitNorms(), false, (*field)->getOmitTermFreqAndPositions());
        }

        if (thisFieldGen != fp->lastGen) {
            // First time we're seeing this field for this doc
            fp->fieldCount = 0;

            if (fieldCount == _fields.size()) {
                _fields.resize(_fields.size() * 2);
            }

            _fields[fieldCount++] = fp;
            fp->lastGen = thisFieldGen;
        }

        if (fp->fieldCount == fp->fields.size()) {
            fp->fields.resize(fp->fields.size() * 2);
        }

        fp->fields[fp->fieldCount++] = *field;
        if ((*field)->isStored()) {
            fieldsWriter->addField(*field, fp->fieldInfo);
        }
    }

    // If we are writing vectors then we must visit fields in sorted order so they are written in sorted order.
    std::sort(_fields.begin(), _fields.begin() + fieldCount, lessFieldInfoName());

    for (int32_t i = 0; i < fieldCount; ++i) {
        _fields[i]->consumer->processFields(_fields[i]->fields, _fields[i]->fieldCount);
    }

    if (!docState->maxTermPrefix.empty() && docState->infoStream) {
        *(docState->infoStream) << L"WARNING: document contains at least one immense term (longer than the max length " <<
                                StringUtils::toString(DocumentsWriter::MAX_TERM_LENGTH) << L"), all of which were skipped.  " <<
                                L"Please correct the analyzer to not produce such terms.  The prefix of the first immense " <<
                                L"term is: '" << StringUtils::toString(docState->maxTermPrefix) << L"...'\n";
        docState->maxTermPrefix.clear();
    }

    DocWriterPtr one(fieldsWriter->finishDocument());
    DocWriterPtr two(consumer->finishDocument());

    if (!one) {
        return two;
    } else if (!two) {
        return one;
    } else {
        DocFieldProcessorPerThreadPerDocPtr both(getPerDoc());
        both->docID = docState->docID;
        BOOST_ASSERT(one->docID == docState->docID);
        BOOST_ASSERT(two->docID == docState->docID);
        both->one = one;
        both->two = two;
        return both;
    }
}

DocFieldProcessorPerThreadPerDocPtr DocFieldProcessorPerThread::getPerDoc() {
    SyncLock syncLock(this);
    if (freeCount == 0) {
        ++allocCount;
        if (allocCount > docFreeList.size()) {
            // Grow our free list up front to make sure we have enough space to recycle all
            // outstanding PerDoc instances
            BOOST_ASSERT(allocCount == docFreeList.size() + 1);
            docFreeList.resize(MiscUtils::getNextSize(allocCount));
        }
        return newLucene<DocFieldProcessorPerThreadPerDoc>(shared_from_this());
    } else {
        return docFreeList[--freeCount];
    }
}

void DocFieldProcessorPerThread::freePerDoc(const DocFieldProcessorPerThreadPerDocPtr& perDoc) {
    SyncLock syncLock(this);
    BOOST_ASSERT(freeCount < docFreeList.size());
    docFreeList[freeCount++] = perDoc;
}

DocFieldProcessorPerThreadPerDoc::DocFieldProcessorPerThreadPerDoc(const DocFieldProcessorPerThreadPtr& docProcessor) {
    this->_docProcessor = docProcessor;
}

DocFieldProcessorPerThreadPerDoc::~DocFieldProcessorPerThreadPerDoc() {
}

int64_t DocFieldProcessorPerThreadPerDoc::sizeInBytes() {
    return one->sizeInBytes() + two->sizeInBytes();
}

void DocFieldProcessorPerThreadPerDoc::finish() {
    LuceneException finally;
    try {
        try {
            one->finish();
        } catch (LuceneException& e) {
            finally = e;
        }
        two->finish();
    } catch (LuceneException& e) {
        finally = e;
    }
    DocFieldProcessorPerThreadPtr(_docProcessor)->freePerDoc(shared_from_this());
    finally.throwException();
}

void DocFieldProcessorPerThreadPerDoc::abort() {
    LuceneException finally;
    try {
        try {
            one->abort();
        } catch (LuceneException& e) {
            finally = e;
        }
        two->abort();
    } catch (LuceneException& e) {
        finally = e;
    }
    DocFieldProcessorPerThreadPtr(_docProcessor)->freePerDoc(shared_from_this());
    finally.throwException();
}

}
