/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef FIELDDOC_H
#define FIELDDOC_H

#include "fnord-fts/search/ScoreDoc.h"

namespace fnord {
namespace fts {

/// A ScoreDoc which also contains information about how to sort the referenced document.  In addition to the
/// document number and score, this object contains an array of values for the document from the field(s) used
/// to sort.  For example, if the sort criteria was to sort by fields "a", "b" then "c", the fields object array
/// will have three elements, corresponding respectively to the term values for the document in fields "a", "b"
/// and "c".  The class of each element in the array will be either Integer, Double or String depending on the
/// type of values in the terms of each field.
class FieldDoc : public ScoreDoc {
public:
    FieldDoc(int32_t doc, double score, Collection<ComparableValue> fields = Collection<ComparableValue>());
    virtual ~FieldDoc();

    LUCENE_CLASS(FieldDoc);

public:
    /// The values which are used to sort the referenced document.  The order of these will match the original
    /// sort criteria given by a Sort object.  Each Object will be either an Integer, Double or String, depending
    /// on the type of values in the terms of the original field.
    /// @see Sort
    /// @see Searcher#search(QueryPtr, FilterPtr, int32_t, SortPtr)
    Collection<ComparableValue> fields;

public:
    virtual String toString();
};

}

}
#endif
