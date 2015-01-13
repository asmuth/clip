/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef WILDCARDTERMENUM_H
#define WILDCARDTERMENUM_H

#include "FilteredTermEnum.h"

namespace Lucene {

/// Subclass of FilteredTermEnum for enumerating all terms that match the specified wildcard filter term.
///
/// Term enumerations are always ordered by Term.compareTo().  Each term in the enumeration is greater than
/// all that precede it.
class WildcardTermEnum : public FilteredTermEnum {
public:
    /// Creates a new WildcardTermEnum.
    ///
    /// After calling the constructor the enumeration is already pointing to the first valid term if such
    /// a term exists.
    WildcardTermEnum(const IndexReaderPtr& reader, const TermPtr& term);

    virtual ~WildcardTermEnum();

    LUCENE_CLASS(WildcardTermEnum);

public:
    static const wchar_t WILDCARD_STRING;
    static const wchar_t WILDCARD_CHAR;

    TermPtr searchTerm;
    String field;
    String text;
    String pre;
    int32_t preLen;
    bool _endEnum;

public:
    virtual double difference();

    /// Determines if a word matches a wildcard pattern.
    static bool wildcardEquals(const String& pattern, int32_t patternIdx, const String& string, int32_t stringIdx);

protected:
    virtual bool termCompare(const TermPtr& term);
    virtual bool endEnum();
};

}

#endif
