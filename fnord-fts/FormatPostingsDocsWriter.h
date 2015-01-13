/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef FORMATPOSTINGSDOCSWRITER_H
#define FORMATPOSTINGSDOCSWRITER_H

#include "FormatPostingsDocsConsumer.h"

namespace Lucene {

/// Consumes doc & freq, writing them using the current index file format
class FormatPostingsDocsWriter : public FormatPostingsDocsConsumer {
public:
    FormatPostingsDocsWriter(const SegmentWriteStatePtr& state, const FormatPostingsTermsWriterPtr& parent);
    virtual ~FormatPostingsDocsWriter();

    LUCENE_CLASS(FormatPostingsDocsWriter);

public:
    IndexOutputPtr out;
    FormatPostingsTermsWriterWeakPtr _parent;
    SegmentWriteStatePtr state;
    FormatPostingsPositionsWriterPtr posWriter;
    DefaultSkipListWriterPtr skipListWriter;
    int32_t skipInterval;
    int32_t totalNumDocs;

    bool omitTermFreqAndPositions;
    bool storePayloads;
    int64_t freqStart;
    FieldInfoPtr fieldInfo;

    int32_t lastDocID;
    int32_t df;

    TermInfoPtr termInfo; // minimize consing
    UTF8ResultPtr utf8;

public:
    virtual void initialize();

    void setField(const FieldInfoPtr& fieldInfo);

    /// Adds a new doc in this term.  If this returns null then we just skip consuming positions/payloads.
    virtual FormatPostingsPositionsConsumerPtr addDoc(int32_t docID, int32_t termDocFreq);

    /// Called when we are done adding docs to this term
    virtual void finish();

    void close();
};

}

#endif
