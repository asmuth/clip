/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "fnord-fts/LuceneInc.h"
#include "fnord-fts/FieldComparator.h"
#include "fnord-fts/FieldCache.h"
#include "fnord-fts/ScoreCachingWrappingScorer.h"
#include "fnord-fts/Collator.h"

namespace Lucene {

FieldComparator::~FieldComparator() {
}

void FieldComparator::setScorer(const ScorerPtr& scorer) {
    // Empty implementation since most comparators don't need the score.
    // This can be overridden by those that need it.
}

ByteComparator::ByteComparator(int32_t numHits, const String& field, const ParserPtr& parser) : NumericComparator<uint8_t>(numHits, field) {
    this->parser = std::static_pointer_cast<ByteParser>(parser);
}

ByteComparator::~ByteComparator() {
}

void ByteComparator::setNextReader(const IndexReaderPtr& reader, int32_t docBase) {
    currentReaderValues = FieldCache::DEFAULT()->getBytes(reader, field, parser);
}

DocComparator::DocComparator(int32_t numHits) : NumericComparator<int32_t>(numHits) {
    this->docBase = 0;
}

DocComparator::~DocComparator() {
}

int32_t DocComparator::compareBottom(int32_t doc) {
    // No overflow risk because docIDs are non-negative
    return (bottom - (docBase + doc));
}

void DocComparator::copy(int32_t slot, int32_t doc) {
    values[slot] = docBase + doc;
}

void DocComparator::setNextReader(const IndexReaderPtr& reader, int32_t docBase) {
    this->docBase = docBase;
}

DoubleComparator::DoubleComparator(int32_t numHits, const String& field, const ParserPtr& parser) : NumericComparator<double>(numHits, field) {
    this->parser = std::static_pointer_cast<DoubleParser>(parser);
}

DoubleComparator::~DoubleComparator() {
}

int32_t DoubleComparator::compare(int32_t slot1, int32_t slot2) {
    double v1 = values[slot1];
    double v2 = values[slot2];
    return v1 > v2 ? 1 : (v1 < v2 ? -1 : 0);
}

int32_t DoubleComparator::compareBottom(int32_t doc) {
    double v2 = currentReaderValues[doc];
    return bottom > v2 ? 1 : (bottom < v2 ? -1 : 0);
}

void DoubleComparator::setNextReader(const IndexReaderPtr& reader, int32_t docBase) {
    currentReaderValues = FieldCache::DEFAULT()->getDoubles(reader, field, parser);
}

IntComparator::IntComparator(int32_t numHits, const String& field, const ParserPtr& parser) : NumericComparator<int32_t>(numHits, field) {
    this->parser = std::static_pointer_cast<IntParser>(parser);
}

IntComparator::~IntComparator() {
}

int32_t IntComparator::compare(int32_t slot1, int32_t slot2) {
    int32_t v1 = values[slot1];
    int32_t v2 = values[slot2];
    return v1 > v2 ? 1 : (v1 < v2 ? -1 : 0);
}

int32_t IntComparator::compareBottom(int32_t doc) {
    int32_t v2 = currentReaderValues[doc];
    return bottom > v2 ? 1 : (bottom < v2 ? -1 : 0);
}

void IntComparator::setNextReader(const IndexReaderPtr& reader, int32_t docBase) {
    currentReaderValues = FieldCache::DEFAULT()->getInts(reader, field, parser);
}

LongComparator::LongComparator(int32_t numHits, const String& field, const ParserPtr& parser) : NumericComparator<int64_t>(numHits, field) {
    this->parser = std::static_pointer_cast<LongParser>(parser);
}

LongComparator::~LongComparator() {
}

int32_t LongComparator::compare(int32_t slot1, int32_t slot2) {
    int64_t v1 = values[slot1];
    int64_t v2 = values[slot2];
    return v1 > v2 ? 1 : (v1 < v2 ? -1 : 0);
}

int32_t LongComparator::compareBottom(int32_t doc) {
    int64_t v2 = currentReaderValues[doc];
    return bottom > v2 ? 1 : (bottom < v2 ? -1 : 0);
}

void LongComparator::setNextReader(const IndexReaderPtr& reader, int32_t docBase) {
    currentReaderValues = FieldCache::DEFAULT()->getLongs(reader, field, parser);
}

RelevanceComparator::RelevanceComparator(int32_t numHits) : NumericComparator<double>(numHits) {
}

RelevanceComparator::~RelevanceComparator() {
}

int32_t RelevanceComparator::compare(int32_t slot1, int32_t slot2) {
    double score1 = values[slot1];
    double score2 = values[slot2];
    return score1 > score2 ? -1 : (score1 < score2 ? 1 : 0);
}

int32_t RelevanceComparator::compareBottom(int32_t doc) {
    double score = scorer->score();
    return bottom > score ? -1 : (bottom < score ? 1 : 0);
}

void RelevanceComparator::copy(int32_t slot, int32_t doc) {
    values[slot] = scorer->score();
}

void RelevanceComparator::setNextReader(const IndexReaderPtr& reader, int32_t docBase) {
}

void RelevanceComparator::setScorer(const ScorerPtr& scorer) {
    this->scorer = newLucene<ScoreCachingWrappingScorer>(scorer);
}

StringComparatorLocale::StringComparatorLocale(int32_t numHits, const String& field, const std::locale& locale) : collator(newLucene<Collator>(locale)) {
    this->values = Collection<String>::newInstance(numHits);
    this->field = field;
}

StringComparatorLocale::~StringComparatorLocale() {
}

int32_t StringComparatorLocale::compare(int32_t slot1, int32_t slot2) {
    return collator->compare(values[slot1], values[slot2]);
}

int32_t StringComparatorLocale::compareBottom(int32_t doc) {
    return collator->compare(bottom, currentReaderValues[doc]);
}

void StringComparatorLocale::copy(int32_t slot, int32_t doc) {
    values[slot] = currentReaderValues[doc];
}

void StringComparatorLocale::setNextReader(const IndexReaderPtr& reader, int32_t docBase) {
    currentReaderValues = FieldCache::DEFAULT()->getStrings(reader, field);
}

void StringComparatorLocale::setBottom(int32_t slot) {
    bottom = values[slot];
}

ComparableValue StringComparatorLocale::value(int32_t slot) {
    return values[slot];
}

StringOrdValComparator::StringOrdValComparator(int32_t numHits, const String& field, int32_t sortPos, bool reversed) {
    this->ords = Collection<int32_t>::newInstance(numHits);
    this->values = Collection<String>::newInstance(numHits);
    this->readerGen = Collection<int32_t>::newInstance(numHits);
    this->sortPos = sortPos;
    this->reversed = reversed;
    this->field = field;
    this->currentReaderGen = -1;
    this->bottomSlot = -1;
    this->bottomOrd = 0;
}

StringOrdValComparator::~StringOrdValComparator() {
}

int32_t StringOrdValComparator::compare(int32_t slot1, int32_t slot2) {
    if (readerGen[slot1] == readerGen[slot2]) {
        int32_t cmp = ords[slot1] - ords[slot2];
        if (cmp != 0) {
            return cmp;
        }
    }
    return values[slot1].compare(values[slot2]);
}

int32_t StringOrdValComparator::compareBottom(int32_t doc) {
    BOOST_ASSERT(bottomSlot != -1);
    int32_t order = this->order[doc];
    int32_t cmp = bottomOrd - order;
    if (cmp != 0) {
        return cmp;
    }
    return bottomValue.compare(lookup[order]);
}

void StringOrdValComparator::convert(int32_t slot) {
    readerGen[slot] = currentReaderGen;
    int32_t index = 0;
    String value(values[slot]);
    if (value.empty()) {
        ords[slot] = 0;
        return;
    }

    if (sortPos == 0 && bottomSlot != -1 && bottomSlot != slot) {
        // Since we are the primary sort, the entries in the queue are bounded by bottomOrd
        BOOST_ASSERT(bottomOrd < lookup.size());
        if (reversed) {
            index = binarySearch(lookup, value, bottomOrd, lookup.size() - 1);
        } else {
            index = binarySearch(lookup, value, 0, bottomOrd);
        }
    } else {
        // Full binary search
        index = binarySearch(lookup, value, 0, lookup.size() - 1);
    }

    if (index < 0) {
        index = -index - 2;
    }

    ords[slot] = index;
}

int32_t StringOrdValComparator::binarySearch(Collection<String> lookup, const String& key, int32_t low, int32_t high) {
    Collection<String>::iterator search = std::lower_bound(lookup.begin() + low, lookup.begin() + high, key);
    int32_t keyPos = std::distance(lookup.begin(), search);
    return (search == lookup.end() || key < *search) ? -(keyPos + 1) : keyPos;
}

void StringOrdValComparator::copy(int32_t slot, int32_t doc) {
    int32_t ord = order[doc];
    ords[slot] = ord;
    BOOST_ASSERT(ord >= 0);
    values[slot] = lookup[ord];
    readerGen[slot] = currentReaderGen;
}

void StringOrdValComparator::setNextReader(const IndexReaderPtr& reader, int32_t docBase) {
    StringIndexPtr currentReaderValues(FieldCache::DEFAULT()->getStringIndex(reader, field));
    ++currentReaderGen;
    order = currentReaderValues->order;
    lookup = currentReaderValues->lookup;
    BOOST_ASSERT(!lookup.empty());
    if (bottomSlot != -1) {
        convert(bottomSlot);
        bottomOrd = ords[bottomSlot];
    }
}

void StringOrdValComparator::setBottom(int32_t slot) {
    bottomSlot = slot;
    if (readerGen[slot] != currentReaderGen) {
        convert(bottomSlot);
    }
    bottomOrd = ords[slot];
    BOOST_ASSERT(bottomOrd >= 0);
    BOOST_ASSERT(bottomOrd < lookup.size());
    bottomValue = values[slot];
}

ComparableValue StringOrdValComparator::value(int32_t slot) {
    return values[slot];
}

Collection<String> StringOrdValComparator::getValues() {
    return values;
}

int32_t StringOrdValComparator::getBottomSlot() {
    return bottomSlot;
}

String StringOrdValComparator::getField() {
    return field;
}

StringValComparator::StringValComparator(int32_t numHits, const String& field) {
    this->values = Collection<String>::newInstance(numHits);
    this->field = field;
}

StringValComparator::~StringValComparator() {
}

int32_t StringValComparator::compare(int32_t slot1, int32_t slot2) {
    return values[slot1].compare(values[slot2]);
}

int32_t StringValComparator::compareBottom(int32_t doc) {
    return bottom.compare(currentReaderValues[doc]);
}

void StringValComparator::copy(int32_t slot, int32_t doc) {
    values[slot] = currentReaderValues[doc];
}

void StringValComparator::setNextReader(const IndexReaderPtr& reader, int32_t docBase) {
    currentReaderValues = FieldCache::DEFAULT()->getStrings(reader, field);
}

void StringValComparator::setBottom(int32_t slot) {
    bottom = values[slot];
}

ComparableValue StringValComparator::value(int32_t slot) {
    return values[slot];
}

}
