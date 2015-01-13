/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef COLLATOR_H
#define COLLATOR_H

#include "LuceneObject.h"

namespace Lucene {

/// Convenience class for storing collate objects.
class Collator : public LuceneObject {
public:
    /// Creates a new Collator, given the file to read from.
    Collator(std::locale locale);
    virtual ~Collator();

    LUCENE_CLASS(Collator);

protected:
    const std::collate<wchar_t>& collate;

public:
    int32_t compare(const String& first, const String& second);
};

}

#endif
