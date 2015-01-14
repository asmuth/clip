/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef FLAGSATTRIBUTE_H
#define FLAGSATTRIBUTE_H

#include "fnord-fts/util/Attribute.h"

namespace fnord {
namespace fts {

/// This attribute can be used to pass different flags down the tokenizer chain, eg from one TokenFilter
/// to another one.
class FlagsAttribute : public Attribute {
public:
    FlagsAttribute();
    virtual ~FlagsAttribute();

    LUCENE_CLASS(FlagsAttribute);

protected:
    int32_t flags;

public:
    virtual String toString();

    /// Get the bitset for any bits that have been set.  This is completely distinct from {@link
    /// TypeAttribute#type()}, although they do share similar purposes.  The flags can be used to encode
    /// information about the token for use by other {@link TokenFilter}s.
    virtual int32_t getFlags();

    /// @see #getFlags()
    virtual void setFlags(int32_t flags);

    virtual void clear();

    virtual bool equals(const LuceneObjectPtr& other);
    virtual int32_t hashCode();
    virtual void copyTo(const AttributePtr& target);
    virtual LuceneObjectPtr clone(const LuceneObjectPtr& other = LuceneObjectPtr());
};

}

}
#endif
