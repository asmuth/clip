/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef COMPLEXEXPLANATION_H
#define COMPLEXEXPLANATION_H

#include "Explanation.h"

namespace Lucene {

/// Describes the score computation for document and query, and can distinguish a match independent
/// of a positive value.
class ComplexExplanation : public Explanation {
public:
    ComplexExplanation(bool match = false, double value = 0, const String& description = EmptyString);
    virtual ~ComplexExplanation();

    LUCENE_CLASS(ComplexExplanation);

protected:
    bool match;

public:
    /// The match status of this explanation node.
    bool getMatch();

    /// Sets the match status assigned to this explanation node.
    void setMatch(bool match);

    /// Indicates whether or not this Explanation models a good match.
    ///
    /// If the match status is explicitly set this method uses it; otherwise it defers to the
    /// superclass.
    ///
    /// @see #getMatch
    virtual bool isMatch();

protected:
    virtual String getSummary();
};

}

#endif
