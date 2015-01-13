/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "LoadFirstFieldSelector.h"

namespace Lucene {

LoadFirstFieldSelector::~LoadFirstFieldSelector() {
}

FieldSelector::FieldSelectorResult LoadFirstFieldSelector::accept(const String& fieldName) {
    return FieldSelector::SELECTOR_LOAD_AND_BREAK;
}

}
