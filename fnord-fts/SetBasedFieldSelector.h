/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef SETBASEDFIELDSELECTOR_H
#define SETBASEDFIELDSELECTOR_H

#include "FieldSelector.h"

namespace Lucene {

/// Declare what fields to load normally and what fields to load lazily
class SetBasedFieldSelector : public FieldSelector {
public:
    /// Pass in the Set of {@link Field} names to load and the Set of {@link Field} names to load lazily.
    /// If both are null, the Document will not have any {@link Field} on it.
    /// @param fieldsToLoad A Set of {@link String} field names to load.  May be empty, but not null
    /// @param lazyFieldsToLoad A Set of {@link String} field names to load lazily.  May be empty, but not null
    SetBasedFieldSelector(HashSet<String> fieldsToLoad, HashSet<String> lazyFieldsToLoad);

    virtual ~SetBasedFieldSelector();

    LUCENE_CLASS(SetBasedFieldSelector);

protected:
    HashSet<String> fieldsToLoad;
    HashSet<String> lazyFieldsToLoad;

public:
    /// Indicate whether to load the field with the given name or not. If the {@link Field#name()} is not in
    /// either of the  initializing Sets, then {@link FieldSelectorResult#NO_LOAD} is returned.  If a Field name
    /// is in both fieldsToLoad and lazyFieldsToLoad, lazy has precedence.
    /// @param fieldName The {@link Field} name to check
    /// @return The {@link FieldSelectorResult}
    virtual FieldSelectorResult accept(const String& fieldName);
};

}

#endif
