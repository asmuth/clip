/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "CharFilter.h"

namespace Lucene {

CharFilter::CharFilter(const CharStreamPtr& in) {
    input = in;
}

CharFilter::~CharFilter() {
}

int32_t CharFilter::correct(int32_t currentOff) {
    return currentOff;
}

int32_t CharFilter::correctOffset(int32_t currentOff) {
    return input->correctOffset(correct(currentOff));
}

void CharFilter::close() {
    input->close();
}

int32_t CharFilter::read(wchar_t* buffer, int32_t offset, int32_t length) {
    return input->read(buffer, offset, length);
}

bool CharFilter::markSupported() {
    return input->markSupported();
}

void CharFilter::mark(int32_t readAheadLimit) {
    input->mark(readAheadLimit);
}

void CharFilter::reset() {
    input->reset();
}

}
