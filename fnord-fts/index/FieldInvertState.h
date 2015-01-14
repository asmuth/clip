/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef FIELDINVERTSTATE_H
#define FIELDINVERTSTATE_H

#include "fnord-fts/util/LuceneObject.h"

namespace fnord {
namespace fts {

/// This class tracks the number and position / offset parameters of terms being added to the index.
/// The information collected in this class is also used to calculate the normalization factor for a field.
class FieldInvertState : public LuceneObject {
public:
    FieldInvertState(int32_t position = 0, int32_t length = 0, int32_t numOverlap = 0, int32_t offset = 0, double boost = 0);
    virtual ~FieldInvertState();

    LUCENE_CLASS(FieldInvertState);

INTERNAL:
    int32_t position;
    int32_t length;
    int32_t numOverlap;
    int32_t offset;
    double boost;
    AttributeSourcePtr attributeSource;

public:
    /// Re-initialize the state, using this boost value.
    /// @param docBoost boost value to use.
    void reset(double docBoost);

    /// Get the last processed term position.
    /// @return the position
    int32_t getPosition();

    /// Get total number of terms in this field.
    /// @return the length
    int32_t getLength();

    /// Get the number of terms with positionIncrement == 0.
    /// @return the numOverlap
    int32_t getNumOverlap();

    /// Get end offset of the last processed term.
    /// @return the offset
    int32_t getOffset();

    /// Get boost value. This is the cumulative product of document boost and field boost for all field
    /// instances sharing the same field name.
    /// @return the boost
    double getBoost();

    AttributeSourcePtr getAttributeSource();
};

}

}
#endif
