/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef FIELDCACHETERMSFILTER_H
#define FIELDCACHETERMSFILTER_H

#include "Filter.h"

namespace Lucene {

/// A {@link Filter} that only accepts documents whose single term value in the specified field is contained
/// in the provided set of allowed terms.
///
/// This is the same functionality as TermsFilter (from contrib/queries), except this filter requires that the
/// field contains only a single term for all documents.  Because of drastically different implementations,
/// they also have different performance characteristics, as described below.
///
/// The first invocation of this filter on a given field will be slower, since a {@link StringIndex} must be
/// created.  Subsequent invocations using the same field will re-use this cache.  However, as with all
/// functionality based on {@link FieldCache}, persistent RAM is consumed to hold the cache, and is not freed
/// until the {@link IndexReader} is closed.  In contrast, TermsFilter has no persistent RAM consumption.
///
/// With each search, this filter translates the specified set of Terms into a private {@link OpenBitSet} keyed
/// by term number per unique {@link IndexReader} (normally one reader per segment).  Then, during matching,
/// the term number for each docID is retrieved from the cache and then checked for inclusion using the {@link
/// OpenBitSet}.  Since all testing is done using RAM resident data structures, performance should be very fast,
/// most likely fast enough to not require further caching of the DocIdSet for each possible combination of
/// terms.  However, because docIDs are simply scanned linearly, an index with a great many small documents may
/// find this linear scan too costly.
///
/// In contrast, TermsFilter builds up an {@link OpenBitSet}, keyed by docID, every time it's created, by
/// enumerating through all matching docs using {@link TermDocs} to seek and scan through each term's docID list.
/// While there is no linear scan of all docIDs, besides the allocation of the underlying array in the {@link
/// OpenBitSet}, this approach requires a number of "disk seeks" in proportion to the number of terms, which can
/// be exceptionally costly when there are cache misses in the OS's IO cache.
///
/// Generally, this filter will be slower on the first invocation for a given field, but subsequent invocations,
/// even if you change the allowed set of Terms, should be faster than TermsFilter, especially as the number of
/// Terms being matched increases.  If you are matching only a very small number of terms, and those terms in
/// turn match a very small number of documents, TermsFilter may perform faster.
///
/// Which filter is best is very application dependent.
class FieldCacheTermsFilter : public Filter {
public:
    FieldCacheTermsFilter(const String& field, Collection<String> terms);
    virtual ~FieldCacheTermsFilter();

    LUCENE_CLASS(FieldCacheTermsFilter);

protected:
    String field;
    Collection<String> terms;

public:
    FieldCachePtr getFieldCache();

    virtual DocIdSetPtr getDocIdSet(const IndexReaderPtr& reader);
};

}

#endif
