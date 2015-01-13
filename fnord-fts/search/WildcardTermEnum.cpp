/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "fnord-fts/LuceneInc.h"
#include <boost/algorithm/string.hpp>
#include "fnord-fts/search/WildcardTermEnum.h"
#include "fnord-fts/index/Term.h"
#include "fnord-fts/index/IndexReader.h"

namespace Lucene {

const wchar_t WildcardTermEnum::WILDCARD_STRING = L'*';
const wchar_t WildcardTermEnum::WILDCARD_CHAR = L'?';

WildcardTermEnum::WildcardTermEnum(const IndexReaderPtr& reader, const TermPtr& term) {
    _endEnum = false;
    searchTerm = term;
    field = searchTerm->field();
    String searchTermText(searchTerm->text());

    String::size_type sidx = searchTermText.find(WILDCARD_STRING);
    String::size_type cidx = searchTermText.find(WILDCARD_CHAR);
    String::size_type idx = sidx;
    if (idx == String::npos) {
        idx = cidx;
    } else if (cidx != String::npos) {
        idx = std::min(idx, cidx);
    }
    pre = idx != String::npos ? searchTerm->text().substr(0, idx) : L"";

    preLen = pre.length();
    text = searchTermText.substr(preLen);
    setEnum(reader->terms(newLucene<Term>(searchTerm->field(), pre)));
}

WildcardTermEnum::~WildcardTermEnum() {
}

bool WildcardTermEnum::termCompare(const TermPtr& term) {
    if (field == term->field()) {
        String searchText(term->text());
        if (boost::starts_with(searchText, pre)) {
            return wildcardEquals(text, 0, searchText, preLen);
        }
    }
    _endEnum = true;
    return false;
}

double WildcardTermEnum::difference() {
    return 1.0;
}

bool WildcardTermEnum::endEnum() {
    return _endEnum;
}

bool WildcardTermEnum::wildcardEquals(const String& pattern, int32_t patternIdx, const String& string, int32_t stringIdx) {
    int32_t p = patternIdx;
    for (int32_t s = stringIdx; ; ++p, ++s) {
        // End of string yet?
        bool sEnd = (s >= (int32_t)string.length());
        // End of pattern yet?
        bool pEnd = (p >= (int32_t)pattern.length());

        // If we're looking at the end of the string
        if (sEnd) {
            // Assume the only thing left on the pattern is/are wildcards
            bool justWildcardsLeft = true;

            // Current wildcard position
            int32_t wildcardSearchPos = p;

            // While we haven't found the end of the pattern, and haven't encountered any non-wildcard characters
            while (wildcardSearchPos < (int32_t)pattern.length() && justWildcardsLeft) {
                // Check the character at the current position
                wchar_t wildchar = pattern[wildcardSearchPos];

                // If it's not a wildcard character, then there is more pattern information after this/these wildcards.
                if (wildchar != WILDCARD_CHAR && wildchar != WILDCARD_STRING) {
                    justWildcardsLeft = false;
                } else {
                    // to prevent "cat" matches "ca??"
                    if (wildchar == WILDCARD_CHAR) {
                        return false;
                    }
                    // Look at the next character
                    ++wildcardSearchPos;
                }
            }

            // This was a prefix wildcard search, and we've matched, so return true.
            if (justWildcardsLeft) {
                return true;
            }
        }

        // If we've gone past the end of the string, or the pattern, return false.
        if (sEnd || pEnd) {
            break;
        }

        // Match a single character, so continue.
        if (pattern[p] == WILDCARD_CHAR) {
            continue;
        }

        if (pattern[p] == WILDCARD_STRING) {
            // Look at the character beyond the '*' characters.
            while (p < (int32_t)pattern.length() && pattern[p] == WILDCARD_STRING) {
                ++p;
            }
            // Examine the string, starting at the last character.
            for (int32_t i = string.length(); i >= s; --i) {
                if (wildcardEquals(pattern, p, string, i)) {
                    return true;
                }
            }
            break;
        }
        if (pattern[p] != string[s]) {
            break;
        }
    }
    return false;
}

}
