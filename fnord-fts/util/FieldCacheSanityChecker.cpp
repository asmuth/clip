/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "FieldCacheSanityChecker.h"
#include "_FieldCacheSanityChecker.h"
#include "FieldCache.h"
#include "IndexReader.h"
#include "StringUtils.h"
#include "VariantUtils.h"

namespace Lucene {

FieldCacheSanityChecker::FieldCacheSanityChecker() {
}

FieldCacheSanityChecker::~FieldCacheSanityChecker() {
}

Collection<InsanityPtr> FieldCacheSanityChecker::checkSanity(const FieldCachePtr& cache) {
    return checkSanity(cache->getCacheEntries());
}

Collection<InsanityPtr> FieldCacheSanityChecker::checkSanity(Collection<FieldCacheEntryPtr> cacheEntries) {
    FieldCacheSanityCheckerPtr sanityChecker(newLucene<FieldCacheSanityChecker>());
    return sanityChecker->check(cacheEntries);
}

Collection<InsanityPtr> FieldCacheSanityChecker::check(Collection<FieldCacheEntryPtr> cacheEntries) {
    if (!cacheEntries || cacheEntries.empty()) {
        return Collection<InsanityPtr>::newInstance();
    }

    // Maps the (valId) identityhashCode of cache values to sets of CacheEntry instances
    MapSetIntFieldCacheEntry valIdToItems(MapSetIntFieldCacheEntry::map_type::newInstance());

    // Maps ReaderField keys to Sets of ValueIds
    MapSetReaderFieldInt readerFieldToValIds(MapSetReaderFieldInt::map_type::newInstance());

    // Any keys that we know result in more then one valId
    SetReaderField valMismatchKeys(SetReaderField::newInstance());

    // iterate over all the cacheEntries to get the mappings we'll need
    for (int32_t i = 0; i < cacheEntries.size(); ++i) {
        FieldCacheEntryPtr item(cacheEntries[i]);
        boost::any val(item->getValue());

        if (VariantUtils::typeOf<CreationPlaceholderPtr>(val)) {
            continue;
        }

        ReaderFieldPtr rf(newLucene<ReaderField>(item->getReaderKey(), item->getFieldName()));
        int32_t valId = VariantUtils::hashCode(val);

        // indirect mapping, so the MapOfSet will dedup identical valIds for us
        valIdToItems.put(valId, item);
        if (1 < readerFieldToValIds.put(rf, valId)) {
            valMismatchKeys.add(rf);
        }
    }

    Collection<InsanityPtr> insanity(Collection<InsanityPtr>::newInstance());

    Collection<InsanityPtr> mismatch(checkValueMismatch(valIdToItems, readerFieldToValIds, valMismatchKeys));
    insanity.addAll(mismatch.begin(), mismatch.end());

    Collection<InsanityPtr> subreaders(checkSubreaders(valIdToItems, readerFieldToValIds));
    insanity.addAll(subreaders.begin(), subreaders.end());

    return insanity;
}

Collection<InsanityPtr> FieldCacheSanityChecker::checkValueMismatch(MapSetIntFieldCacheEntry valIdToItems,
        MapSetReaderFieldInt readerFieldToValIds,
        SetReaderField valMismatchKeys) {
    Collection<InsanityPtr> insanity(Collection<InsanityPtr>::newInstance());

    if (!valMismatchKeys.empty()) {
        // we have multiple values for some ReaderFields

        MapSetReaderFieldInt::map_type rfMap = readerFieldToValIds.getMap();
        MapSetIntFieldCacheEntry::map_type valMap = valIdToItems.getMap();

        for (SetReaderField::iterator rf = valMismatchKeys.begin(); rf != valMismatchKeys.end(); ++rf) {
            Collection<FieldCacheEntryPtr> badEntries(Collection<FieldCacheEntryPtr>::newInstance());

            MapSetReaderFieldInt::set_type values(rfMap.get(*rf));
            for (MapSetReaderFieldInt::set_type::iterator value = values.begin(); value != values.end(); ++value) {
                MapSetIntFieldCacheEntry::set_type cacheEntries(valMap.get(*value));
                for (MapSetIntFieldCacheEntry::set_type::iterator cacheEntry = cacheEntries.begin(); cacheEntry != cacheEntries.end(); ++cacheEntry) {
                    badEntries.add(*cacheEntry);
                }
            }

            insanity.add(newLucene<Insanity>(VALUEMISMATCH, L"Multiple distinct value objects for " + (*rf)->toString(), badEntries));
        }
    }
    return insanity;
}

Collection<InsanityPtr> FieldCacheSanityChecker::checkSubreaders(MapSetIntFieldCacheEntry valIdToItems,
        MapSetReaderFieldInt readerFieldToValIds) {
    Collection<InsanityPtr> insanity(Collection<InsanityPtr>::newInstance());

    MapReaderFieldSetReaderField badChildren(MapReaderFieldSetReaderField::newInstance());
    MapSetReaderFieldReaderField badKids(badChildren); // wrapper

    MapSetIntFieldCacheEntry::map_type viToItemSets = valIdToItems.getMap();
    MapSetReaderFieldInt::map_type rfToValIdSets = readerFieldToValIds.getMap();

    SetReaderField seen(SetReaderField::newInstance());

    for (MapSetReaderFieldInt::map_type::iterator rf = rfToValIdSets.begin(); rf != rfToValIdSets.end(); ++rf) {
        if (seen.contains(rf->first)) {
            continue;
        }

        Collection<LuceneObjectPtr> kids(getAllDecendentReaderKeys(rf->first->readerKey));
        for (Collection<LuceneObjectPtr>::iterator kidKey = kids.begin(); kidKey != kids.end(); ++kidKey) {
            ReaderFieldPtr kid(newLucene<ReaderField>(*kidKey, rf->first->fieldName));

            if (badChildren.contains(kid)) {
                // we've already process this kid as RF and found other problems track those problems as our own
                badKids.put(rf->first, kid);
                badKids.putAll(rf->first, badChildren.get(kid));
                badChildren.remove(kid);
            } else if (rfToValIdSets.contains(kid)) {
                // we have cache entries for the kid
                badKids.put(rf->first, kid);
            }
            seen.add(kid);
        }
        seen.add(rf->first);
    }

    // every mapping in badKids represents an Insanity
    for (MapReaderFieldSetReaderField::iterator parent = badChildren.begin(); parent != badChildren.end(); ++parent) {
        SetReaderField kids = parent->second;
        Collection<FieldCacheEntryPtr> badEntries(Collection<FieldCacheEntryPtr>::newInstance());

        // put parent entries in first
        MapSetReaderFieldInt::set_type values(rfToValIdSets.get(parent->first));
        for (MapSetReaderFieldInt::set_type::iterator value = values.begin(); value != values.end(); ++value) {
            MapSetIntFieldCacheEntry::set_type cacheEntries(viToItemSets.get(*value));
            badEntries.addAll(cacheEntries.begin(), cacheEntries.end());
        }

        // now the entries for the descendants
        for (SetReaderField::iterator kid = kids.begin(); kid != kids.end(); ++kid) {
            MapSetReaderFieldInt::set_type values(rfToValIdSets.get(*kid));
            for (MapSetReaderFieldInt::set_type::iterator value = values.begin(); value != values.end(); ++value) {
                MapSetIntFieldCacheEntry::set_type cacheEntries(viToItemSets.get(*value));
                badEntries.addAll(cacheEntries.begin(), cacheEntries.end());
            }
        }

        insanity.add(newLucene<Insanity>(SUBREADER, L"Found caches for descendants of " + parent->first->toString(), badEntries));
    }

    return insanity;
}

Collection<LuceneObjectPtr> FieldCacheSanityChecker::getAllDecendentReaderKeys(const LuceneObjectPtr& seed) {
    Collection<LuceneObjectPtr> all(Collection<LuceneObjectPtr>::newInstance()); // will grow as we iter
    all.add(seed);
    for (int32_t i = 0; i < all.size(); ++i) {
        IndexReaderPtr indexReader(boost::dynamic_pointer_cast<IndexReader>(all[i]));
        if (indexReader) {
            Collection<IndexReaderPtr> subs(indexReader->getSequentialSubReaders());
            for (int32_t j = 0; subs && j < subs.size(); ++j) {
                all.add(subs[j]->getFieldCacheKey());
            }
        }
    }

    // need to remove the first, because it was the seed
    all.remove(all.begin());
    return all;
}

ReaderField::ReaderField(const LuceneObjectPtr& readerKey, const String& fieldName) {
    this->readerKey = readerKey;
    this->fieldName = fieldName;
}

ReaderField::~ReaderField() {
}

int32_t ReaderField::hashCode() {
    return readerKey->hashCode() * StringUtils::hashCode(fieldName);
}

bool ReaderField::equals(const LuceneObjectPtr& other) {
    ReaderFieldPtr otherReaderField(boost::dynamic_pointer_cast<ReaderField>(other));
    if (!otherReaderField) {
        return false;
    }
    return (readerKey->equals(otherReaderField->readerKey) && fieldName == otherReaderField->fieldName);
}

String ReaderField::toString() {
    return readerKey->toString() + L"+" + fieldName;
}

Insanity::Insanity(FieldCacheSanityChecker::InsanityType type, const String& msg, Collection<FieldCacheEntryPtr> entries) {
    if (!entries || entries.empty()) {
        boost::throw_exception(IllegalArgumentException(L"Insanity requires non-null/non-empty CacheEntry[]"));
    }
    this->type = type;
    this->msg = msg;
    this->entries = entries;
}

Insanity::~Insanity() {
}

FieldCacheSanityChecker::InsanityType Insanity::getType() {
    return type;
}

String Insanity::getMsg() {
    return msg;
}

Collection<FieldCacheEntryPtr> Insanity::getCacheEntries() {
    return entries;
}

String Insanity::toString() {
    StringStream buffer;
    switch (type) {
    case FieldCacheSanityChecker::SUBREADER:
        buffer << L"SUBREADER: ";
        break;
    case FieldCacheSanityChecker::VALUEMISMATCH:
        buffer << L"VALUEMISMATCH: ";
        break;
    case FieldCacheSanityChecker::EXPECTED:
        buffer << L"EXPECTED: ";
        break;
    }
    buffer << msg << L"\n";

    for (Collection<FieldCacheEntryPtr>::iterator ce = entries.begin(); ce != entries.end(); ++ce) {
        buffer << L"\t" << (*ce)->toString() << L"\n";
    }

    return buffer.str();
}

}
