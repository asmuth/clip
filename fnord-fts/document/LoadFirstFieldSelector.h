/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef LOADFIRSTFIELDSELECTOR_H
#define LOADFIRSTFIELDSELECTOR_H

#include "fnord-fts/document/FieldSelector.h"

namespace Lucene {

/// Load the First field and break.
/// See {@link FieldSelectorResult#LOAD_AND_BREAK}
class LoadFirstFieldSelector : public FieldSelector {
public:
    virtual ~LoadFirstFieldSelector();

    LUCENE_CLASS(LoadFirstFieldSelector);

public:
    virtual FieldSelectorResult accept(const String& fieldName);
};

}

#endif
