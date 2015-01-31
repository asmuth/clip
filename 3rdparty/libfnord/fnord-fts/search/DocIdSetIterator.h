/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef DOCIDSETITERATOR_H
#define DOCIDSETITERATOR_H

#include "fnord-fts/util/LuceneObject.h"

namespace fnord {
namespace fts {

/// This abstract class defines methods to iterate over a set of non-decreasing doc ids.  Note that this class
/// assumes it iterates on doc Ids, and therefore {@link #NO_MORE_DOCS} is set to {@value #NO_MORE_DOCS} in order to
/// be used as a sentinel object. Implementations of this class are expected to consider INT_MAX as an invalid value.
class DocIdSetIterator : public LuceneObject {
public:
    virtual ~DocIdSetIterator();

    LUCENE_CLASS(DocIdSetIterator);

public:
    /// When returned by {@link #nextDoc()}, {@link #advance(int)} and {@link #docID()} it means there are no more
    /// docs in the iterator.
    static const int32_t NO_MORE_DOCS;

public:
    /// Returns the following:
    /// <ul>
    /// <li>-1 or {@link #NO_MORE_DOCS} if {@link #nextDoc()} or {@link #advance(int)} were not called yet.
    /// <li>{@link #NO_MORE_DOCS} if the iterator has exhausted.
    /// <li>Otherwise it should return the doc ID it is currently on.
    /// </ul>
    virtual int32_t docID() = 0;

    /// Advances to the next document in the set and returns the doc it is currently on, or {@link #NO_MORE_DOCS}
    /// if there are no more docs in the set.
    ///
    /// NOTE: after the iterator has exhausted you should not call this method, as it may result in unpredicted
    /// behaviour.
    virtual int32_t nextDoc() = 0;

    /// Advances to the first beyond the current whose document number is greater than or equal to target. Returns
    /// the current document number or {@link #NO_MORE_DOCS} if there are no more docs in the set.
    ///
    /// Behaves as if written:
    ///
    /// <pre>
    /// int32_t advance(int32_t target)
    /// {
    ///     int32_t doc;
    ///     while ((doc = nextDoc()) < target)
    ///     { }
    ///     return doc;
    /// }
    /// </pre>
    ///
    /// Some implementations are considerably more efficient than that.
    ///
    /// NOTE: certain implementations may return a different value (each time) if called several times in a row
    /// with the same target.
    ///
    /// NOTE: this method may be called with {@value #NO_MORE_DOCS} for efficiency by some Scorers. If your
    /// implementation cannot efficiently determine that it should exhaust, it is recommended that you check for
    /// that value in each call to this method.
    ///
    /// NOTE: after the iterator has exhausted you should not call this method, as it may result in unpredicted
    /// behaviour.
    virtual int32_t advance(int32_t target) = 0;
};

}

}
#endif
