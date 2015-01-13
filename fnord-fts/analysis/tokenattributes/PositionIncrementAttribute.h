/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef POSITIONINCREMENTATTRIBUTE_H
#define POSITIONINCREMENTATTRIBUTE_H

#include "fnord-fts/util/Attribute.h"

namespace Lucene {

/// The positionIncrement determines the position of this token relative to the previous Token in a
/// TokenStream, used in phrase searching.
///
/// The default value is one.
///
/// Some common uses for this are:
///
/// Set it to zero to put multiple terms in the same position.  This is useful if, eg., a word has multiple
/// stems.  Searches for phrases including either stem will match.  In this case, all but the first stem's
/// increment should be set to zero: the increment of the first instance should be one.  Repeating a token
/// with an increment of zero can also be used to boost the scores of matches on that token.
///
/// Set it to values greater than one to inhibit exact phrase matches.  If, for example, one does not want
/// phrases to match across removed stop words, then one could build a stop word filter that removes stop
/// words and also sets the increment to the number of stop words removed before each non-stop word.  Then
/// exact phrase queries will only match when the terms occur with no intervening stop words.
///
/// @see TermPositions
class PositionIncrementAttribute : public Attribute {
public:
    PositionIncrementAttribute();
    virtual ~PositionIncrementAttribute();

    LUCENE_CLASS(PositionIncrementAttribute);

protected:
    int32_t positionIncrement;

public:
    virtual String toString();

    /// Set the position increment. The default value is one.
    /// @param positionIncrement the distance from the prior term
    virtual void setPositionIncrement(int32_t positionIncrement);

    /// Returns the position increment of this Token.
    /// @see #setPositionIncrement
    virtual int32_t getPositionIncrement();

    virtual void clear();
    virtual bool equals(const LuceneObjectPtr& other);
    virtual int32_t hashCode();
    virtual void copyTo(const AttributePtr& target);
    virtual LuceneObjectPtr clone(const LuceneObjectPtr& other = LuceneObjectPtr());
};

}

#endif
