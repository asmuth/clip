/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef _MULTITERMQUERY_H
#define _MULTITERMQUERY_H

#include "LuceneObject.h"

namespace Lucene {

class ConstantScoreFilterRewrite : public RewriteMethod {
public:
    virtual ~ConstantScoreFilterRewrite();
    LUCENE_CLASS(ConstantScoreFilterRewrite);

public:
    virtual QueryPtr rewrite(const IndexReaderPtr& reader, const MultiTermQueryPtr& query);
};

class ScoringBooleanQueryRewrite : public RewriteMethod {
public:
    virtual ~ScoringBooleanQueryRewrite();
    LUCENE_CLASS(ScoringBooleanQueryRewrite);

public:
    virtual QueryPtr rewrite(const IndexReaderPtr& reader, const MultiTermQueryPtr& query);
};

class ConstantScoreBooleanQueryRewrite : public ScoringBooleanQueryRewrite {
public:
    virtual ~ConstantScoreBooleanQueryRewrite();
    LUCENE_CLASS(ConstantScoreBooleanQueryRewrite);

public:
    virtual QueryPtr rewrite(const IndexReaderPtr& reader, const MultiTermQueryPtr& query);
};

class ConstantScoreAutoRewriteDefault : public ConstantScoreAutoRewrite {
public:
    virtual ~ConstantScoreAutoRewriteDefault();
    LUCENE_CLASS(ConstantScoreAutoRewriteDefault);

public:
    virtual void setTermCountCutoff(int32_t count);
    virtual void setDocCountPercent(double percent);
};

}

#endif
