/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef SMALLDOUBLE_H
#define SMALLDOUBLE_H

#include "LuceneObject.h"

namespace Lucene {

/// Floating point numbers smaller than 32 bits.
class SmallDouble : public LuceneObject {
public:
    virtual ~SmallDouble();
    LUCENE_CLASS(SmallDouble);

public:
    /// Converts a floating point number to an 8 bit float.
    /// Values less than zero are all mapped to zero.
    /// Values are truncated (rounded down) to the nearest 8 bit value.
    /// Values between zero and the smallest representable value are rounded up.
    static uint8_t doubleToByte(double f);

    /// Converts an 8 bit floating point number to a double.
    static double byteToDouble(uint8_t b);
};

}

#endif
