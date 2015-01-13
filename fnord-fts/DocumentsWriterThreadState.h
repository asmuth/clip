/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef DOCUMENTSWRITERTHREADSTATE_H
#define DOCUMENTSWRITERTHREADSTATE_H

#include "LuceneObject.h"

namespace Lucene {

/// Used by DocumentsWriter to maintain per-thread state.
/// We keep a separate Posting hash and other state for each thread and then merge postings
/// hashes from all threads when writing the segment.
class DocumentsWriterThreadState : public LuceneObject {
public:
    DocumentsWriterThreadState(const DocumentsWriterPtr& docWriter);
    virtual ~DocumentsWriterThreadState();

    LUCENE_CLASS(DocumentsWriterThreadState);

public:
    bool isIdle; // false if this is currently in use by a thread
    int32_t numThreads; // Number of threads that share this instance
    bool doFlushAfter; // true if we should flush after processing current doc
    DocConsumerPerThreadPtr consumer;
    DocStatePtr docState;
    DocumentsWriterWeakPtr _docWriter;

public:
    virtual void initialize();
    void doAfterFlush();
};

}

#endif
