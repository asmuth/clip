/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "fnord-fts/fts.h"
#include "fnord-fts/search/FieldCacheImpl.h"
#include "fnord-fts/index/IndexReader.h"
#include "fnord-fts/util/InfoStream.h"
#include "fnord-fts/index/TermEnum.h"
#include "fnord-fts/index/TermDocs.h"
#include "fnord-fts/index/Term.h"
#include "fnord-fts/util/FieldCacheSanityChecker.h"
#include "fnord-fts/util/StringUtils.h"
#include "fnord-fts/util/VariantUtils.h"

namespace fnord {
namespace fts {

FieldCacheImpl::FieldCacheImpl() {
}

FieldCacheImpl::~FieldCacheImpl() {
}

void FieldCacheImpl::initialize() {
    caches = MapStringCache::newInstance();
    caches.put(CACHE_BYTE, newLucene<ByteCache>(shared_from_this()));
    caches.put(CACHE_INT, newLucene<IntCache>(shared_from_this()));
    caches.put(CACHE_LONG, newLucene<LongCache>(shared_from_this()));
    caches.put(CACHE_DOUBLE, newLucene<DoubleCache>(shared_from_this()));
    caches.put(CACHE_STRING, newLucene<StringCache>(shared_from_this()));
    caches.put(CACHE_STRING_INDEX, newLucene<StringIndexCache>(shared_from_this()));
}

void FieldCacheImpl::purgeAllCaches() {
    initialize();
}

void FieldCacheImpl::purge(const IndexReaderPtr& r) {
    for (MapStringCache::iterator cache = caches.begin(); cache != caches.end(); ++cache) {
        cache->second->purge(r);
    }
}

Collection<FieldCacheEntryPtr> FieldCacheImpl::getCacheEntries() {
    Collection<FieldCacheEntryPtr> result(Collection<FieldCacheEntryPtr>::newInstance());
    for (MapStringCache::iterator cache = caches.begin(); cache != caches.end(); ++cache) {
        for (WeakMapLuceneObjectMapEntryAny::iterator key = cache->second->readerCache.begin(); key != cache->second->readerCache.end(); ++key) {
            LuceneObjectPtr readerKey(key->first.lock());

            // we've now materialized a hard ref
            if (readerKey) {
                for (MapEntryAny::iterator mapEntry = key->second.begin(); mapEntry != key->second.end(); ++mapEntry) {
                    result.add(newLucene<FieldCacheEntryImpl>(readerKey, mapEntry->first->field, cache->first, mapEntry->first->custom, mapEntry->second));
                }
            }
        }
    }
    return result;
}

Collection<uint8_t> FieldCacheImpl::getBytes(const IndexReaderPtr& reader, const String& field) {
    return getBytes(reader, field, ByteParserPtr());
}

Collection<uint8_t> FieldCacheImpl::getBytes(const IndexReaderPtr& reader, const String& field, const ByteParserPtr& parser) {
    return VariantUtils::get< Collection<uint8_t> >(caches.get(CACHE_BYTE)->get(reader, newLucene<Entry>(field, parser)));
}

Collection<int32_t> FieldCacheImpl::getInts(const IndexReaderPtr& reader, const String& field) {
    return getInts(reader, field, IntParserPtr());
}

Collection<int32_t> FieldCacheImpl::getInts(const IndexReaderPtr& reader, const String& field, const IntParserPtr& parser) {
    return VariantUtils::get< Collection<int32_t> >(caches.get(CACHE_INT)->get(reader, newLucene<Entry>(field, parser)));
}

Collection<int64_t> FieldCacheImpl::getLongs(const IndexReaderPtr& reader, const String& field) {
    return getLongs(reader, field, LongParserPtr());
}

Collection<int64_t> FieldCacheImpl::getLongs(const IndexReaderPtr& reader, const String& field, const LongParserPtr& parser) {
    return VariantUtils::get< Collection<int64_t> >(caches.get(CACHE_LONG)->get(reader, newLucene<Entry>(field, parser)));
}

Collection<double> FieldCacheImpl::getDoubles(const IndexReaderPtr& reader, const String& field) {
    return getDoubles(reader, field, DoubleParserPtr());
}

Collection<double> FieldCacheImpl::getDoubles(const IndexReaderPtr& reader, const String& field, const DoubleParserPtr& parser) {
    return VariantUtils::get< Collection<double> >(caches.get(CACHE_DOUBLE)->get(reader, newLucene<Entry>(field, parser)));
}

Collection<String> FieldCacheImpl::getStrings(const IndexReaderPtr& reader, const String& field) {
    return VariantUtils::get< Collection<String> >(caches.get(CACHE_STRING)->get(reader, newLucene<Entry>(field, ParserPtr())));
}

StringIndexPtr FieldCacheImpl::getStringIndex(const IndexReaderPtr& reader, const String& field) {
    return VariantUtils::get< StringIndexPtr >(caches.get(CACHE_STRING_INDEX)->get(reader, newLucene<Entry>(field, ParserPtr())));
}

void FieldCacheImpl::setInfoStream(const InfoStreamPtr& stream) {
    infoStream = stream;
}

InfoStreamPtr FieldCacheImpl::getInfoStream() {
    return infoStream;
}

Entry::Entry(const String& field, const boost::any& custom) {
    this->field = field;
    this->custom = custom;
}

Entry::~Entry() {
}

bool Entry::equals(const LuceneObjectPtr& other) {
    if (LuceneObject::equals(other)) {
        return true;
    }

    EntryPtr otherEntry(std::dynamic_pointer_cast<Entry>(other));
    if (otherEntry) {
        if (otherEntry->field == field) {
            return VariantUtils::equalsType(custom, otherEntry->custom);
        }
    }
    return false;
}

int32_t Entry::hashCode() {
    return StringUtils::hashCode(field) ^ VariantUtils::hashCode(custom);
}

Cache::Cache(const FieldCachePtr& wrapper) {
    this->_wrapper = wrapper;
    this->readerCache = WeakMapLuceneObjectMapEntryAny::newInstance();
}

Cache::~Cache() {
}

void Cache::purge(const IndexReaderPtr& r) {
    LuceneObjectPtr readerKey(r->getFieldCacheKey());
    SyncLock cacheLock(&readerCache);
    readerCache.remove(readerKey);
}

boost::any Cache::get(const IndexReaderPtr& reader, const EntryPtr& key) {
    MapEntryAny innerCache;
    boost::any value;
    LuceneObjectPtr readerKey(reader->getFieldCacheKey());
    {
        SyncLock cacheLock(&readerCache);
        innerCache = readerCache.get(readerKey);
        if (!innerCache) {
            innerCache = MapEntryAny::newInstance();
            readerCache.put(readerKey, innerCache);
        } else if (innerCache.contains(key)) {
            value = innerCache[key];
        }
        if (VariantUtils::isNull(value)) {
            value = newLucene<CreationPlaceholder>();
            innerCache.put(key, value);
        }
    }
    if (VariantUtils::typeOf<CreationPlaceholderPtr>(value)) {
        CreationPlaceholderPtr progress(VariantUtils::get<CreationPlaceholderPtr>(value));
        SyncLock valueLock(progress);
        if (VariantUtils::isNull(progress->value)) {
            progress->value = createValue(reader, key);
            {
                SyncLock cacheLock(&readerCache);
                innerCache.put(key, progress->value);
            }

            FieldCachePtr wrapper(_wrapper);

            // Only check if key.custom (the parser) is non-null; else, we check twice for a single
            // call to FieldCache.getXXX
            if (!VariantUtils::isNull(key->custom) && wrapper) {
                InfoStreamPtr infoStream(wrapper->getInfoStream());
                if (infoStream) {
                    printNewInsanity(infoStream, progress->value);
                }
            }
        }
        return progress->value;
    }
    return value;
}

void Cache::printNewInsanity(const InfoStreamPtr& infoStream, const boost::any& value) {
    Collection<InsanityPtr> insanities(FieldCacheSanityChecker::checkSanity(FieldCachePtr(_wrapper)));
    for (Collection<InsanityPtr>::iterator insanity = insanities.begin(); insanity != insanities.end(); ++insanity) {
        Collection<FieldCacheEntryPtr> entries((*insanity)->getCacheEntries());
        for (Collection<FieldCacheEntryPtr>::iterator entry = entries.begin(); entry != entries.end(); ++entry) {
            if (VariantUtils::equalsType((*entry)->getValue(), value)) {
                // OK this insanity involves our entry
                *infoStream << L"WARNING: new FieldCache insanity created\nDetails: " + (*insanity)->toString() << L"\n";
                break;
            }
        }
    }
}

ByteCache::ByteCache(const FieldCachePtr& wrapper) : Cache(wrapper) {
}

ByteCache::~ByteCache() {
}

boost::any ByteCache::createValue(const IndexReaderPtr& reader, const EntryPtr& key) {
    EntryPtr entry(key);
    String field(entry->field);
    ByteParserPtr parser(VariantUtils::get<ByteParserPtr>(entry->custom));
    if (!parser) {
        return FieldCachePtr(_wrapper)->getBytes(reader, field, FieldCache::DEFAULT_BYTE_PARSER());
    }
    Collection<uint8_t> retArray(Collection<uint8_t>::newInstance(reader->maxDoc()));
    TermDocsPtr termDocs(reader->termDocs());
    TermEnumPtr termEnum(reader->terms(newLucene<Term>(field)));
    LuceneException finally;
    try {
        do {
            TermPtr term(termEnum->term());
            if (!term || term->field() != field) {
                break;
            }
            uint8_t termval = parser->parseByte(term->text());
            termDocs->seek(termEnum);
            while (termDocs->next()) {
                retArray[termDocs->doc()] = termval;
            }
        } while (termEnum->next());
    } catch (StopFillCacheException&) {
    } catch (LuceneException& e) {
        finally = e;
    }
    termDocs->close();
    termEnum->close();
    finally.throwException();
    return retArray;
}

IntCache::IntCache(const FieldCachePtr& wrapper) : Cache(wrapper) {
}

IntCache::~IntCache() {
}

boost::any IntCache::createValue(const IndexReaderPtr& reader, const EntryPtr& key) {
    EntryPtr entry(key);
    String field(entry->field);
    IntParserPtr parser(VariantUtils::get<IntParserPtr>(entry->custom));
    if (!parser) {
        FieldCachePtr wrapper(_wrapper);
        boost::any ints;
        try {
            ints = wrapper->getInts(reader, field, FieldCache::DEFAULT_INT_PARSER());
        } catch (NumberFormatException&) {
            ints = wrapper->getInts(reader, field, FieldCache::NUMERIC_UTILS_INT_PARSER());
        }
        return ints;
    }
    Collection<int32_t> retArray;
    TermDocsPtr termDocs(reader->termDocs());
    TermEnumPtr termEnum(reader->terms(newLucene<Term>(field)));
    LuceneException finally;
    try {
        do {
            TermPtr term(termEnum->term());
            if (!term || term->field() != field) {
                break;
            }
            int32_t termval = parser->parseInt(term->text());
            if (!retArray) { // late init
                retArray = Collection<int32_t>::newInstance(reader->maxDoc());
            }
            termDocs->seek(termEnum);
            while (termDocs->next()) {
                retArray[termDocs->doc()] = termval;
            }
        } while (termEnum->next());
    } catch (StopFillCacheException&) {
    } catch (LuceneException& e) {
        finally = e;
    }
    termDocs->close();
    termEnum->close();
    finally.throwException();
    if (!retArray) { // no values
        retArray = Collection<int32_t>::newInstance(reader->maxDoc());
    }
    return retArray;
}

LongCache::LongCache(const FieldCachePtr& wrapper) : Cache(wrapper) {
}

LongCache::~LongCache() {
}

boost::any LongCache::createValue(const IndexReaderPtr& reader, const EntryPtr& key) {
    EntryPtr entry(key);
    String field(entry->field);
    LongParserPtr parser(VariantUtils::get<LongParserPtr>(entry->custom));
    if (!parser) {
        FieldCachePtr wrapper(_wrapper);
        boost::any longs;
        try {
            longs = wrapper->getLongs(reader, field, FieldCache::DEFAULT_LONG_PARSER());
        } catch (NumberFormatException&) {
            longs = wrapper->getLongs(reader, field, FieldCache::NUMERIC_UTILS_LONG_PARSER());
        }
        return longs;
    }
    Collection<int64_t> retArray;
    TermDocsPtr termDocs(reader->termDocs());
    TermEnumPtr termEnum(reader->terms(newLucene<Term>(field)));
    LuceneException finally;
    try {
        do {
            TermPtr term(termEnum->term());
            if (!term || term->field() != field) {
                break;
            }
            int64_t termval = parser->parseLong(term->text());
            if (!retArray) { // late init
                retArray = Collection<int64_t>::newInstance(reader->maxDoc());
            }
            termDocs->seek(termEnum);
            while (termDocs->next()) {
                retArray[termDocs->doc()] = termval;
            }
        } while (termEnum->next());
    } catch (StopFillCacheException&) {
    } catch (LuceneException& e) {
        finally = e;
    }
    termDocs->close();
    termEnum->close();
    finally.throwException();
    if (!retArray) { // no values
        retArray = Collection<int64_t>::newInstance(reader->maxDoc());
    }
    return retArray;
}

DoubleCache::DoubleCache(const FieldCachePtr& wrapper) : Cache(wrapper) {
}

DoubleCache::~DoubleCache() {
}

boost::any DoubleCache::createValue(const IndexReaderPtr& reader, const EntryPtr& key) {
    EntryPtr entry(key);
    String field(entry->field);
    DoubleParserPtr parser(VariantUtils::get<DoubleParserPtr>(entry->custom));
    if (!parser) {
        FieldCachePtr wrapper(_wrapper);
        boost::any doubles;
        try {
            doubles = wrapper->getDoubles(reader, field, FieldCache::DEFAULT_DOUBLE_PARSER());
        } catch (NumberFormatException&) {
            doubles = wrapper->getDoubles(reader, field, FieldCache::NUMERIC_UTILS_DOUBLE_PARSER());
        }
        return doubles;
    }
    Collection<double> retArray;
    TermDocsPtr termDocs(reader->termDocs());
    TermEnumPtr termEnum(reader->terms(newLucene<Term>(field)));
    LuceneException finally;
    try {
        do {
            TermPtr term(termEnum->term());
            if (!term || term->field() != field) {
                break;
            }
            double termval = parser->parseDouble(term->text());
            if (!retArray) { // late init
                retArray = Collection<double>::newInstance(reader->maxDoc());
            }
            termDocs->seek(termEnum);
            while (termDocs->next()) {
                retArray[termDocs->doc()] = termval;
            }
        } while (termEnum->next());
    } catch (StopFillCacheException&) {
    } catch (LuceneException& e) {
        finally = e;
    }
    termDocs->close();
    termEnum->close();
    finally.throwException();
    if (!retArray) { // no values
        retArray = Collection<double>::newInstance(reader->maxDoc());
    }
    return retArray;
}

StringCache::StringCache(const FieldCachePtr& wrapper) : Cache(wrapper) {
}

StringCache::~StringCache() {
}

boost::any StringCache::createValue(const IndexReaderPtr& reader, const EntryPtr& key) {
    EntryPtr entry(key);
    String field(entry->field);
    Collection<String> retArray(Collection<String>::newInstance(reader->maxDoc()));
    TermDocsPtr termDocs(reader->termDocs());
    TermEnumPtr termEnum(reader->terms(newLucene<Term>(field)));
    LuceneException finally;
    try {
        do {
            TermPtr term(termEnum->term());
            if (!term || term->field() != field) {
                break;
            }
            String termval(term->text());
            termDocs->seek(termEnum);
            while (termDocs->next()) {
                retArray[termDocs->doc()] = termval;
            }
        } while (termEnum->next());
    } catch (LuceneException& e) {
        finally = e;
    }
    termDocs->close();
    termEnum->close();
    finally.throwException();
    return retArray;
}

StringIndexCache::StringIndexCache(const FieldCachePtr& wrapper) : Cache(wrapper) {
}

StringIndexCache::~StringIndexCache() {
}

boost::any StringIndexCache::createValue(const IndexReaderPtr& reader, const EntryPtr& key) {
    EntryPtr entry(key);
    String field(entry->field);
    Collection<int32_t> retArray(Collection<int32_t>::newInstance(reader->maxDoc()));
    Collection<String> mterms(Collection<String>::newInstance(reader->maxDoc() + 1));
    TermDocsPtr termDocs(reader->termDocs());
    TermEnumPtr termEnum(reader->terms(newLucene<Term>(field)));
    int32_t t = 0; // current term number

    // an entry for documents that have no terms in this field should a document with no terms be at
    // top or bottom?  This puts them at the top - if it is changed, FieldDocSortedHitQueue needs to
    // change as well.
    mterms[t++] = L"";

    LuceneException finally;
    try {
        do {
            TermPtr term(termEnum->term());
            if (!term || term->field() != field || t >= mterms.size() ) {
                break;
            }

            // store term text
            mterms[t] = term->text();

            termDocs->seek(termEnum);
            while (termDocs->next()) {
                retArray[termDocs->doc()] = t;
            }

            ++t;
        } while (termEnum->next());
    } catch (LuceneException& e) {
        finally = e;
    }
    termDocs->close();
    termEnum->close();
    finally.throwException();

    if (t == 0) {
        // if there are no terms, make the term array have a single null entry
        mterms = Collection<String>::newInstance(1);
    } else if (t < mterms.size()) {
        // if there are less terms than documents, trim off the dead array space
        mterms.resize(t);
    }

    return newLucene<StringIndex>(retArray, mterms);
}

FieldCacheEntryImpl::FieldCacheEntryImpl(const LuceneObjectPtr& readerKey, const String& fieldName, int32_t cacheType, const boost::any& custom, const boost::any& value) {
    this->readerKey = readerKey;
    this->fieldName = fieldName;
    this->cacheType = cacheType;
    this->custom = custom;
    this->value = value;
}

FieldCacheEntryImpl::~FieldCacheEntryImpl() {
}

LuceneObjectPtr FieldCacheEntryImpl::getReaderKey() {
    return readerKey;
}

String FieldCacheEntryImpl::getFieldName() {
    return fieldName;
}

int32_t FieldCacheEntryImpl::getCacheType() {
    return cacheType;
}

boost::any FieldCacheEntryImpl::getCustom() {
    return custom;
}

boost::any FieldCacheEntryImpl::getValue() {
    return value;
}

}

}
