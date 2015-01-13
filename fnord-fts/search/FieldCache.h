/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef FIELDCACHE_H
#define FIELDCACHE_H

#include <boost/any.hpp>
#include "fnord-fts/util/LuceneObject.h"

namespace Lucene {

/// Maintains caches of term values.
/// @see FieldCacheSanityChecker
class FieldCache {
public:
    virtual ~FieldCache();
    LUCENE_INTERFACE(FieldCache);

public:
    /// Specifies whether and how a field should be stored.
    enum CacheType {
        CACHE_BYTE = 1,
        CACHE_INT,
        CACHE_LONG,
        CACHE_DOUBLE,
        CACHE_STRING,
        CACHE_STRING_INDEX
    };

    /// Indicator for StringIndex values in the cache.
    /// NOTE: the value assigned to this constant must not be the same as any of those in SortField
    static const int32_t STRING_INDEX;

public:
    /// The cache used internally by sorting and range query classes.
    static FieldCachePtr DEFAULT();

    /// The default parser for byte values, which are encoded by StringUtils::toInt
    static ByteParserPtr DEFAULT_BYTE_PARSER();

    /// The default parser for int values, which are encoded by StringUtils::toInt
    static IntParserPtr DEFAULT_INT_PARSER();

    /// The default parser for int values, which are encoded by StringUtils::toLong
    static LongParserPtr DEFAULT_LONG_PARSER();

    /// The default parser for double values, which are encoded by StringUtils::toDouble
    static DoubleParserPtr DEFAULT_DOUBLE_PARSER();

    /// A parser instance for int values encoded by {@link NumericUtils#prefixCodedToInt(String)},
    /// eg. when indexed via {@link NumericField}/{@link NumericTokenStream}.
    static IntParserPtr NUMERIC_UTILS_INT_PARSER();

    /// A parser instance for long values encoded by {@link NumericUtils#prefixCodedToLong(String)},
    /// eg. when indexed via {@link NumericField}/{@link NumericTokenStream}.
    static LongParserPtr NUMERIC_UTILS_LONG_PARSER();

    /// A parser instance for double values encoded by {@link NumericUtils},
    /// eg. when indexed via {@link NumericField}/{@link NumericTokenStream}.
    static DoubleParserPtr NUMERIC_UTILS_DOUBLE_PARSER();

    /// Checks the internal cache for an appropriate entry, and if none are found, reads the terms in
    /// field as a single byte and returns an array of size reader.maxDoc() of the value each document
    /// has in the given field.
    /// @param reader Used to get field values.
    /// @param field Which field contains the single byte values.
    /// @return The values in the given field for each document.
    virtual Collection<uint8_t> getBytes(const IndexReaderPtr& reader, const String& field);

    /// Checks the internal cache for an appropriate entry, and if none are found, reads the terms in
    /// field as bytes and returns an array of size reader.maxDoc() of the value each document has in
    /// the given field.
    /// @param reader Used to get field values.
    /// @param field Which field contains the bytes.
    /// @param parser Computes byte for string values.
    /// @return The values in the given field for each document.
    virtual Collection<uint8_t> getBytes(const IndexReaderPtr& reader, const String& field, const ByteParserPtr& parser);

    /// Checks the internal cache for an appropriate entry, and if none are found, reads the terms in
    /// field as integers and returns an array of size reader.maxDoc() of the value each document has in
    /// the given field.
    /// @param reader Used to get field values.
    /// @param field Which field contains the integers.
    /// @return The values in the given field for each document.
    virtual Collection<int32_t> getInts(const IndexReaderPtr& reader, const String& field);

    /// Checks the internal cache for an appropriate entry, and if none are found, reads the terms in
    /// field as integers and returns an array of size reader.maxDoc() of the value each document has in
    /// the given field.
    /// @param reader Used to get field values.
    /// @param field Which field contains the integers.
    /// @param parser Computes integer for string values.
    /// @return The values in the given field for each document.
    virtual Collection<int32_t> getInts(const IndexReaderPtr& reader, const String& field, const IntParserPtr& parser);

    /// Checks the internal cache for an appropriate entry, and if none are found, reads the terms in
    /// field as longs and returns an array of size reader.maxDoc() of the value each document has in
    /// the given field.
    /// @param reader Used to get field values.
    /// @param field Which field contains the longs.
    /// @return The values in the given field for each document.
    virtual Collection<int64_t> getLongs(const IndexReaderPtr& reader, const String& field);

    /// Checks the internal cache for an appropriate entry, and if none are found, reads the terms in
    /// field as longs and returns an array of size reader.maxDoc() of the value each document has in
    /// the given field.
    /// @param reader Used to get field values.
    /// @param field Which field contains the longs.
    /// @param parser Computes long for string values.
    /// @return The values in the given field for each document.
    virtual Collection<int64_t> getLongs(const IndexReaderPtr& reader, const String& field, const LongParserPtr& parser);

    /// Checks the internal cache for an appropriate entry, and if none are found, reads the terms in
    /// field as integers and returns an array of size reader.maxDoc() of the value each document has in
    /// the given field.
    /// @param reader Used to get field values.
    /// @param field Which field contains the doubles.
    /// @return The values in the given field for each document.
    virtual Collection<double> getDoubles(const IndexReaderPtr& reader, const String& field);

