/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef FIELDCACHEIMPL_H
#define FIELDCACHEIMPL_H

#include "FieldCache.h"

namespace Lucene {

/// The default cache implementation, storing all values in memory.  A WeakHashMap is used for storage.
class FieldCacheImpl : public FieldCache, public LuceneObject {
public:
    FieldCacheImpl();
    virtual ~FieldCacheImpl();

    LUCENE_CLASS(FieldCacheImpl);

protected:
    MapStringCache caches;
    InfoStreamPtr infoStream;

public:
    virtual void initialize();
    virtual void purgeAllCaches();
    virtual void purge(const IndexReaderPtr& r);
    virtual Collection<FieldCacheEntryPtr> getCacheEntries();

    virtual Collection<uint8_t> getBytes(const IndexReaderPtr& reader, const String& field);
    virtual Collection<uint8_t> getBytes(const IndexReaderPtr& reader, const String& field, const ByteParserPtr& parser);

    virtual Collection<int32_t> getInts(const IndexReaderPtr& reader, const String& field);
    virtual Collection<int32_t> getInts(const IndexReaderPtr& reader, const String& field, const IntParserPtr& parser);

    virtual Collection<int64_t> getLongs(const IndexReaderPtr& reader, const String& field);
    virtual Collection<int64_t> getLongs(const IndexReaderPtr& reader, const String& field, const LongParserPtr& parser);

    virtual Collection<double> getDoubles(const IndexReaderPtr& reader, const String& field);
    virtual Collection<double> getDoubles(const IndexReaderPtr& reader, const String& field, const DoubleParserPtr& parser);

    virtual Collection<String> getStrings(const IndexReaderPtr& reader, const String& field);
    virtual StringIndexPtr getStringIndex(const IndexReaderPtr& reader, const String& field);

    virtual void setInfoStream(const InfoStreamPtr& stream);
    virtual InfoStreamPtr getInfoStream();
};

class Entry : public LuceneObject {
public:
    /// Creates one of these objects for a custom comparator/parser.
    Entry(const String& field, const boost::any& custom);
    virtual ~Entry();

    LUCENE_CLASS(Entry);

public:
    String field; // which Fieldable
    boost::any custom; // which custom comparator or parser

public:
    /// Two of these are equal if they reference the same field and type.
    virtual bool equals(const LuceneObjectPtr& other);

    /// Composes a hashcode based on the field and type.
    virtual int32_t hashCode();
};

/// Internal cache.
class Cache : public LuceneObject {
public:
    Cache(const FieldCachePtr& wrapper = FieldCachePtr());
    virtual ~Cache();

    LUCENE_CLASS(Cache);

public:
    FieldCacheWeakPtr _wrapper;
    WeakMapLuceneObjectMapEntryAny readerCache;

protected:
    virtual boost::any createValue(const IndexReaderPtr& reader, const EntryPtr& key) = 0;

public:
    /// Remove this reader from the cache, if present.
    virtual void purge(const IndexReaderPtr& r);

    virtual boost::any get(const IndexReaderPtr& reader, const EntryPtr& key);
    virtual void printNewInsanity(const InfoStreamPtr& infoStream, const boost::any& value);
};

class ByteCache : public Cache {
public:
    ByteCache(const FieldCachePtr& wrapper = FieldCachePtr());
    virtual ~ByteCache();

    LUCENE_CLASS(ByteCache);

protected:
    virtual boost::any createValue(const IndexReaderPtr& reader, const EntryPtr& key);
};

class IntCache : public Cache {
public:
    IntCache(const FieldCachePtr& wrapper = FieldCachePtr());
    virtual ~IntCache();

    LUCENE_CLASS(IntCache);

protected:
    virtual boost::any createValue(const IndexReaderPtr& reader, const EntryPtr& key);
};

class LongCache : public Cache {
public:
    LongCache(const FieldCachePtr& wrapper = FieldCachePtr());
    virtual ~LongCache();

    LUCENE_CLASS(LongCache);

protected:
    virtual boost::any createValue(const IndexReaderPtr& reader, const EntryPtr& key);
};

class DoubleCache : public Cache {
public:
    DoubleCache(const FieldCachePtr& wrapper = FieldCachePtr());
    virtual ~DoubleCache();

    LUCENE_CLASS(DoubleCache);

protected:
    virtual boost::any createValue(const IndexReaderPtr& reader, const EntryPtr& key);
};

class StringCache : public Cache {
public:
    StringCache(const FieldCachePtr& wrapper = FieldCachePtr());
    virtual ~StringCache();

    LUCENE_CLASS(StringCache);

protected:
    virtual boost::any createValue(const IndexReaderPtr& reader, const EntryPtr& key);
};

class StringIndexCache : public Cache {
public:
    StringIndexCache(const FieldCachePtr& wrapper = FieldCachePtr());
    virtual ~StringIndexCache();

    LUCENE_CLASS(StringIndexCache);

protected:
    virtual boost::any createValue(const IndexReaderPtr& reader, const EntryPtr& key);
};

class FieldCacheEntryImpl : public FieldCacheEntry {
public:
    FieldCacheEntryImpl(const LuceneObjectPtr& readerKey, const String& fieldName, int32_t cacheType, const boost::any& custom, const boost::any& value);
    virtual ~FieldCacheEntryImpl();

    LUCENE_CLASS(FieldCacheEntryImpl);

protected:
    LuceneObjectPtr readerKey;
    String fieldName;
    int32_t cacheType;
    boost::any custom;
    boost::any value;

public:
    virtual LuceneObjectPtr getReaderKey();
    virtual String getFieldName();
    virtual int32_t getCacheType();
    virtual boost::any getCustom();
    virtual boost::any getValue();
};

}

#endif
