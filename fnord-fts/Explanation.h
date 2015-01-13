/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef EXPLANATION_H
#define EXPLANATION_H

#include "LuceneObject.h"

namespace Lucene {

/// Describes the score computation for document and query.
class Explanation : public LuceneObject {
public:
    Explanation(double value = 0, const String& description = EmptyString);
    virtual ~Explanation();

    LUCENE_CLASS(Explanation);

protected:
    double value; // the value of this node
    String description; // what it represents
    Collection<ExplanationPtr> details; // sub-explanations

public:
    /// Indicates whether or not this Explanation models a good match.
    ///
    /// By default, an Explanation represents a "match" if the value is positive.
    ///
    /// @see #getValue
    virtual bool isMatch();

    /// The value assigned to this explanation node.
    virtual double getValue();

    /// Sets the value assigned to this explanation node.
    virtual void setValue(double value);

    /// A description of this explanation node.
    virtual String getDescription();

    /// Sets the description of this explanation node.
    virtual void setDescription(const String& description);

    /// The sub-nodes of this explanation node.
    virtual Collection<ExplanationPtr> getDetails();

    /// Adds a sub-node to this explanation node.
    virtual void addDetail(const ExplanationPtr& detail);

    /// Render an explanation as text.
    virtual String toString();

    /// Render an explanation as HTML.
    virtual String toHtml();

protected:
    /// A short one line summary which should contain all high level information about this Explanation,
    /// without the "Details"
    virtual String getSummary();

    virtual String toString(int32_t depth);
};

/// Small Util class used to pass both an idf factor as well as an explanation for that factor.
///
/// This class will likely be held on a {@link Weight}, so be aware before storing any large fields.
class IDFExplanation : public LuceneObject {
public:
    virtual ~IDFExplanation();
    LUCENE_CLASS(IDFExplanation);

public:
    /// @return the idf factor
    virtual double getIdf() = 0;

    /// This should be calculated lazily if possible.
    /// @return the explanation for the idf factor.
    virtual String explain() = 0;
};

}

#endif
