/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef MULTIPLETERMPOSITIONS_H
#define MULTIPLETERMPOSITIONS_H

#include "fnord-fts/index/TermPositions.h"

namespace fnord {
namespace fts {

/// Allows you to iterate over the {@link TermPositions} for multiple {@link Term}s as a single
/// {@link TermPositions}.
class MultipleTermPositions : public TermPositions, public LuceneObject {
public:
    MultipleTermPositions(const IndexReaderPtr& indexReader, Collection<TermPtr> terms);
    virtual ~MultipleTermPositions();

    LUCENE_CLASS(MultipleTermPositions);

protected:
    int32_t _doc;
    int32_t _freq;
    TermPositionsQueuePtr termPositionsQueue;
    IntQueuePtr posList;

public:
    virtual bool next();
    virtual int32_t nextPosition();
    virtual bool skipTo(int32_t target);
    virtual int32_t doc();
    virtual int32_t freq();
    virtual void close();

    /// Not implemented.
    virtual void seek(const TermPtr& term);

    /// Not implemented.
    virtual void seek(const TermEnumPtr& termEnum);

    /// Not implemented.
    virtual int32_t read(Collection<int32_t> docs, Collection<int32_t> freqs);

    /// Not implemented.
    virtual ByteArray getPayload(ByteArray data, int32_t offset);

    /// @return false
    virtual bool isPayloadAvailable();
};

}

}
#endif
