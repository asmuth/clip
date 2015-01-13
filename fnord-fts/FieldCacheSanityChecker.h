/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef FIELDCACHESANITYCHECKER_H
#define FIELDCACHESANITYCHECKER_H

#include "LuceneObject.h"
#include "MapOfSets.h"

namespace Lucene {

/// Provides methods for sanity checking that entries in the FieldCache  are not wasteful or inconsistent.
///
/// Lucene 2.9 Introduced numerous enhancements into how the FieldCache is used by the low levels of Lucene
/// searching (for Sorting and ValueSourceQueries) to improve both the speed for Sorting, as well as reopening
/// of IndexReaders.  But these changes have shifted the usage of FieldCache from "top level" IndexReaders
/// (frequently a MultiReader or DirectoryReader) down to the leaf level SegmentReaders.  As a result,
/// existing applications that directly access the FieldCache may find RAM usage increase significantly when
/// upgrading to 2.9 or later.  This class provides an API for these applications (or their Unit tests) to
/// check at run time if the FieldCache contains "insane" usages of the FieldCache.
///
/// @see FieldCache
/// @see FieldCacheSanityChecker.Insanity
/// @see FieldCacheSanityChecker.InsanityType
class FieldCacheSanityChecker : public LuceneObject {
public:
    FieldCacheSanityChecker();
    virtual ~FieldCacheSanityChecker();

    LUCENE_CLASS(FieldCacheSanityChecker);

public:
    typedef MapOfSets< int32_t, boost::hash<int32_t>, std::equal_to<int32_t>, FieldCacheEntryPtr, luceneHash<FieldCacheEntryPtr>, luceneEquals<FieldCacheEntryPtr> > MapSetIntFieldCacheEntry;
    typedef MapOfSets< ReaderFieldPtr, luceneHash<ReaderFieldPtr>, luceneEquals<ReaderFieldPtr>, int32_t, boost::hash<int32_t>, std::equal_to<int32_t> > MapSetReaderFieldInt;
    typedef MapOfSets< ReaderFieldPtr, luceneHash<ReaderFieldPtr>, luceneEquals<ReaderFieldPtr>, ReaderFieldPtr, luceneHash<ReaderFieldPtr>, luceneEquals<ReaderFieldPtr> > MapSetReaderFieldReaderField;

    /// An Enumeration of the different types of "insane" behaviour that may be detected in a FieldCache.
    enum InsanityType {
        /// Indicates an overlap in cache usage on a given field in sub/super readers.
        SUBREADER,

        /// Indicates entries have the same reader+fieldname but different cached values.  This can happen
        /// if different datatypes, or parsers are used -- and while it's not necessarily a bug it's
        /// typically an indication of a possible problem.
        ///
        /// NOTE: Only the reader, fieldname, and cached value are actually tested -- if two cache entries
        /// have different parsers or datatypes but the cached values are the same Object (== not just equal())
        /// this method  does not consider that a red flag.  This allows for subtle variations in the way a
        /// Parser is specified (null vs DEFAULT_LONG_PARSER, etc...)
        VALUEMISMATCH,

        /// Indicates an expected bit of "insanity".  This may be useful for clients that wish to preserve/log
        /// information about insane usage but indicate that it was expected.
        EXPECTED
    };

    /// Quick and dirty convenience method
    /// @see #check
    static Collection<InsanityPtr> checkSanity(const FieldCachePtr& cache);

    /// Quick and dirty convenience method that instantiates an instance with "good defaults" and uses it to
    /// test the CacheEntrys.
    /// @see #check
    static Collection<InsanityPtr> checkSanity(Collection<FieldCacheEntryPtr> cacheEntries);

    /// Tests a CacheEntry[] for indication of "insane" cache usage.
    /// NOTE: FieldCache CreationPlaceholder objects are ignored.
    Collection<InsanityPtr> check(Collection<FieldCacheEntryPtr> cacheEntries);

protected:
    /// Internal helper method used by check that iterates over valMismatchKeys and generates a Collection of
    /// Insanity instances accordingly.  The MapOfSets are used to populate the Insanity objects.
    /// @see InsanityType#VALUEMISMATCH
    Collection<InsanityPtr> checkValueMismatch(MapSetIntFieldCacheEntry valIdToItems,
            MapSetReaderFieldInt readerFieldToValIds,
            SetReaderField valMismatchKeys);

    /// Internal helper method used by check that iterates over the keys of readerFieldToValIds and generates a
    /// Collection of Insanity instances whenever two (or more) ReaderField instances are found that have an
    /// ancestry relationships.
    /// @see InsanityType#SUBREADER
    Collection<InsanityPtr> checkSubreaders(MapSetIntFieldCacheEntry valIdToItems,
                                            MapSetReaderFieldInt readerFieldToValIds);

    /// Checks if the seed is an IndexReader, and if so will walk the hierarchy of subReaders building up a
    /// list of the objects returned by obj.getFieldCacheKey()
    Collection<LuceneObjectPtr> getAllDecendentReaderKeys(const LuceneObjectPtr& seed);
};

/// Simple container for a collection of related CacheEntry objects that in conjunction with each other
/// represent some "insane" usage of the FieldCache.
class Insanity : public LuceneObject {
public:
    Insanity(FieldCacheSanityChecker::InsanityType type, const String& msg, Collection<FieldCacheEntryPtr> entries);
    virtual ~Insanity();

    LUCENE_CLASS(Insanity);

protected:
    FieldCacheSanityChecker::InsanityType type;
    String msg;
    Collection<FieldCacheEntryPtr> entries;

public:
    /// Type of insane behavior this object represents
    FieldCacheSanityChecker::InsanityType getType();

    /// Description of the insane behaviour
    String getMsg();

    /// CacheEntry objects which suggest a problem
    Collection<FieldCacheEntryPtr> getCacheEntries();

    /// Multi-Line representation of this Insanity object, starting with the Type and Msg, followed by each
    /// CacheEntry.toString() on it's own line prefaced by a tab character
    virtual String toString();
};

}

#endif
