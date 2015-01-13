/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef POSITIVESCORESONLYCOLLECTOR_H
#define POSITIVESCORESONLYCOLLECTOR_H

#include "fnord-fts/search/Collector.h"

namespace Lucene {

/// A {@link Collector} implementation which wraps another {@link Collector} and makes sure only
/// documents with scores > 0 are collected.
class PositiveScoresOnlyCollector : public Collector {
public:
    PositiveScoresOnlyCollector(const CollectorPtr& c);
    virtual ~PositiveScoresOnlyCollector();

    LUCENE_CLASS(PositiveScoresOnlyCollector);

protected:
    CollectorPtr collector;
    ScorerPtr scorer;

public:
    virtual void collect(int32_t doc);
    virtual void setNextReader(const IndexReaderPtr& reader, int32_t docBase);
    virtual void setScorer(const ScorerPtr& scorer);
    virtual bool acceptsDocsOutOfOrder();
};

}

#endif