    /// Checks the internal cache for an appropriate entry, and if none are found, reads the terms in
    /// field as doubles and returns an array of size reader.maxDoc() of the value each document has in
    /// the given field.
    /// @param reader Used to get field values.
    /// @param field Which field contains the doubles.
    /// @param parser Computes double for string values.
    /// @return The values in the given field for each document.
    virtual Collection<double> getDoubles(const IndexReaderPtr& reader, const String& field, const DoubleParserPtr& parser);

    /// Checks the internal cache for an appropriate entry, and if none are found, reads the term values in
    /// field and returns an array of size reader.maxDoc() containing the value each document has in
    /// the given field.
    /// @param reader Used to get field values.
    /// @param field Which field contains the strings.
    /// @return The values in the given field for each document.
    virtual Collection<String> getStrings(const IndexReaderPtr& reader, const String& field);

    /// Checks the internal cache for an appropriate entry, and if none are found reads the term values in
    /// field and returns an array of them in natural order, along with an array telling which element in
    /// the term array each document uses.
    /// @param reader Used to get field values.
    /// @param field Which field contains the strings.
    /// @return Array of terms and index into the array for each document.
    virtual StringIndexPtr getStringIndex(const IndexReaderPtr& reader, const String& field);

    /// Generates an array of CacheEntry objects representing all items currently in the FieldCache.
    virtual Collection<FieldCacheEntryPtr> getCacheEntries() = 0;

    /// Instructs the FieldCache to forcibly expunge all entries from the underlying caches.  This is intended
    /// only to be used for test methods as a way to ensure a known base state of the Cache.  It should not be
    /// relied on for "Cache maintenance" in general application code.
    virtual void purgeAllCaches() = 0;

    /// Drops all cache entries associated with this reader.  NOTE: this reader must precisely match the reader
    /// that the cache entry is keyed on. If you pass a top-level reader, it usually will have no effect as
    /// Lucene now caches at the segment reader level.
    virtual void purge(const IndexReaderPtr& r) = 0;

    /// If non-null, FieldCacheImpl will warn whenever entries are created that are not sane according to
    /// {@link FieldCacheSanityChecker}.
    virtual void setInfoStream(const InfoStreamPtr& stream);

    /// @see #setInfoStream
    virtual InfoStreamPtr getInfoStream();
};

class CreationPlaceholder : public LuceneObject {
public:
    virtual ~CreationPlaceholder();
    LUCENE_CLASS(CreationPlaceholder);

public:
    boost::any value;
};

/// Stores term text values and document ordering data.
class StringIndex : public LuceneObject {
public:
    StringIndex(Collection<int32_t> values, Collection<String> lookup);
    virtual ~StringIndex();

    LUCENE_CLASS(StringIndex);

public:
    /// All the term values, in natural order.
    Collection<String> lookup;

    /// For each document, an index into the lookup array.
    Collection<int32_t> order;

public:
    int32_t binarySearchLookup(const String& key);
};

/// Marker interface as super-interface to all parsers.  It is used to specify a custom parser to {@link
/// SortField#SortField(String, Parser)}.
class Parser : public LuceneObject {
public:
    virtual ~Parser();
    LUCENE_CLASS(Parser);
};

/// Interface to parse bytes from document fields.
/// @see FieldCache#getBytes(IndexReaderPtr, String, ByteParserPtr)
class ByteParser : public Parser {
public:
    virtual ~ByteParser();
    LUCENE_CLASS(ByteParser);

public:
    /// Return a single Byte representation of this field's value.
    virtual uint8_t parseByte(const String& string);
};

/// Interface to parse ints from document fields.
/// @see FieldCache#getInts(IndexReaderPtr, String, IntParserPtr)
class IntParser : public Parser {
public:
    virtual ~IntParser();
    LUCENE_CLASS(IntParser);

public:
    /// Return a integer representation of this field's value.
    virtual int32_t parseInt(const String& string);
};

/// Interface to parse longs from document fields.
/// @see FieldCache#getLongs(IndexReaderPtr, String, LongParserPtr)
class LongParser : public Parser {
public:
    virtual ~LongParser();
    LUCENE_CLASS(LongParser);

public:
    /// Return a long representation of this field's value.
    virtual int64_t parseLong(const String& string);
};

/// Interface to parse doubles from document fields.
/// @see FieldCache#getDoubles(IndexReaderPtr, String, DoubleParserPtr)
class DoubleParser : public Parser {
public:
    virtual ~DoubleParser();
    LUCENE_CLASS(DoubleParser);

public:
    /// Return a double representation of this field's value.
    virtual double parseDouble(const String& string);
};

/// A unique Identifier/Description for each item in the FieldCache.  Can be useful for logging/debugging.
class FieldCacheEntry : public LuceneObject {
public:
    virtual ~FieldCacheEntry();
    LUCENE_CLASS(FieldCacheEntry);

public:
    virtual LuceneObjectPtr getReaderKey() = 0;
    virtual String getFieldName() = 0;
    virtual int32_t getCacheType() = 0;
    virtual boost::any getCustom() = 0;
    virtual boost::any getValue() = 0;

    virtual String toString();
};

}

#endif
