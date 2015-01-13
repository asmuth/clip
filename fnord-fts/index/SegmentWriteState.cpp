/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "SegmentWriteState.h"

namespace Lucene {

SegmentWriteState::SegmentWriteState(const DocumentsWriterPtr& docWriter, const DirectoryPtr& directory, const String& segmentName,
                                     const String& docStoreSegmentName, int32_t numDocs, int32_t numDocsInStore,
                                     int32_t termIndexInterval) {
    this->_docWriter = docWriter;
    this->directory = directory;
    this->segmentName = segmentName;
    this->docStoreSegmentName = docStoreSegmentName;
    this->numDocs = numDocs;
    this->numDocsInStore = numDocsInStore;
    this->termIndexInterval = termIndexInterval;
    this->flushedFiles = HashSet<String>::newInstance();
}

SegmentWriteState::~SegmentWriteState() {
}

String SegmentWriteState::segmentFileName(const String& ext) {
    return segmentName + L"." + ext;
}

}
