/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef CYCLECHECK_H
#define CYCLECHECK_H

#include "fnord-fts/fts.h"

namespace fnord {
namespace fts {

/// Debug utility to track std::shared_ptr utilization.
class CycleCheck {
public:
    virtual ~CycleCheck();

protected:
    static MapStringInt cycleMap;
    static Set<LuceneObjectPtr*> staticRefs;

protected:
    void addRef(const String& className, int32_t ref);
    static void addStatic(LuceneObjectPtr* staticRef);

public:
    template <class TYPE>
    static void addStatic(TYPE& staticRef) {
        addStatic(reinterpret_cast<LuceneObjectPtr*>(&staticRef));
    }

    static void dumpRefs();
};

template <class TYPE>
class CycleCheckT : public CycleCheck {
public:
    CycleCheckT() {
        addRef(TYPE::_getClassName(), 1);
    }

    virtual ~CycleCheckT() {
        addRef(TYPE::_getClassName(), -1);
    }
};

}

}
#endif
