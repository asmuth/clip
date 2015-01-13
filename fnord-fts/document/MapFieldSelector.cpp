/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "MapFieldSelector.h"

namespace Lucene {

MapFieldSelector::MapFieldSelector(MapStringFieldSelectorResult fieldSelections) {
    this->fieldSelections = fieldSelections;
}

MapFieldSelector::MapFieldSelector(Collection<String> fields) {
    fieldSelections = MapStringFieldSelectorResult::newInstance();
    for (Collection<String>::iterator field = fields.begin(); field != fields.end(); ++field) {
        fieldSelections.put(*field, FieldSelector::SELECTOR_LOAD);
    }
}

MapFieldSelector::~MapFieldSelector() {
}

FieldSelector::FieldSelectorResult MapFieldSelector::accept(const String& fieldName) {
    MapStringFieldSelectorResult::iterator selection = fieldSelections.find(fieldName);
    return selection != fieldSelections.end() ? selection->second : FieldSelector::SELECTOR_NO_LOAD;
}

}
