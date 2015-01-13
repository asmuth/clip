/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef MAPFIELDSELECTOR_H
#define MAPFIELDSELECTOR_H

#include "fnord-fts/document/FieldSelector.h"

namespace Lucene {

typedef HashMap<String, FieldSelector::FieldSelectorResult> MapStringFieldSelectorResult;

/// A {@link FieldSelector} based on a Map of field names to {@link FieldSelectorResult}s
class MapFieldSelector : public FieldSelector {
public:
    /// Create a MapFieldSelector
    /// @param fieldSelections maps from field names (String) to {@link FieldSelectorResult}s
    MapFieldSelector(MapStringFieldSelectorResult fieldSelections);

    /// Create a MapFieldSelector
    /// @param fields fields to LOAD.  List of Strings.  All other fields are NO_LOAD.
    MapFieldSelector(Collection<String> fields);

    virtual ~MapFieldSelector();

    LUCENE_CLASS(MapFieldSelector);

public:
    MapStringFieldSelectorResult fieldSelections;

public:
    /// Load field according to its associated value in fieldSelections
    /// @param field a field name
    /// @return the fieldSelections value that field maps to or NO_LOAD if none.
    virtual FieldSelectorResult accept(const String& fieldName);
};

}

#endif
