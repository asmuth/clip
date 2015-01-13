/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef _NOLOCKFACTORY_H
#define _NOLOCKFACTORY_H

#include "Lock.h"

namespace Lucene {

class NoLock : public Lock {
public:
    virtual ~NoLock();

    LUCENE_CLASS(NoLock);

public:
    virtual bool obtain();
    virtual void release();
    virtual bool isLocked();
    virtual String toString();
};

}

#endif
