/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef SORTFIELD_H
#define SORTFIELD_H

#include "fnord-fts/util/LuceneObject.h"

namespace fnord {
namespace fts {

/// Stores information about how to sort documents by terms in an individual field.  Fields must be indexed
/// in order to sort by them.
class SortField : public LuceneObject {
public:
    /// Creates a sort by terms in the given field with the type of term values explicitly given.
    /// @param field Name of field to sort by.  Can be null if type is SCORE or DOC.
    /// @param type Type of values in the terms.
    /// @param reverse True if natural order should be reversed.
    SortField(const String& field, int32_t type, bool reverse = false);

    /// Creates a sort, possibly in reverse, by terms in the given field, parsed to numeric values using a
    /// custom {@link Parser}.
    /// @param field  Name of field to sort by
    /// @param parser Instance of a {@link Parser}, which must subclass one of the existing numeric parsers from
    /// {@link FieldCache}. Sort type is inferred by testing which numeric parser the parser subclasses.
    /// @param reverse True if natural order should be reversed.
    SortField(const String& field, const ParserPtr& parser, bool reverse = false);

    /// Creates a sort, possibly in reverse, by terms in the given field sorted according to the given locale.
    /// @param field  Name of field to sort by, cannot be null.
    /// @param locale Locale of values in the field.
    /// @param reverse True if natural order should be reversed.
    SortField(const String& field, const std::locale& locale, bool reverse = false);

    /// Creates a sort, possibly in reverse, with a custom comparison function.
    /// @param field Name of field to sort by; cannot be null.
    /// @param comparator Returns a comparator for sorting hits.
    /// @param reverse True if natural order should be reversed.
    SortField(const String& field, const FieldComparatorSourcePtr& comparator, bool reverse = false);

    virtual ~SortField();

    LUCENE_CLASS(SortField);

public:
    /// Sort by document score (relevancy).  Sort values are Double and higher values are at the front.
    static const int32_t SCORE;

    /// Sort by document number (index order).  Sort values are Integer and lower values are at the front.
    static const int32_t DOC;

    /// Sort using term values as Strings.  Sort values are String and lower values are at the front.
    static const int32_t STRING;

    /// Sort using term values as Integers.  Sort values are Integer and lower values are at the front.
    static const int32_t INT;

    /// Sort using term values as Floats.  Sort values are Float and lower values are at the front.
    static const int32_t FLOAT;

    /// Sort using term values as Longs.  Sort values are Long and lower values are at the front.
    static const int32_t LONG;

    /// Sort using term values as Doubles.  Sort values are Double and lower values are at the front.
    static const int32_t DOUBLE;

    /// Sort using term values as Shorts.  Sort values are Short and lower values are at the front.
    static const int32_t SHORT;

    /// Sort using a custom Comparator.  Sort values are any ComparableValue and sorting is done according
    /// to natural order.
    static const int32_t CUSTOM;

    /// Sort using term values as Bytes.  Sort values are Byte and lower values are at the front.
    static const int32_t BYTE;

    /// Sort using term values as Strings, but comparing by value (using String::compare) for all comparisons.
    /// This is typically slower than {@link #STRING}, which uses ordinals to do the sorting.
    static const int32_t STRING_VAL;

INTERNAL:
    bool reverse; // defaults to natural order

    String field;
    int32_t type; // defaults to determining type dynamically
    localePtr locale; // defaults to "natural order" (no Locale)
    ParserPtr parser;

private:
    /// Used for CUSTOM sort
    FieldComparatorSourcePtr comparatorSource;

public:
    /// Represents sorting by document score (relevancy).
    static SortFieldPtr FIELD_SCORE();

    /// Represents sorting by document number (index order).
    static SortFieldPtr FIELD_DOC();

    /// Returns the name of the field.  Could return null if the sort is by SCORE or DOC.
    /// @return Name of field, possibly null.
    String getField();

    /// Returns the type of contents in the field.
    /// @return One of the constants SCORE, DOC, STRING, INT or DOUBLE.
    int32_t getType();

    /// Returns the Locale by which term values are interpreted.
    localePtr getLocale();

    /// Returns the instance of a {@link FieldCache} parser that fits to the given sort type.  May return null
    /// if no parser was specified. Sorting is using the default parser then.
    /// @return An instance of a parser, or null.
    ParserPtr getParser();

    /// Returns whether the sort should be reversed.
    /// @return True if natural order should be reversed.
    bool getReverse();

    /// Returns the {@link FieldComparatorSource} used for custom sorting
    FieldComparatorSourcePtr getComparatorSource();

    virtual String toString();

    /// Returns true if other is equal to this.  If a {@link FieldComparatorSource} or {@link Parser} was provided,
    /// it must properly implement equals (unless a singleton is always used).
    virtual bool equals(const LuceneObjectPtr& other);

    virtual int32_t hashCode();

    /// Returns the {@link FieldComparator} to use for sorting.
    /// @param numHits number of top hits the queue will store
    /// @param sortPos position of this SortField within {@link Sort}.  The comparator is primary if sortPos == 0,
    /// secondary if sortPos == 1, etc.  Some comparators can optimize themselves when they are the primary sort.
    /// @return {@link FieldComparator} to use when sorting
    FieldComparatorPtr getComparator(int32_t numHits, int32_t sortPos);

protected:
    /// Sets field and type, and ensures field is not NULL unless type is SCORE or DOC
    void initFieldType(const String& field, int32_t type);
};

}

}
#endif
