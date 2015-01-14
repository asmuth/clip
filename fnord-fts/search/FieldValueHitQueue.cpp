/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "fnord-fts/fts.h"
#include "fnord-fts/search/FieldValueHitQueue.h"
#include "fnord-fts/search/_FieldValueHitQueue.h"
#include "fnord-fts/search/FieldComparator.h"
#include "fnord-fts/search/FieldDoc.h"
#include "fnord-fts/search/SortField.h"

namespace Lucene {

FieldValueHitQueue::FieldValueHitQueue(Collection<SortFieldPtr> fields, int32_t size) : HitQueueBase(size) {
    // When we get here, fields.size() is guaranteed to be > 0, therefore no need to check it again.

    // All these are required by this class's API - need to return arrays.  Therefore even in the case
    // of a single comparator, create an array anyway.
    this->fields = fields;
    int32_t numComparators = fields.size();
    comparators = Collection<FieldComparatorPtr>::newInstance(numComparators);
    reverseMul = Collection<int32_t>::newInstance(numComparators);
}

FieldValueHitQueue::~FieldValueHitQueue() {
}

FieldValueHitQueuePtr FieldValueHitQueue::create(Collection<SortFieldPtr> fields, int32_t size) {
    if (fields.empty()) {
        boost::throw_exception(IllegalArgumentException(L"Sort must contain at least one field"));
    }

    if (fields.size() == 1) {
        return newLucene<OneComparatorFieldValueHitQueue>(fields, size);
    } else {
        return newLucene<MultiComparatorsFieldValueHitQueue>(fields, size);
    }
}

Collection<FieldComparatorPtr> FieldValueHitQueue::getComparators() {
    return comparators;
}

Collection<int32_t> FieldValueHitQueue::getReverseMul() {
    return reverseMul;
}

FieldDocPtr FieldValueHitQueue::fillFields(const FieldValueHitQueueEntryPtr& entry) {
    int32_t n = comparators.size();
    Collection<ComparableValue> fields(Collection<ComparableValue>::newInstance(n));
    for (int32_t i = 0; i < n; ++i) {
        fields[i] = comparators[i]->value(entry->slot);
    }
    return newLucene<FieldDoc>(entry->doc, entry->score, fields);
}

Collection<SortFieldPtr> FieldValueHitQueue::getFields() {
    return fields;
}

FieldValueHitQueueEntry::FieldValueHitQueueEntry(int32_t slot, int32_t doc, double score) : ScoreDoc(doc, score) {
    this->slot = slot;
}

FieldValueHitQueueEntry::~FieldValueHitQueueEntry() {
}

String FieldValueHitQueueEntry::toString() {
    StringStream buffer;
    buffer << L"slot:" << slot << L" " << ScoreDoc::toString();
    return buffer.str();
}

OneComparatorFieldValueHitQueue::OneComparatorFieldValueHitQueue(Collection<SortFieldPtr> fields, int32_t size) : FieldValueHitQueue(fields, size) {
    if (fields.empty()) {
        boost::throw_exception(IllegalArgumentException(L"Sort must contain at least one field"));
    }

    SortFieldPtr field(fields[0]);
    comparator = field->getComparator(size, 0);
    oneReverseMul = field->reverse ? -1 : 1;

    comparators[0] = comparator;
    reverseMul[0] = oneReverseMul;
}

OneComparatorFieldValueHitQueue::~OneComparatorFieldValueHitQueue() {
}

bool OneComparatorFieldValueHitQueue::lessThan(const ScoreDocPtr& first, const ScoreDocPtr& second) {
    FieldValueHitQueueEntryPtr firstEntry(std::static_pointer_cast<FieldValueHitQueueEntry>(first));
    FieldValueHitQueueEntryPtr secondEntry(std::static_pointer_cast<FieldValueHitQueueEntry>(second));

    BOOST_ASSERT(firstEntry != secondEntry);
    BOOST_ASSERT(firstEntry->slot != secondEntry->slot);

    int32_t c = oneReverseMul * comparator->compare(firstEntry->slot, secondEntry->slot);

    // avoid random sort order that could lead to duplicates
    return c != 0 ? (c > 0) : (firstEntry->doc > secondEntry->doc);
}

MultiComparatorsFieldValueHitQueue::MultiComparatorsFieldValueHitQueue(Collection<SortFieldPtr> fields, int32_t size) : FieldValueHitQueue(fields, size) {
    int32_t numComparators = comparators.size();
    for (int32_t i = 0; i < numComparators; ++i) {
        SortFieldPtr field(fields[i]);
        reverseMul[i] = field->reverse ? -1 : 1;
        comparators[i] = field->getComparator(size, i);
    }
}

MultiComparatorsFieldValueHitQueue::~MultiComparatorsFieldValueHitQueue() {
}

bool MultiComparatorsFieldValueHitQueue::lessThan(const ScoreDocPtr& first, const ScoreDocPtr& second) {
    FieldValueHitQueueEntryPtr firstEntry(std::static_pointer_cast<FieldValueHitQueueEntry>(first));
    FieldValueHitQueueEntryPtr secondEntry(std::static_pointer_cast<FieldValueHitQueueEntry>(second));

    BOOST_ASSERT(firstEntry != secondEntry);
    BOOST_ASSERT(firstEntry->slot != secondEntry->slot);

    int32_t numComparators = comparators.size();
    for (int32_t i = 0; i < numComparators; ++i) {
        int32_t c = reverseMul[i] * comparators[i]->compare(firstEntry->slot, secondEntry->slot);
        if (c != 0) {
            return (c > 0);    // Short circuit
        }
    }

    // avoid random sort order that could lead to duplicates
    return (firstEntry->doc > secondEntry->doc);
}

}
