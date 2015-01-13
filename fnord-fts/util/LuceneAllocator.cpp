/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "LuceneAllocator.h"

namespace Lucene {

void* AllocMemory(size_t size) {
#if (defined(_WIN32) || defined(_WIN64)) && !defined(NDEBUG)
    return _malloc_dbg(size, _NORMAL_BLOCK, __FILE__, __LINE__);
#else
    return malloc(size);
#endif
}

void* ReallocMemory(void* memory, size_t size) {
    if (memory == NULL) {
        return AllocMemory(size);
    }
    if (size == 0) {
        FreeMemory(memory);
        return NULL;
    }
#if defined(_WIN32) && !defined(NDEBUG)
    return _realloc_dbg(memory, size, _NORMAL_BLOCK, __FILE__, __LINE__);
#else
    return realloc(memory, size);
#endif
}

void FreeMemory(void* memory) {
    if (memory == NULL) {
        return;
    }
#if defined(_WIN32) && !defined(NDEBUG)
    _free_dbg(memory, _NORMAL_BLOCK);
#else
    free(memory);
#endif
}

}
