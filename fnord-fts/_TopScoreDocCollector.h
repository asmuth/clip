/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef _TOPSCOREDOCCOLLECTOR_H
#define _TOPSCOREDOCCOLLECTOR_H

#include "TopDocsCollector.h"

namespace Lucene {

/// Assumes docs are scored in order.
class InOrderTopScoreDocCollector : public TopScoreDocCollector {
public:
    InOrderTopScoreDocCollector(int32_t numHits);
    virtual ~InOrderTopScoreDocCollector();

    LUCENE_CLASS(InOrderTopScoreDocCollector);

public:
    virtual void collect(int32_t doc);
    virtual bool acceptsDocsOutOfOrder();
};

/// Assumes docs are scored out of order.
class OutOfOrderTopScoreDocCollector : public TopScoreDocCollector {
public:
    OutOfOrderTopScoreDocCollector(int32_t numHits);
    virtual ~OutOfOrderTopScoreDocCollector();

    LUCENE_CLASS(OutOfOrderTopScoreDocCollector);

public:
    virtual void collect(int32_t doc);
    virtual bool acceptsDocsOutOfOrder();
};

}

#endif
