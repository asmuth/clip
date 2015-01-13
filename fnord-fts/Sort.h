/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef SORT_H
#define SORT_H

#include "LuceneObject.h"

namespace Lucene {

/// Encapsulates sort criteria for returned hits.
///
/// The fields used to determine sort order must be carefully chosen.  Documents must contain a single term
/// in such a field, and the value of the term should indicate the document's relative position in a given
/// sort order.  The field must be indexed, but should not be tokenized, and does not need to be stored
/// (unless you happen to want it back with the rest of your document data).  In other words:
///
/// <pre>
/// document->add(newLucene<Field>(L"byNumber", StringUtils::toString(x), Field::STORE_NO, Field::INDEX_NOT_ANALYZED));
/// </pre>
///
/// Valid Types of Values
///
/// There are four possible kinds of term values which may be put into sorting fields: Integers, Longs, Doubles,
/// or Strings.  Unless {@link SortField SortField} objects are specified, the type of value in the field is
/// determined by parsing the first term in the field.
///
/// Integer term values should contain only digits and an optional preceding negative sign.  Values must be base
/// 10 and in the range INT_MIN and INT_MAX inclusive.  Documents which should appear first in the sort should
/// have low value integers, later documents high values (ie. the documents should be numbered 1..n where 1 is
/// the first and n the last).
///
/// Long term values should contain only digits and an optional preceding negative sign.  Values must be base 10
/// and in the range LLONG_MIN and LLONG_MAX inclusive.  Documents which should appear first in the sort should
/// have low value integers, later documents high values.
///
/// Double term values should conform to values accepted by Double (except that NaN and Infinity are not
/// supported).  Documents which should appear first in the sort should have low values, later documents high
/// values.
///
/// String term values can contain any valid String, but should not be tokenized.  The values are sorted according
/// to their comparable natural order.  Note that using this type of term value has higher memory requirements
/// than the other two types.
///
/// Object Reuse
///
/// One of these objects can be used multiple times and the sort order changed between usages.
/// This class is thread safe.
///
/// Memory Usage
///
/// Sorting uses of caches of term values maintained by the internal HitQueue(s).  The cache is static and
/// contains an integer or double array of length IndexReader::maxDoc() for each field name for which a sort is
/// performed.  In other words, the size of the cache in bytes is:
///
/// <pre>
/// 4 * IndexReader::maxDoc() * (# of different fields actually used to sort)
/// </pre>
///
/// For String fields, the cache is larger: in addition to the above array, the value of every term in the
/// field is kept in memory.  If there are many unique terms in the field, this could be quite large.
///
/// Note that the size of the cache is not affected by how many fields are in the index and might be used to
/// sort - only by the ones actually used to sort a result set.
class Sort : public LuceneObject {
public:
    /// Sorts by computed relevance. This is the same sort criteria as calling {@link
    /// Searcher#search(QueryPtr, int32_t) Searcher#search()} without a sort criteria, only with slightly more
    /// overhead.
    Sort();

    /// Sorts by the criteria in the given SortField.
    Sort(const SortFieldPtr& field);

    /// Sorts in succession by the criteria in each SortField.
    Sort(Collection<SortFieldPtr> fields);

    virtual ~Sort();

    LUCENE_CLASS(Sort);

public:
    /// Internal representation of the sort criteria
    Collection<SortFieldPtr> fields;

public:
    /// Represents sorting by computed relevance. Using this sort criteria returns the same results as calling
    /// {@link Searcher#search(QueryPtr, int32_t) Searcher#search()} without a sort criteria, only with slightly
    /// more overhead.
    static SortPtr RELEVANCE();

    /// Represents sorting by index order.
    static SortPtr INDEXORDER();

    /// Sets the sort to the given criteria.
    void setSort(const SortFieldPtr& field);

    /// Sets the sort to the given criteria in succession.
    void setSort(Collection<SortFieldPtr> fields);

    /// Representation of the sort criteria.
    /// @return Array of SortField objects used in this sort criteria
    Collection<SortFieldPtr> getSort();

    virtual String toString();
    virtual bool equals(const LuceneObjectPtr& other);
    virtual int32_t hashCode();
};

}

#endif
