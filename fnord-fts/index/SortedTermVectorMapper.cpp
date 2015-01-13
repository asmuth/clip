/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "SortedTermVectorMapper.h"
#include "TermVectorEntry.h"

namespace Lucene {

const wchar_t* SortedTermVectorMapper::ALL = L"_ALL_";

SortedTermVectorMapper::SortedTermVectorMapper(TermVectorEntryComparator comparator) : TermVectorMapper(false, false) {
    this->storeOffsets = false;
    this->storePositions = false;
    this->comparator = comparator;
    this->currentSet = Collection<TermVectorEntryPtr>::newInstance();
    this->termToTVE = MapStringTermVectorEntry::newInstance();
}

SortedTermVectorMapper::SortedTermVectorMapper(bool ignoringPositions, bool ignoringOffsets, TermVectorEntryComparator comparator)
    : TermVectorMapper(ignoringPositions, ignoringPositions) {
    this->storeOffsets = false;
    this->storePositions = false;
    this->comparator = comparator;
    this->currentSet = Collection<TermVectorEntryPtr>::newInstance();
    this->termToTVE = MapStringTermVectorEntry::newInstance();
}

SortedTermVectorMapper::~SortedTermVectorMapper() {
}

void SortedTermVectorMapper::map(const String& term, int32_t frequency, Collection<TermVectorOffsetInfoPtr> offsets, Collection<int32_t> positions) {
    // We need to combine any previous mentions of the term
    TermVectorEntryPtr entry(termToTVE.get(term));
    if (!entry) {
        entry = newLucene<TermVectorEntry>(ALL, term, frequency, storeOffsets ? offsets : Collection<TermVectorOffsetInfoPtr>(), storePositions ? positions : Collection<int32_t>());
        termToTVE.put(term, entry);

        if (!currentSet.contains_if(luceneEqualTo<TermVectorEntryPtr>(entry))) {
            currentSet.insert(std::upper_bound(currentSet.begin(), currentSet.end(), entry, comparator), entry);
        }
    } else {
        entry->setFrequency(entry->getFrequency() + frequency);
        if (storeOffsets) {
            Collection<TermVectorOffsetInfoPtr> existingOffsets(entry->getOffsets());
            // A few diff. cases here:  offsets is null, existing offsets is null, both are null, same for positions
            if (existingOffsets && offsets && !offsets.empty()) {
                // copy over the existing offsets
                Collection<TermVectorOffsetInfoPtr> newOffsets(Collection<TermVectorOffsetInfoPtr>::newInstance(existingOffsets.begin(), existingOffsets.end()));
                newOffsets.addAll(offsets.begin(), offsets.end());
                entry->setOffsets(newOffsets);
            } else if (!existingOffsets && offsets && !offsets.empty()) {
                entry->setOffsets(offsets);
            }
            // else leave it alone
        }
        if (storePositions) {
            Collection<int32_t> existingPositions(entry->getPositions());
            if (existingPositions && positions && !positions.empty()) {
                Collection<int32_t> newPositions(existingPositions);
                newPositions.addAll(positions.begin(), positions.end());
                entry->setPositions(newPositions);
            } else if (!existingPositions && positions && !positions.empty()) {
                entry->setPositions(positions);
            }
            // else leave it alone
        }
    }
}

void SortedTermVectorMapper::setExpectations(const String& field, int32_t numTerms, bool storeOffsets, bool storePositions) {
    this->storeOffsets = storeOffsets;
    this->storePositions = storePositions;
}

Collection<TermVectorEntryPtr> SortedTermVectorMapper::getTermVectorEntrySet() {
    return currentSet;
}

}
