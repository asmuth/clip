/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef DOCFIELDPROCESSORPERTHREAD_H
#define DOCFIELDPROCESSORPERTHREAD_H

#include "DocConsumerPerThread.h"
#include "DocumentsWriter.h"

namespace Lucene {

/// Gathers all Fieldables for a document under the same name, updates FieldInfos, and calls per-field
/// consumers to process field by field.
///
/// Currently, only a single thread visits the fields, sequentially, for processing.
class DocFieldProcessorPerThread : public DocConsumerPerThread {
public:
    DocFieldProcessorPerThread(const DocumentsWriterThreadStatePtr& threadState, const DocFieldProcessorPtr& docFieldProcessor);
    virtual ~DocFieldProcessorPerThread();

    LUCENE_CLASS(DocFieldProcessorPerThread);

public:
    double docBoost;
    int32_t fieldGen;
    DocFieldProcessorWeakPtr _docFieldProcessor;
    FieldInfosPtr fieldInfos;
    DocFieldConsumerPerThreadPtr consumer;
    Collection<DocFieldProcessorPerFieldPtr> _fields; // Holds all fields seen in current doc
    int32_t fieldCount;

    Collection<DocFieldProcessorPerFieldPtr> fieldHash; // Hash table for all fields ever seen
    int32_t hashMask;
    int32_t totalFieldCount;

    StoredFieldsWriterPerThreadPtr fieldsWriter;
    DocStatePtr docState;

    Collection<DocFieldProcessorPerThreadPerDocPtr> docFreeList;
    int32_t freeCount;
    int32_t allocCount;

public:
    virtual void initialize();
    virtual void abort();
    Collection<DocFieldConsumerPerFieldPtr> fields();

    // If there are fields we've seen but did not see again in the last run, then free them up.
    void trimFields(const SegmentWriteStatePtr& state);

    virtual DocWriterPtr processDocument();

    DocFieldProcessorPerThreadPerDocPtr getPerDoc();
    void freePerDoc(const DocFieldProcessorPerThreadPerDocPtr& perDoc);

protected:
    void rehash();
};

class DocFieldProcessorPerThreadPerDoc : public DocWriter {
public:
    DocFieldProcessorPerThreadPerDoc(const DocFieldProcessorPerThreadPtr& docProcessor);
    virtual ~DocFieldProcessorPerThreadPerDoc();

    LUCENE_CLASS(DocFieldProcessorPerThreadPerDoc);

public:
    DocWriterPtr one;
    DocWriterPtr two;

protected:
    DocFieldProcessorPerThreadWeakPtr _docProcessor;

public:
    virtual int64_t sizeInBytes();
    virtual void finish();
    virtual void abort();
};

}

#endif
