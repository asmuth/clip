/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef TERMVECTORENTRY_H
#define TERMVECTORENTRY_H

#include "fnord-fts/util/LuceneObject.h"

namespace fnord {
namespace fts {

/// Convenience class for holding TermVector information.
class TermVectorEntry : public LuceneObject {
public:
    TermVectorEntry(const String& field = EmptyString, const String& term = EmptyString, int32_t frequency = 0,
                    Collection<TermVectorOffsetInfoPtr> offsets = Collection<TermVectorOffsetInfoPtr>(),
                    Collection<int32_t> positions = Collection<int32_t>());
    virtual ~TermVectorEntry();

    LUCENE_CLASS(TermVectorEntry);

protected:
    String field;
    String term;
    int32_t frequency;
    Collection<TermVectorOffsetInfoPtr> offsets;
    Collection<int32_t> positions;

public:
    String getField();
    int32_t getFrequency();
    Collection<TermVectorOffsetInfoPtr> getOffsets();
    Collection<int32_t> getPositions();
    String getTerm();

    void setFrequency(int32_t frequency);
    void setOffsets(Collection<TermVectorOffsetInfoPtr> offsets);
    void setPositions(Collection<int32_t> positions);

    virtual bool equals(const LuceneObjectPtr& other);
    virtual int32_t hashCode();
    virtual String toString();
};

}

}
#endif
