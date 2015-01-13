/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef FIELDINFO_H
#define FIELDINFO_H

#include "LuceneObject.h"

namespace Lucene {

class FieldInfo : public LuceneObject {
public:
    FieldInfo(const String& na, bool tk, int32_t nu, bool storeTermVector, bool storePositionWithTermVector,
              bool storeOffsetWithTermVector, bool omitNorms, bool storePayloads, bool omitTermFreqAndPositions);
    virtual ~FieldInfo();

    LUCENE_CLASS(FieldInfo);

public:
    String name;
    bool isIndexed;
    int32_t number;

    // true if term vector for this field should be stored
    bool storeTermVector;
    bool storeOffsetWithTermVector;
    bool storePositionWithTermVector;

    bool omitNorms; // omit norms associated with indexed fields
    bool omitTermFreqAndPositions;

    bool storePayloads; // whether this field stores payloads together with term positions

public:
    virtual LuceneObjectPtr clone(const LuceneObjectPtr& other = LuceneObjectPtr());

    void update(bool isIndexed, bool storeTermVector, bool storePositionWithTermVector, bool storeOffsetWithTermVector,
                bool omitNorms, bool storePayloads, bool omitTermFreqAndPositions);
};

}

#endif
