/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef _CHECKINDEX_H
#define _CHECKINDEX_H

#include "SegmentTermDocs.h"

namespace Lucene {

class MySegmentTermDocs : public SegmentTermDocs {
public:
    MySegmentTermDocs(const SegmentReaderPtr& p);
    virtual ~MySegmentTermDocs();

    LUCENE_CLASS(MySegmentTermDocs);

public:
    int32_t delCount;

public:
    virtual void seek(const TermPtr& term);
    virtual void skippingDoc();
};

}

#endif
