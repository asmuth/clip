/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef NUMERICFIELD_H
#define NUMERICFIELD_H

#include "Field.h"

namespace Lucene {

/// This class provides a {@link Field} that enables indexing of numeric values for efficient range filtering and
/// sorting.  The native types int32_t, int64_t and double are directly supported.  However, any value that can be
/// converted into these native types can also be indexed.  For example, date/time values represented by a {@link
/// Date} can be translated into a int64_t value.  If you don't need millisecond precision, you can quantize the
/// value, either by dividing the result or using the separate getters (for year, month, etc.) to construct an int32_t
/// or int64_t value.
///
/// To perform range querying or filtering against a NumericField, use {@link NumericRangeQuery} or {@link
/// NumericRangeFilter}.  To sort according to a NumericField, use the normal numeric sort types, eg {@link
/// SortField#INT}. NumericField values can also be loaded directly from {@link FieldCache}.
///
/// By default, a NumericField's value is not stored but is indexed for range filtering and sorting.  You can use the
/// {@link #NumericField(String,Field.Store,boolean)} constructor if you need to change these defaults.
///
/// You may add the same field name as a NumericField to the same document more than once.  Range querying and
/// filtering will be the logical OR of all values; so a range query will hit all documents that have at least one
/// value in the range.  However sort behavior is not defined.  If you need to sort, you should separately index a
/// single-valued NumericField.
///
/// A NumericField will consume somewhat more disk space in the index than an ordinary single-valued field.  However,
/// for a typical index that includes substantial textual content per document, this increase will likely be in the
/// noise.
///
/// Within Lucene, each numeric value is indexed as a trie structure, where each term is logically assigned to larger
/// and larger pre-defined brackets (which are simply lower-precision representations of the value).  The step size
/// between each successive bracket is called the precisionStep, measured in bits.  Smaller precisionStep values
/// result in larger number of brackets, which consumes more disk space in the index but may result in faster range
/// search performance.  The default value 4 was selected for a reasonable trade off of disk space consumption versus
/// performance.  You can use the expert constructor {@link #NumericField(String,int,Field.Store,boolean)} if you'd
/// like to change the value.  Note that you must also specify a congruent value when creating {@link NumericRangeQuery}
/// or {@link NumericRangeFilter}.  For low cardinality fields larger precision steps are good.  If the cardinality
/// is < 100, it is fair to use {@link INT_MAX}, which produces one term per value.
///
/// For more information on the internals of numeric trie indexing, including the precisionStep configuration, see
/// {@link NumericRangeQuery}.  The format of indexed values is described in {@link NumericUtils}.
///
/// If you only need to sort by numeric value, and never run range querying/filtering, you can index using a
/// precisionStep of {@link MAX_INT}.  This will minimize disk space consumed.
///
/// More advanced users can instead use {@link NumericTokenStream} directly, when indexing numbers.  This class is a
/// wrapper around this token stream type for easier, more intuitive usage.
///
/// NOTE: This class is only used during indexing. When retrieving the stored field value from a {@link Document}
/// instance after search, you will get a conventional {@link Fieldable} instance where the numeric values are
/// returned as strings (according to toString(value) of the used data type).
class NumericField : public AbstractField {
public:
    /// Creates a field for numeric values using the default precisionStep {@link NumericUtils#PRECISION_STEP_DEFAULT}
    /// (4). The instance is not yet initialized with a numeric value, before indexing a document containing this field,
    /// set a value using the various set???Value() methods.
    /// This constructor creates an indexed, but not stored field.
    /// @param name the field name
    NumericField(const String& name);

    /// Creates a field for numeric values using the default precisionStep {@link NumericUtils#PRECISION_STEP_DEFAULT}
    /// (4). The instance is not yet initialized with a numeric value, before indexing a document containing this field,
    /// set a value using the various set???Value() methods.
    /// This constructor creates an indexed, but not stored field.
    /// @param name the field name
    /// @param store if the field should be stored in plain text form (according to toString(value) of the used
    /// data type)
    /// @param index if the field should be indexed using {@link NumericTokenStream}
    NumericField(const String& name, Field::Store store, bool index);

    /// Creates a field for numeric values with the specified precisionStep. The instance is not yet initialized with
    /// a numeric value, before indexing a document containing this field, set a value using the various set???Value()
    /// methods. This constructor creates an indexed, but not stored field.
    /// @param name the field name
    /// @param precisionStep the used precision step
    NumericField(const String& name, int32_t precisionStep);

    /// Creates a field for numeric values with the specified precisionStep. The instance is not yet initialized with
    /// a numeric value, before indexing a document containing this field, set a value using the various set???Value()
    /// methods. This constructor creates an indexed, but not stored field.
    /// @param name the field name
    /// @param precisionStep the used precision step
    /// @param store if the field should be stored in plain text form (according to toString(value) of the used
    /// data type)
    /// @param index if the field should be indexed using {@link NumericTokenStream}
    NumericField(const String& name, int32_t precisionStep, Field::Store store, bool index);

    virtual ~NumericField();

    LUCENE_CLASS(NumericField);

protected:
    NumericTokenStreamPtr tokenStream;

public:
    /// Returns a {@link NumericTokenStream} for indexing the numeric value.
    virtual TokenStreamPtr tokenStreamValue();

    /// Returns always null for numeric fields
    virtual ByteArray getBinaryValue(ByteArray result);

    /// Returns always null for numeric fields
    virtual ReaderPtr readerValue();

    /// Returns the numeric value as a string (how it is stored, when {@link Field.Store#YES} is chosen).
    virtual String stringValue();

    /// Returns the current numeric value.
    virtual int64_t getNumericValue();

    /// Initializes the field with the supplied long value.
    /// @param value the numeric value
    virtual NumericFieldPtr setLongValue(int64_t value);

    /// Initializes the field with the supplied int value.
    /// @param value the numeric value
    virtual NumericFieldPtr setIntValue(int32_t value);

    /// Initializes the field with the supplied double value.
    /// @param value the numeric value
    virtual NumericFieldPtr setDoubleValue(double value);
};

}

#endif
