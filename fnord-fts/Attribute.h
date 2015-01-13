/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H

#include "LuceneObject.h"

namespace Lucene {

/// Base class for Attributes that can be added to a {@link AttributeSource}.
///
/// Attributes are used to add data in a dynamic, yet type-safe way to a source of usually streamed objects,
/// eg. a {@link TokenStream}.
class Attribute : public LuceneObject {
public:
    virtual ~Attribute();
    LUCENE_CLASS(Attribute);

public:
    /// Clears the values in this Attribute and resets it to its default value. If this implementation
    /// implements more than one Attribute interface it clears all.
    virtual void clear() = 0;

    /// Subclasses must implement this method and should compute a hashCode similar to this:
    ///
    /// int32_t hashCode()
    /// {
    ///     int32_t code = startOffset;
    ///     code = code * 31 + endOffset;
    ///     return code;
    /// }
    ///
    /// see also {@link #equals(Object)}
    virtual int32_t hashCode() = 0;

    /// All values used for computation of {@link #hashCode()} should be checked here for equality.
    ///
    /// see also {@link LuceneObject#equals(Object)}
    virtual bool equals(const LuceneObjectPtr& other) = 0;

    /// Copies the values from this Attribute into the passed-in target attribute. The target implementation
    /// must support all the Attributes this implementation supports.
    virtual void copyTo(const AttributePtr& target) = 0;

    /// Shallow clone. Subclasses must override this if they need to clone any members deeply.
    /// @param base clone reference - null when called initially, then set in top virtual override.
    virtual LuceneObjectPtr clone(const LuceneObjectPtr& other = LuceneObjectPtr()) = 0;
};

}

#endif
