/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef FIELDCOMPARATOR_H
#define FIELDCOMPARATOR_H

#include "LuceneObject.h"

namespace Lucene {

/// A FieldComparator compares hits so as to determine their sort order when collecting the top results with
/// {@link TopFieldCollector}.  The concrete public FieldComparator classes here correspond to the SortField types.
///
/// This API is designed to achieve high performance sorting, by exposing a tight interaction with {@link
/// FieldValueHitQueue} as it visits hits.  Whenever a hit is competitive, it's enrolled into a virtual slot,
/// which is an int ranging from 0 to numHits-1.  The {@link FieldComparator} is made aware of segment transitions
/// during searching in case any internal state it's tracking needs to be recomputed during these transitions.
///
/// A comparator must define these functions:
/// <ul>
/// <li> {@link #compare} Compare a hit at 'slot a' with hit 'slot b'.
///
/// <li> {@link #setBottom} This method is called by {@link FieldValueHitQueue} to notify the FieldComparator of
/// the current weakest ("bottom") slot.  Note that this slot may not hold the weakest value according to your
/// comparator, in cases where your comparator is not the primary one (ie, is only used to break ties from the
/// comparators before it).
///
/// <li> {@link #compareBottom} Compare a new hit (docID) against the "weakest" (bottom) entry in the queue.
///
/// <li> {@link #copy} Installs a new hit into the priority queue.  The {@link FieldValueHitQueue} calls this
/// method when a new hit is competitive.
///
/// <li> {@link #setNextReader} Invoked when the search is switching to the next segment.  You may need to update
/// internal state of the comparator, for example retrieving new values from the {@link FieldCache}.
///
/// <li> {@link #value} Return the sort value stored in the specified slot.  This is only called at the end of
/// the search, in order to populate {@link FieldDoc#fields} when returning the top results.
/// </ul>
class FieldComparator : public LuceneObject {
public:
    virtual ~FieldComparator();
    LUCENE_CLASS(FieldComparator);

public:
    /// Compare hit at slot1 with hit at slot2.
    /// @param slot1 first slot to compare
    /// @param slot2 second slot to compare
    /// @return any N < 0 if slot2's value is sorted after slot1, any N > 0 if the slot2's value is sorted
    /// before slot1 and 0 if they are equal
    virtual int32_t compare(int32_t slot1, int32_t slot2) = 0;

    /// Set the bottom slot, ie the "weakest" (sorted last) entry in the queue.  When {@link #compareBottom}
    /// is called, you should compare against this slot.  This will always be called before {@link #compareBottom}.
    /// @param slot the currently weakest (sorted last) slot in the queue
    virtual void setBottom(int32_t slot) = 0;

    /// Compare the bottom of the queue with doc.  This will only invoked after setBottom has been called.
    /// This should return the same result as {@link #compare(int,int)}} as if bottom were slot1 and the new
    /// document were slot 2.
    ///
    /// For a search that hits many results, this method will be the hotspot (invoked by far the most frequently).
    ///
    /// @param doc that was hit
    /// @return any N < 0 if the doc's value is sorted after the bottom entry (not competitive), any N > 0 if
    /// the doc's value is sorted before the bottom entry and 0 if they are equal.
    virtual int32_t compareBottom(int32_t doc) = 0;

    /// This method is called when a new hit is competitive.  You should copy any state associated with this
    /// document that will be required for future comparisons, into the specified slot.
    /// @param slot which slot to copy the hit to
    /// @param doc docID relative to current reader
    virtual void copy(int32_t slot, int32_t doc) = 0;

    /// Set a new Reader.  All doc correspond to the current Reader.
    ///
    /// @param reader current reader
    /// @param docBase docBase of this reader
    virtual void setNextReader(const IndexReaderPtr& reader, int32_t docBase) = 0;

    /// Sets the Scorer to use in case a document's score is needed.
    /// @param scorer Scorer instance that you should use to obtain the current hit's score, if necessary.
    virtual void setScorer(const ScorerPtr& scorer);

