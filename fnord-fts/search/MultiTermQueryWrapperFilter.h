/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef MULTITERMQUERYWRAPPERFILTER_H
#define MULTITERMQUERYWRAPPERFILTER_H

#include "fnord-fts/search/Filter.h"

namespace Lucene {

/// A wrapper for {@link MultiTermQuery}, that exposes its functionality as a {@link Filter}.
///
/// MultiTermQueryWrapperFilter is not designed to be used by itself. Normally you subclass it to
/// provide a Filter counterpart for a {@link MultiTermQuery} subclass.
///
/// For example, {@link TermRangeFilter} and {@link PrefixFilter} extend MultiTermQueryWrapperFilter.
/// This class also provides the functionality behind {@link MultiTermQuery#CONSTANT_SCORE_FILTER_REWRITE};
/// this is why it is not abstract.
class MultiTermQueryWrapperFilter : public Filter {
INTERNAL:
    /// Wrap a {@link MultiTermQuery} as a Filter.
    MultiTermQueryWrapperFilter(const MultiTermQueryPtr& query);

public:
    virtual ~MultiTermQueryWrapperFilter();

    LUCENE_CLASS(MultiTermQueryWrapperFilter);

protected:
    MultiTermQueryPtr query;

public:
    virtual String toString();
    virtual bool equals(const LuceneObjectPtr& other);
    virtual int32_t hashCode();

    /// Return the number of unique terms visited during execution of the filter.  If there are many of them,
    /// you may consider using another filter type or optimize your total term count in index.
    ///
    /// This method is not thread safe, be sure to only call it when no filter is running!  If you re-use the
    /// same filter instance for another search, be sure to first reset the term counter with {@link
    /// #clearTotalNumberOfTerms}.
    /// @see #clearTotalNumberOfTerms
    int32_t getTotalNumberOfTerms();

    /// Resets the counting of unique terms. Do this before executing the filter.
    /// @see #getTotalNumberOfTerms
    void clearTotalNumberOfTerms();

    /// Returns a DocIdSet with documents that should be permitted in search results.
    virtual DocIdSetPtr getDocIdSet(const IndexReaderPtr& reader);
};

}

#endif
