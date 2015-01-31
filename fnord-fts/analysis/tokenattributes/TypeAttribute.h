/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef TYPEATTRIBUTE_H
#define TYPEATTRIBUTE_H

#include "fnord-fts/util/Attribute.h"

namespace fnord {
namespace fts {

/// A Token's lexical type. The Default value is "word".
class TypeAttribute : public Attribute {
public:
    TypeAttribute();
    TypeAttribute(const String& type);
    virtual ~TypeAttribute();

    LUCENE_CLASS(TypeAttribute);

protected:
    String _type;
    static const String& DEFAULT_TYPE();

public:
    virtual String toString();

    /// Returns this Token's lexical type.  Defaults to "word".
    String type();

    /// Set the lexical type.
    /// @see #type()
    void setType(const String& type);

    virtual void clear();
    virtual bool equals(const LuceneObjectPtr& other);
    virtual int32_t hashCode();
    virtual void copyTo(const AttributePtr& target);
    virtual LuceneObjectPtr clone(const LuceneObjectPtr& other = LuceneObjectPtr());
};

}

}
#endif
