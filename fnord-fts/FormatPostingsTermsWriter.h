/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef FORMATPOSTINGSTERMSWRITER_H
#define FORMATPOSTINGSTERMSWRITER_H

#include "FormatPostingsTermsConsumer.h"

namespace Lucene {

class FormatPostingsTermsWriter : public FormatPostingsTermsConsumer {
public:
    FormatPostingsTermsWriter(const SegmentWriteStatePtr& state, const FormatPostingsFieldsWriterPtr& parent);
    virtual ~FormatPostingsTermsWriter();

    LUCENE_CLASS(FormatPostingsTermsWriter);

public:
    FormatPostingsFieldsWriterWeakPtr _parent;
    SegmentWriteStatePtr state;
    FormatPostingsDocsWriterPtr docsWriter;
    TermInfosWriterPtr termsOut;
    FieldInfoPtr fieldInfo;

    CharArray currentTerm;
    int32_t currentTermStart;

    int64_t freqStart;
    int64_t proxStart;

public:
    virtual void initialize();

    void setField(const FieldInfoPtr& fieldInfo);

    /// Adds a new term in this field
    virtual FormatPostingsDocsConsumerPtr addTerm(CharArray text, int32_t start);

    /// Called when we are done adding terms to this field
    virtual void finish();

    void close();
};

}

#endif
