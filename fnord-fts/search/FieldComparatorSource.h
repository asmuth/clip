/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef FIELDCOMPARATORSOURCE_H
#define FIELDCOMPARATORSOURCE_H

#include "fnord-fts/util/LuceneObject.h"

namespace Lucene {

/// Provides a {@link FieldComparator} for custom field sorting.
class FieldComparatorSource : public LuceneObject {
public:
    virtual ~FieldComparatorSource();
    LUCENE_CLASS(FieldComparatorSource);

public:
    /// Creates a comparator for the field in the given index.
    /// @param fieldname Name of the field to create comparator for.
    /// @return FieldComparator.
    virtual FieldComparatorPtr newComparator(const String& fieldname, int32_t numHits, int32_t sortPos, bool reversed) = 0;
};

}

#endif