    /// Return the actual value in the slot.
    /// @param slot the value
    /// @return value in this slot upgraded to ComparableValue
    virtual ComparableValue value(int32_t slot) = 0;
};

template <typename TYPE>
class NumericComparator : public FieldComparator {
public:
    NumericComparator(int32_t numHits, const String& field = EmptyString) {
        this->values = Collection<TYPE>::newInstance(numHits);
        this->field = field;
        this->bottom = 0;
    }

    virtual ~NumericComparator() {
    }

protected:
    Collection<TYPE> values;
    Collection<TYPE> currentReaderValues;
    String field;
    TYPE bottom;

public:
    virtual int32_t compare(int32_t slot1, int32_t slot2) {
        return (int32_t)(values[slot1] - values[slot2]);
    }

    virtual int32_t compareBottom(int32_t doc) {
        return (int32_t)(bottom - currentReaderValues[doc]);
    }

    virtual void copy(int32_t slot, int32_t doc) {
        values[slot] = currentReaderValues[doc];
    }

    virtual void setBottom(int32_t slot) {
        bottom = values[slot];
    }

    virtual ComparableValue value(int32_t slot) {
        return ComparableValue(values[slot]);
    }
};

/// Parses field's values as byte (using {@link FieldCache#getBytes} and sorts by ascending value.
class ByteComparator : public NumericComparator<uint8_t> {
public:
    ByteComparator(int32_t numHits, const String& field, const ParserPtr& parser);
    virtual ~ByteComparator();

    LUCENE_CLASS(ByteComparator);

protected:
    ByteParserPtr parser;

public:
    virtual void setNextReader(const IndexReaderPtr& reader, int32_t docBase);
};

/// Sorts by ascending docID
class DocComparator : public NumericComparator<int32_t> {
public:
    DocComparator(int32_t numHits);
    virtual ~DocComparator();

    LUCENE_CLASS(DocComparator);

protected:
    int32_t docBase;

public:
    virtual int32_t compareBottom(int32_t doc);
    virtual void copy(int32_t slot, int32_t doc);
    virtual void setNextReader(const IndexReaderPtr& reader, int32_t docBase);
};

/// Parses field's values as double (using {@link FieldCache#getDoubles} and sorts by ascending value
class DoubleComparator : public NumericComparator<double> {
public:
    DoubleComparator(int32_t numHits, const String& field, const ParserPtr& parser);
    virtual ~DoubleComparator();

    LUCENE_CLASS(DoubleComparator);

protected:
    DoubleParserPtr parser;

public:
    virtual int32_t compare(int32_t slot1, int32_t slot2);
    virtual int32_t compareBottom(int32_t doc);
    virtual void setNextReader(const IndexReaderPtr& reader, int32_t docBase);
};

/// Parses field's values as int (using {@link FieldCache#getInts} and sorts by ascending value
class IntComparator : public NumericComparator<int32_t> {
public:
    IntComparator(int32_t numHits, const String& field, const ParserPtr& parser);
    virtual ~IntComparator();

    LUCENE_CLASS(IntComparator);

protected:
    IntParserPtr parser;

public:
    virtual int32_t compare(int32_t slot1, int32_t slot2);
    virtual int32_t compareBottom(int32_t doc);
    virtual void setNextReader(const IndexReaderPtr& reader, int32_t docBase);
};

/// Parses field's values as long (using {@link FieldCache#getLongs} and sorts by ascending value
class LongComparator : public NumericComparator<int64_t> {
public:
    LongComparator(int32_t numHits, const String& field, const ParserPtr& parser);
    virtual ~LongComparator();

    LUCENE_CLASS(LongComparator);

protected:
    LongParserPtr parser;

public:
    virtual int32_t compare(int32_t slot1, int32_t slot2);
    virtual int32_t compareBottom(int32_t doc);
    virtual void setNextReader(const IndexReaderPtr& reader, int32_t docBase);
};

/// Sorts by descending relevance.  NOTE: if you are sorting only by descending relevance and then secondarily
/// by ascending docID, performance is faster using {@link TopScoreDocCollector} directly (which {@link
/// IndexSearcher#search} uses when no {@link Sort} is specified).
class RelevanceComparator : public NumericComparator<double> {
public:
    RelevanceComparator(int32_t numHits);
    virtual ~RelevanceComparator();

