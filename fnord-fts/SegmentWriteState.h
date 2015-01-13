/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef SEGMENTWRITESTATE_H
#define SEGMENTWRITESTATE_H

#include "LuceneObject.h"

namespace Lucene {

class SegmentWriteState : public LuceneObject {
public:
    SegmentWriteState(const DocumentsWriterPtr& docWriter, const DirectoryPtr& directory, const String& segmentName,
                      const String& docStoreSegmentName, int32_t numDocs, int32_t numDocsInStore,
                      int32_t termIndexInterval);
    virtual ~SegmentWriteState();

    LUCENE_CLASS(SegmentWriteState);

public:
    DocumentsWriterWeakPtr _docWriter;
    DirectoryPtr directory;
    String segmentName;
    String docStoreSegmentName;
    int32_t numDocs;
    int32_t termIndexInterval;
    int32_t numDocsInStore;
    HashSet<String> flushedFiles;

public:
    String segmentFileName(const String& ext);
};

}

#endif
