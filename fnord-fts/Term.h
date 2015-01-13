/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef TERM_H
#define TERM_H

#include "LuceneObject.h"

namespace Lucene {

/// A Term represents a word from text.  This is the unit of search.  It is composed of two elements,
/// the text of the word, as a string, and the name of the field that the text occurred in, an interned
/// string.
///
/// Note that terms may represent more than words from text fields, but also things like dates, email
/// addresses, urls, etc.
class Term : public LuceneObject {
public:
    /// Constructs a Term with the given field and text.
    Term(const String& fld, const String& txt = EmptyString);
    virtual ~Term();

    LUCENE_CLASS(Term);

public:
    String _field;
    String _text;

public:
    /// Returns the field of this term, an interned string.   The field indicates the part of a document
    /// which this term came from.
    String field();

    /// Returns the text of this term.  In the case of words, this is simply the text of the word.  In
    /// the case of dates and other types, this is an encoding of the object as a string.
    String text();

    /// Optimized construction of new Terms by reusing same field as this Term
    /// @param text The text of the new term (field is implicitly same as this Term instance)
    /// @return A new Term
    TermPtr createTerm(const String& text);

    virtual bool equals(const LuceneObjectPtr& other);
    virtual int32_t hashCode();

    /// Compares two terms, returning a negative integer if this term belongs before the argument, zero
    /// if this term is equal to the argument, and a positive integer if this term belongs after the argument.
    ///
    /// The ordering of terms is first by field, then by text.
    virtual int32_t compareTo(const LuceneObjectPtr& other);

    void set(const String& fld, const String& txt);

    virtual String toString();
};

}

#endif
