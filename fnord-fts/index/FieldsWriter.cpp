/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "FieldsWriter.h"
#include "IndexFileNames.h"
#include "Directory.h"
#include "IndexOutput.h"
#include "RAMOutputStream.h"
#include "FieldInfo.h"
#include "FieldInfos.h"
#include "Fieldable.h"
#include "Document.h"
#include "TestPoint.h"

namespace Lucene {

const uint8_t FieldsWriter::FIELD_IS_TOKENIZED = 0x1;
const uint8_t FieldsWriter::FIELD_IS_BINARY = 0x2;
const uint8_t FieldsWriter::FIELD_IS_COMPRESSED = 0x4;

const int32_t FieldsWriter::FORMAT = 0; // Original format
const int32_t FieldsWriter::FORMAT_VERSION_UTF8_LENGTH_IN_BYTES = 1; // Changed strings to UTF8
const int32_t FieldsWriter::FORMAT_LUCENE_3_0_NO_COMPRESSED_FIELDS = 2; // Lucene 3.0: Removal of compressed fields

// NOTE: if you introduce a new format, make it 1 higher than the current one, and always change this if you
// switch to a new format!
const int32_t FieldsWriter::FORMAT_CURRENT = FieldsWriter::FORMAT_LUCENE_3_0_NO_COMPRESSED_FIELDS;

FieldsWriter::FieldsWriter(const DirectoryPtr& d, const String& segment, const FieldInfosPtr& fn) {
    fieldInfos = fn;

    bool success = false;
    String fieldsName(segment + L"." + IndexFileNames::FIELDS_EXTENSION());
    LuceneException finally;
    try {
        fieldsStream = d->createOutput(fieldsName);
        fieldsStream->writeInt(FORMAT_CURRENT);
        success = true;
    } catch (LuceneException& e) {
        finally = e;
    }
    if (!success) {
        try {
            close();
            d->deleteFile(fieldsName);
        } catch (...) {
            // Suppress so we keep throwing the original exception
        }
    }
    finally.throwException();

    success = false;
    String indexName(segment + L"." + IndexFileNames::FIELDS_INDEX_EXTENSION());
    try {
        indexStream = d->createOutput(indexName);
        indexStream->writeInt(FORMAT_CURRENT);
        success = true;
    } catch (LuceneException& e) {
        finally = e;
    }
    if (!success) {
        try {
            close();
            d->deleteFile(fieldsName);
            d->deleteFile(indexName);
        } catch (...) {
            // Suppress so we keep throwing the original exception
        }
    }
    finally.throwException();

    doClose = true;
}

FieldsWriter::FieldsWriter(const IndexOutputPtr& fdx, const IndexOutputPtr& fdt, const FieldInfosPtr& fn) {
    fieldInfos = fn;
    fieldsStream = fdt;
    indexStream = fdx;
    doClose = false;
}

FieldsWriter::~FieldsWriter() {
}

void FieldsWriter::setFieldsStream(const IndexOutputPtr& stream) {
    this->fieldsStream = stream;
}

void FieldsWriter::flushDocument(int32_t numStoredFields, const RAMOutputStreamPtr& buffer) {
    TestScope testScope(L"FieldsWriter", L"flushDocument");
    indexStream->writeLong(fieldsStream->getFilePointer());
    fieldsStream->writeVInt(numStoredFields);
    buffer->writeTo(fieldsStream);
}

void FieldsWriter::skipDocument() {
    indexStream->writeLong(fieldsStream->getFilePointer());
    fieldsStream->writeVInt(0);
}

void FieldsWriter::flush() {
    indexStream->flush();
    fieldsStream->flush();
}

void FieldsWriter::close() {
    if (doClose) {
        LuceneException finally;
        if (fieldsStream) {
            try {
                fieldsStream->close();
            } catch (LuceneException& e) {
                finally = e;
            }
            fieldsStream.reset();
        }
        if (indexStream) {
            try {
                indexStream->close();
            } catch (LuceneException& e) {
                if (finally.isNull()) { // throw first exception hit
                    finally = e;
                }
            }
            indexStream.reset();
        }
        finally.throwException();
    }
}

void FieldsWriter::writeField(const FieldInfoPtr& fi, const FieldablePtr& field) {
    fieldsStream->writeVInt(fi->number);
    uint8_t bits = 0;
    if (field->isTokenized()) {
        bits |= FIELD_IS_TOKENIZED;
    }
    if (field->isBinary()) {
        bits |= FIELD_IS_BINARY;
    }

    fieldsStream->writeByte(bits);

    if (field->isBinary()) {
        ByteArray data(field->getBinaryValue());
        int32_t len = field->getBinaryLength();
        int32_t offset = field->getBinaryOffset();

        fieldsStream->writeVInt(len);
        fieldsStream->writeBytes(data.get(), offset, len);
    } else {
        fieldsStream->writeString(field->stringValue());
    }
}

void FieldsWriter::addRawDocuments(const IndexInputPtr& stream, Collection<int32_t> lengths, int32_t numDocs) {
    int64_t position = fieldsStream->getFilePointer();
    int64_t start = position;
    for (int32_t i = 0; i < numDocs; ++i) {
        indexStream->writeLong(position);
        position += lengths[i];
    }
    fieldsStream->copyBytes(stream, position - start);
    BOOST_ASSERT(fieldsStream->getFilePointer() == position);
}

void FieldsWriter::addDocument(const DocumentPtr& doc) {
    indexStream->writeLong(fieldsStream->getFilePointer());

    int32_t storedCount = 0;
    Collection<FieldablePtr> fields(doc->getFields());
    for (Collection<FieldablePtr>::iterator field = fields.begin(); field != fields.end(); ++field) {
        if ((*field)->isStored()) {
            ++storedCount;
        }
    }
    fieldsStream->writeVInt(storedCount);

    for (Collection<FieldablePtr>::iterator field = fields.begin(); field != fields.end(); ++field) {
        if ((*field)->isStored()) {
            writeField(fieldInfos->fieldInfo((*field)->name()), *field);
        }
    }
}

}
