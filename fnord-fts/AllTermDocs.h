/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef ALLTERMDOCS_H
#define ALLTERMDOCS_H

#include "AbstractAllTermDocs.h"

namespace Lucene {

class AllTermDocs : public AbstractAllTermDocs {
public:
    AllTermDocs(const SegmentReaderPtr& parent);
    virtual ~AllTermDocs();

    LUCENE_CLASS(AllTermDocs);

protected:
    BitVectorWeakPtr _deletedDocs;

public:
    virtual bool isDeleted(int32_t doc);
};

}

#endif
