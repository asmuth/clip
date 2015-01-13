/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "AbstractAllTermDocs.h"

namespace Lucene {

AbstractAllTermDocs::AbstractAllTermDocs(int32_t maxDoc) {
    this->maxDoc = maxDoc;
    this->_doc = -1;
}

AbstractAllTermDocs::~AbstractAllTermDocs() {
}

void AbstractAllTermDocs::seek(const TermPtr& term) {
    if (!term) {
        _doc = -1;
    } else {
        boost::throw_exception(UnsupportedOperationException());
    }
}

void AbstractAllTermDocs::seek(const TermEnumPtr& termEnum) {
    boost::throw_exception(UnsupportedOperationException());
}

int32_t AbstractAllTermDocs::doc() {
    return _doc;
}

int32_t AbstractAllTermDocs::freq() {
    return 1;
}

bool AbstractAllTermDocs::next() {
    return skipTo(_doc + 1);
}

int32_t AbstractAllTermDocs::read(Collection<int32_t> docs, Collection<int32_t> freqs) {
    int32_t length = docs.size();
    int32_t i = 0;
    while (i < length && _doc < maxDoc) {
        if (!isDeleted(_doc)) {
            docs[i] = _doc;
            freqs[i] = 1;
            ++i;
        }
        ++_doc;
    }
    return i;
}

bool AbstractAllTermDocs::skipTo(int32_t target) {
    _doc = target;
    while (_doc < maxDoc) {
        if (!isDeleted(_doc)) {
            return true;
        }
        ++_doc;
    }
    return false;
}

void AbstractAllTermDocs::close() {
}

}
