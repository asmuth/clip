/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef DEFAULTSKIPLISTREADER_H
#define DEFAULTSKIPLISTREADER_H

#include "MultiLevelSkipListReader.h"

namespace Lucene {

/// Implements the skip list reader for the default posting list format that stores positions and payloads.
class DefaultSkipListReader : public MultiLevelSkipListReader {
public:
    DefaultSkipListReader(const IndexInputPtr& skipStream, int32_t maxSkipLevels, int32_t skipInterval);
    virtual ~DefaultSkipListReader();

    LUCENE_CLASS(DefaultSkipListReader);

protected:
    bool currentFieldStoresPayloads;
    Collection<int64_t> freqPointer;
    Collection<int64_t> proxPointer;
    Collection<int32_t> payloadLength;

    int64_t lastFreqPointer;
    int64_t lastProxPointer;
    int32_t lastPayloadLength;

public:
    void init(int64_t skipPointer, int64_t freqBasePointer, int64_t proxBasePointer, int32_t df, bool storesPayloads);

    /// Returns the freq pointer of the doc to which the last call of {@link MultiLevelSkipListReader#skipTo(int)}
    /// has skipped.
    int64_t getFreqPointer();

    /// Returns the prox pointer of the doc to which the last call of {@link MultiLevelSkipListReader#skipTo(int)}
    /// has skipped.
    int64_t getProxPointer();

    /// Returns the payload length of the payload stored just before the doc to which the last call of {@link
    /// MultiLevelSkipListReader#skipTo(int)} has skipped.
    int32_t getPayloadLength();

protected:
    /// Seeks the skip entry on the given level
    virtual void seekChild(int32_t level);

    /// Copies the values of the last read skip entry on this level
    virtual void setLastSkipData(int32_t level);

    /// Subclasses must implement the actual skip data encoding in this method.
    virtual int32_t readSkipData(int32_t level, const IndexInputPtr& skipStream);
};

}

#endif
