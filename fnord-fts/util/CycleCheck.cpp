/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include <iostream>
#include "CycleCheck.h"

namespace Lucene {

MapStringInt CycleCheck::cycleMap;
Set<LuceneObjectPtr*> CycleCheck::staticRefs;

CycleCheck::~CycleCheck() {
}

void CycleCheck::addRef(const String& className, int32_t ref) {
    if (!cycleMap) {
        cycleMap = MapStringInt::newInstance();
    }
    SyncLock lockRef(&cycleMap);
    MapStringInt::iterator classRef = cycleMap.find(className);
    if (classRef == cycleMap.end()) {
        cycleMap.put(className, 1);
    } else {
        classRef->second += ref;
        if (classRef->second < 0) {
            boost::throw_exception(RuntimeException(L"invalid class reference"));
        }
    }
}

void CycleCheck::addStatic(LuceneObjectPtr* staticRef) {
#ifdef LPP_USE_CYCLIC_CHECK
    if (!staticRefs) {
        staticRefs = Set<LuceneObjectPtr*>::newInstance();
    }
    staticRefs.add(staticRef);
#endif
}

void CycleCheck::dumpRefs() {
    // destroy all registered statics
    if (staticRefs) {
        for (Set<LuceneObjectPtr*>::iterator staticRef = staticRefs.begin(); staticRef != staticRefs.end(); ++staticRef) {
            (*staticRef)->reset();
        }
    }

    if (cycleMap) {
        SyncLock lockRef(&cycleMap);
        bool reportCycles = true;
        for (MapStringInt::iterator classRef = cycleMap.begin(); classRef != cycleMap.end(); ++classRef) {
            if (classRef->second > 0) {
                if (reportCycles) {
                    std::wcout << L"Cyclic references detected!\n";
                    reportCycles = false;
                }
                std::wcout << classRef->first << L": " << classRef->second << L"\n";
            }
        }
    }
}

}
