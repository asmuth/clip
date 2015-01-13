/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef DEFAULTSKIPLISTWRITER_H
#define DEFAULTSKIPLISTWRITER_H

#include "MultiLevelSkipListWriter.h"

namespace Lucene {

/// Implements the skip list writer for the default posting list format that stores positions and payloads.
class DefaultSkipListWriter : public MultiLevelSkipListWriter {
public:
    DefaultSkipListWriter(int32_t skipInterval, int32_t numberOfSkipLevels, int32_t docCount, const IndexOutputPtr& freqOutput, const IndexOutputPtr& proxOutput);
    virtual ~DefaultSkipListWriter();

    LUCENE_CLASS(DefaultSkipListWriter);

protected:
    Collection<int32_t> lastSkipDoc;
    Collection<int32_t> lastSkipPayloadLength;
    Collection<int64_t> lastSkipFreqPointer;
    Collection<int64_t> lastSkipProxPointer;

    IndexOutputPtr freqOutput;
    IndexOutputPtr proxOutput;

    int32_t curDoc;
    bool curStorePayloads;
    int32_t curPayloadLength;
    int64_t curFreqPointer;
    int64_t curProxPointer;

public:
    void setFreqOutput(const IndexOutputPtr& freqOutput);
    void setProxOutput(const IndexOutputPtr& proxOutput);

    /// Sets the values for the current skip data.
    void setSkipData(int32_t doc, bool storePayloads, int32_t payloadLength);

protected:
    virtual void resetSkip();
    virtual void writeSkipData(int32_t level, const IndexOutputPtr& skipBuffer);

    friend class FormatPostingsTermsWriter;
};

}

#endif
