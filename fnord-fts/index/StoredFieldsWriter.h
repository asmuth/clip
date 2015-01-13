/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef STOREDFIELDSWRITER_H
#define STOREDFIELDSWRITER_H

#include "DocumentsWriter.h"

namespace Lucene {

/// This is a DocFieldConsumer that writes stored fields.
class StoredFieldsWriter : public LuceneObject {
public:
    StoredFieldsWriter(const DocumentsWriterPtr& docWriter, const FieldInfosPtr& fieldInfos);
    virtual ~StoredFieldsWriter();

    LUCENE_CLASS(StoredFieldsWriter);

public:
    FieldsWriterPtr fieldsWriter;
    DocumentsWriterWeakPtr _docWriter;
    FieldInfosPtr fieldInfos;
    int32_t lastDocID;

    Collection<StoredFieldsWriterPerDocPtr> docFreeList;
    int32_t freeCount;
    int32_t allocCount;

public:
    StoredFieldsWriterPerThreadPtr addThread(const DocStatePtr& docState);
    void flush(const SegmentWriteStatePtr& state);
    void closeDocStore(const SegmentWriteStatePtr& state);
    StoredFieldsWriterPerDocPtr getPerDoc();
    void abort();

    /// Fills in any hole in the docIDs
    void fill(int32_t docID);

    void finishDocument(const StoredFieldsWriterPerDocPtr& perDoc);
    bool freeRAM();
    void free(const StoredFieldsWriterPerDocPtr& perDoc);

protected:
    void initFieldsWriter();
};

class StoredFieldsWriterPerDoc : public DocWriter {
public:
    StoredFieldsWriterPerDoc(const StoredFieldsWriterPtr& fieldsWriter);
    virtual ~StoredFieldsWriterPerDoc();

    LUCENE_CLASS(StoredFieldsWriterPerDoc);

protected:
    StoredFieldsWriterWeakPtr _fieldsWriter;

public:
    PerDocBufferPtr buffer;
    RAMOutputStreamPtr fdt;
    int32_t numStoredFields;

public:
    void reset();
    virtual void abort();
    virtual int64_t sizeInBytes();
    virtual void finish();
};

}

#endif
