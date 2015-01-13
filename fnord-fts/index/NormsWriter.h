/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef NORMSWRITER_H
#define NORMSWRITER_H

#include "InvertedDocEndConsumer.h"

namespace Lucene {

/// Writes norms.  Each thread X field accumulates the norms for the doc/fields it saw, then the flush method
/// below merges all of these together into a single _X.nrm file.
class NormsWriter : public InvertedDocEndConsumer {
public:
    NormsWriter();
    virtual ~NormsWriter();

    LUCENE_CLASS(NormsWriter);

protected:
    FieldInfosPtr fieldInfos;

public:
    virtual InvertedDocEndConsumerPerThreadPtr addThread(const DocInverterPerThreadPtr& docInverterPerThread);
    virtual void abort();

    // We only write the _X.nrm file at flush
    virtual void files(HashSet<String> files);

    virtual void setFieldInfos(const FieldInfosPtr& fieldInfos);

    /// Produce _X.nrm if any document had a field with norms not disabled
    virtual void flush(MapInvertedDocEndConsumerPerThreadCollectionInvertedDocEndConsumerPerField threadsAndFields, const SegmentWriteStatePtr& state);
    virtual void closeDocStore(const SegmentWriteStatePtr& state);

protected:
    static uint8_t getDefaultNorm();
};

}

#endif
