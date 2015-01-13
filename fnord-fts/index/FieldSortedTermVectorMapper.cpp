/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "FieldSortedTermVectorMapper.h"
#include "TermVectorEntry.h"

namespace Lucene {

FieldSortedTermVectorMapper::FieldSortedTermVectorMapper(TermVectorEntryComparator comparator)
    : TermVectorMapper(false, false) {
    this->fieldToTerms = MapStringCollectionTermVectorEntry::newInstance();
    this->comparator = comparator;
}

FieldSortedTermVectorMapper::FieldSortedTermVectorMapper(bool ignoringPositions, bool ignoringOffsets, TermVectorEntryComparator comparator)
    : TermVectorMapper(ignoringPositions, ignoringOffsets) {
    this->fieldToTerms = MapStringCollectionTermVectorEntry::newInstance();
    this->comparator = comparator;
}

FieldSortedTermVectorMapper::~FieldSortedTermVectorMapper() {
}

void FieldSortedTermVectorMapper::map(const String& term, int32_t frequency, Collection<TermVectorOffsetInfoPtr> offsets, Collection<int32_t> positions) {
    TermVectorEntryPtr entry(newLucene<TermVectorEntry>(currentField, term, frequency, offsets, positions));
    if (!currentSet.contains_if(luceneEqualTo<TermVectorEntryPtr>(entry))) {
        currentSet.insert(std::upper_bound(currentSet.begin(), currentSet.end(), entry, comparator), entry);
    }
}

void FieldSortedTermVectorMapper::setExpectations(const String& field, int32_t numTerms, bool storeOffsets, bool storePositions) {
    currentSet = Collection<TermVectorEntryPtr>::newInstance();
    currentField = field;
    fieldToTerms.put(field, currentSet);
}

MapStringCollectionTermVectorEntry FieldSortedTermVectorMapper::getFieldToTerms() {
    return fieldToTerms;
}

TermVectorEntryComparator FieldSortedTermVectorMapper::getComparator() {
    return comparator;
}

}
