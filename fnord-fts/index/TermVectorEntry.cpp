/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "TermVectorEntry.h"
#include "StringUtils.h"

namespace Lucene {

TermVectorEntry::TermVectorEntry(const String& field, const String& term, int32_t frequency,
                                 Collection<TermVectorOffsetInfoPtr> offsets, Collection<int32_t> positions) {
    this->field = field;
    this->term = term;
    this->frequency = frequency;
    this->offsets = offsets;
    this->positions = positions;
}

TermVectorEntry::~TermVectorEntry() {
}

String TermVectorEntry::getField() {
    return field;
}

int32_t TermVectorEntry::getFrequency() {
    return frequency;
}

Collection<TermVectorOffsetInfoPtr> TermVectorEntry::getOffsets() {
    return offsets;
}

Collection<int32_t> TermVectorEntry::getPositions() {
    return positions;
}

String TermVectorEntry::getTerm() {
    return term;
}

void TermVectorEntry::setFrequency(int32_t frequency) {
    this->frequency = frequency;
}

void TermVectorEntry::setOffsets(Collection<TermVectorOffsetInfoPtr> offsets) {
    this->offsets = offsets;
}

void TermVectorEntry::setPositions(Collection<int32_t> positions) {
    this->positions = positions;
}

bool TermVectorEntry::equals(const LuceneObjectPtr& other) {
    if (LuceneObject::equals(other)) {
        return true;
    }

    TermVectorEntryPtr otherTermVectorEntry(boost::dynamic_pointer_cast<TermVectorEntry>(other));
    if (otherTermVectorEntry) {
        return (term == otherTermVectorEntry->term);
    }

    return false;
}

int32_t TermVectorEntry::hashCode() {
    return StringUtils::hashCode(term);
}

String TermVectorEntry::toString() {
    StringStream buffer;
    buffer << L"TermVectorEntry{field='" << field;
    buffer << L"\', term='" << term;
    buffer << L"\', frequency=" << frequency << L"}";
    return buffer.str();
}

}
