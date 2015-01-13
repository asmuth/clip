/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef TERMRANGETERMENUM_H
#define TERMRANGETERMENUM_H

#include "fnord-fts/search/FilteredTermEnum.h"

namespace Lucene {

/// Subclass of FilteredTermEnum for enumerating all terms that match the specified range parameters.
///
/// Term enumerations are always ordered by Term.compareTo().  Each term in the enumeration is greater than
/// all that precede it.
class TermRangeTermEnum : public FilteredTermEnum {
public:
    /// Enumerates all terms greater/equal than lowerTerm but less/equal than upperTerm.
    ///
    /// If an endpoint is null, it is said to be "open". Either or both endpoints may be open.  Open endpoints
    /// may not be exclusive (you can't select all but the first or last term without explicitly specifying
    /// the term to exclude.)
    ///
    /// @param reader
    /// @param field An interned field that holds both lower and upper terms.
    /// @param lowerTermText The term text at the lower end of the range
    /// @param upperTermText The term text at the upper end of the range
    /// @param includeLower If true, the lowerTerm is included in the range.
    /// @param includeUpper If true, the upperTerm is included in the range.
    /// @param collator The collator to use to collate index Terms, to determine their membership in the range
    /// bounded by lowerTerm and upperTerm.
    TermRangeTermEnum(const IndexReaderPtr& reader, const String& field, StringValue lowerTermText, StringValue upperTermText,
                      bool includeLower, bool includeUpper, const CollatorPtr& collator);

    virtual ~TermRangeTermEnum();

    LUCENE_CLASS(TermRangeTermEnum);

protected:
    CollatorPtr collator;
    bool _endEnum;
    String field;
    StringValue upperTermText;
    StringValue lowerTermText;
    bool includeLower;
    bool includeUpper;

public:
    virtual double difference();

protected:
    virtual bool endEnum();
    virtual bool termCompare(const TermPtr& term);
};

}

#endif