    LUCENE_CLASS(RelevanceComparator);

protected:
    ScorerPtr scorer;

public:
    virtual int32_t compare(int32_t slot1, int32_t slot2);
    virtual int32_t compareBottom(int32_t doc);
    virtual void copy(int32_t slot, int32_t doc);
    virtual void setNextReader(const IndexReaderPtr& reader, int32_t docBase);
    virtual void setScorer(const ScorerPtr& scorer);
};

/// Sorts by a field's value using the Collator for a given Locale.
class StringComparatorLocale : public FieldComparator {
public:
    StringComparatorLocale(int32_t numHits, const String& field, const std::locale& locale);
    virtual ~StringComparatorLocale();

    LUCENE_CLASS(StringComparatorLocale);

protected:
    Collection<String> values;
    Collection<String> currentReaderValues;
    String field;
    CollatorPtr collator;
    String bottom;

public:
    virtual int32_t compare(int32_t slot1, int32_t slot2);
    virtual int32_t compareBottom(int32_t doc);
    virtual void copy(int32_t slot, int32_t doc);
    virtual void setNextReader(const IndexReaderPtr& reader, int32_t docBase);
    virtual void setBottom(int32_t slot);
    virtual ComparableValue value(int32_t slot);
};

/// Sorts by field's natural String sort order, using ordinals.  This is functionally equivalent to {@link
/// StringValComparator}, but it first resolves the string to their relative ordinal positions (using the
/// index returned by {@link FieldCache#getStringIndex}), and does most comparisons using the ordinals.
/// For medium to large results, this comparator will be much faster than {@link StringValComparator}.  For
/// very small result sets it may be slower.
class StringOrdValComparator : public FieldComparator {
public:
    StringOrdValComparator(int32_t numHits, const String& field, int32_t sortPos, bool reversed);
    virtual ~StringOrdValComparator();

    LUCENE_CLASS(StringOrdValComparator);

protected:
    Collection<int32_t> ords;
    Collection<String> values;
    Collection<int32_t> readerGen;

    int32_t currentReaderGen;
    Collection<String> lookup;
    Collection<int32_t> order;
    String field;

    int32_t bottomSlot;
    int32_t bottomOrd;
    String bottomValue;
    bool reversed;
    int32_t sortPos;

public:
    virtual int32_t compare(int32_t slot1, int32_t slot2);
    virtual int32_t compareBottom(int32_t doc);
    virtual void copy(int32_t slot, int32_t doc);
    virtual void setNextReader(const IndexReaderPtr& reader, int32_t docBase);
    virtual void setBottom(int32_t slot);
    virtual ComparableValue value(int32_t slot);
    virtual Collection<String> getValues();
    virtual int32_t getBottomSlot();
    virtual String getField();

protected:
    void convert(int32_t slot);
    int32_t binarySearch(Collection<String> lookup, const String& key, int32_t low, int32_t high);
};

/// Sorts by field's natural String sort order.  All comparisons are done using String.compare, which is
/// slow for medium to large result sets but possibly very fast for very small results sets.
class StringValComparator : public FieldComparator {
public:
    StringValComparator(int32_t numHits, const String& field);
    virtual ~StringValComparator();

    LUCENE_CLASS(StringOrdValComparator);

protected:
    Collection<String> values;
    Collection<String> currentReaderValues;
    String field;
    String bottom;

public:
    virtual int32_t compare(int32_t slot1, int32_t slot2);
    virtual int32_t compareBottom(int32_t doc);
    virtual void copy(int32_t slot, int32_t doc);
    virtual void setNextReader(const IndexReaderPtr& reader, int32_t docBase);
    virtual void setBottom(int32_t slot);
    virtual ComparableValue value(int32_t slot);
};

}

#endif
