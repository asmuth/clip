/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef _NATIVEFSLOCKFACTORY_H
#define _NATIVEFSLOCKFACTORY_H

#include "Lock.h"

namespace Lucene {

class NativeFSLock : public Lock {
public:
    NativeFSLock(const String& lockDir, const String& lockFileName);
    virtual ~NativeFSLock();

    LUCENE_CLASS(NativeFSLock);

protected:
    String lockDir;
    String path;
    filelockPtr lock;

    static SynchronizePtr LOCK_HELD_LOCK();
    static HashSet<String> LOCK_HELD();

public:
    virtual bool obtain();
    virtual void release();
    virtual bool isLocked();
    virtual String toString();

protected:
    bool lockExists();
};

}

#endif
