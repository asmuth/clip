/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef ORDFIELDSOURCE_H
#define ORDFIELDSOURCE_H

#include "fnord-fts/search/function/ValueSource.h"

namespace fnord {
namespace fts {

/// Obtains the ordinal of the field value from the default Lucene {@link FieldCache} using getStringIndex().
///
/// The native lucene index order is used to assign an ordinal value for each field value.
///
/// Field values (terms) are lexicographically ordered by unicode value, and numbered starting at 1.
/// Example:
/// If there were only three field values: "apple","banana","pear" then ord("apple")=1, ord("banana")=2,
/// ord("pear")=3
///
/// WARNING: ord() depends on the position in an index and can thus change when other documents are inserted
/// or deleted, or if a MultiSearcher is used.
///
/// NOTE: with the switch in 2.9 to segment-based searching, if {@link #getValues} is invoked with a composite
/// (multi-segment) reader, this can easily cause double RAM usage for the values in the FieldCache.  It's
/// best to switch your application to pass only atomic (single segment) readers to this API.  Alternatively,
/// for a short-term fix, you could wrap your ValueSource using {@link MultiValueSource}, which costs more CPU
/// per lookup but will not consume double the FieldCache RAM.
class OrdFieldSource : public ValueSource {
public:
    /// Constructor for a certain field.
    ///@param field field whose values order is used.
    OrdFieldSource(const String& field);
    virtual ~OrdFieldSource();

    LUCENE_CLASS(OrdFieldSource);

protected:
    String field;

public:
    virtual String description();
    virtual DocValuesPtr getValues(const IndexReaderPtr& reader);
    virtual bool equals(const LuceneObjectPtr& other);
    virtual int32_t hashCode();
};

}

}
#endif
