/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "TermInfo.h"

namespace Lucene {

TermInfo::TermInfo(const TermInfoPtr& ti) {
    set(ti);
}

TermInfo::TermInfo(int32_t df, int64_t fp, int64_t pp) {
    docFreq = df;
    freqPointer = fp;
    proxPointer = pp;
    skipOffset = 0;
}

TermInfo::~TermInfo() {
}

void TermInfo::set(int32_t docFreq, int64_t freqPointer, int64_t proxPointer, int32_t skipOffset) {
    this->docFreq = docFreq;
    this->freqPointer = freqPointer;
    this->proxPointer = proxPointer;
    this->skipOffset = skipOffset;
}

void TermInfo::set(const TermInfoPtr& ti) {
    docFreq = ti->docFreq;
    freqPointer = ti->freqPointer;
    proxPointer = ti->proxPointer;
    skipOffset = ti->skipOffset;
}

}
