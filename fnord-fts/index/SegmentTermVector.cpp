/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "SegmentTermVector.h"

namespace Lucene {

SegmentTermVector::SegmentTermVector(const String& field, Collection<String> terms, Collection<int32_t> termFreqs) {
    this->field = field;
    this->terms = terms;
    this->termFreqs = termFreqs;
}

SegmentTermVector::~SegmentTermVector() {
}

String SegmentTermVector::getField() {
    return field;
}

String SegmentTermVector::toString() {
    StringStream segTermVector;
    segTermVector << L"{" << field;
    if (terms) {
        for (int32_t i = 0; i < terms.size(); ++i) {
            if (i > 0) {
                segTermVector << L", ";
            }
            segTermVector << terms[i] << L"/" << termFreqs[i];
        }
    }
    segTermVector << L"}";
    return segTermVector.str();
}

int32_t SegmentTermVector::size() {
    return terms ? terms.size() : 0;
}

Collection<String> SegmentTermVector::getTerms() {
    return terms;
}

Collection<int32_t> SegmentTermVector::getTermFrequencies() {
    return termFreqs;
}

int32_t SegmentTermVector::indexOf(const String& term) {
    if (!terms) {
        return -1;
    }
    Collection<String>::iterator search = std::lower_bound(terms.begin(), terms.end(), term);
    return (search == terms.end() || term < *search) ? -1 : std::distance(terms.begin(), search);
}

Collection<int32_t> SegmentTermVector::indexesOf(Collection<String> termNumbers, int32_t start, int32_t length) {
    Collection<int32_t> res(Collection<int32_t>::newInstance(length));
    for (int32_t i = 0; i < length; ++i) {
        res[i] = indexOf(termNumbers[start + i]);
    }
    return res;
}

}
