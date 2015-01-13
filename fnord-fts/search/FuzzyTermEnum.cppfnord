/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "fnord-fts/LuceneInc.h"
#include <boost/algorithm/string.hpp>
#include "fnord-fts/FuzzyTermEnum.h"
#include "fnord-fts/FuzzyQuery.h"
#include "fnord-fts/Term.h"
#include "fnord-fts/IndexReader.h"

namespace Lucene {

FuzzyTermEnum::FuzzyTermEnum(const IndexReaderPtr& reader, const TermPtr& term, double minSimilarity, int32_t prefixLength) {
    ConstructTermEnum(reader, term, minSimilarity, prefixLength);
}

FuzzyTermEnum::FuzzyTermEnum(const IndexReaderPtr& reader, const TermPtr& term, double minSimilarity) {
    ConstructTermEnum(reader, term, minSimilarity, FuzzyQuery::defaultPrefixLength);
}

FuzzyTermEnum::FuzzyTermEnum(const IndexReaderPtr& reader, const TermPtr& term) {
    ConstructTermEnum(reader, term, FuzzyQuery::defaultMinSimilarity(), FuzzyQuery::defaultPrefixLength);
}

FuzzyTermEnum::~FuzzyTermEnum() {
}

void FuzzyTermEnum::ConstructTermEnum(const IndexReaderPtr& reader, const TermPtr& term, double minSimilarity, int32_t prefixLength) {
    if (minSimilarity >= 1.0) {
        boost::throw_exception(IllegalArgumentException(L"minimumSimilarity cannot be greater than or equal to 1"));
    } else if (minSimilarity < 0.0) {
        boost::throw_exception(IllegalArgumentException(L"minimumSimilarity cannot be less than 0"));
    }
    if (prefixLength < 0) {
        boost::throw_exception(IllegalArgumentException(L"prefixLength cannot be less than 0"));
    }

    this->minimumSimilarity = minSimilarity;
    this->scale_factor = 1.0 / (1.0 - minimumSimilarity);
    this->searchTerm = term;
    this->field = searchTerm->field();
    this->_endEnum = false;
    this->_similarity = 0.0;

    // The prefix could be longer than the word.
    // It's kind of silly though.  It means we must match the entire word.
    int32_t fullSearchTermLength = searchTerm->text().length();
    int32_t realPrefixLength = prefixLength > fullSearchTermLength ? fullSearchTermLength : prefixLength;

    this->text = searchTerm->text().substr(realPrefixLength);
    this->prefix = searchTerm->text().substr(0, realPrefixLength);

    this->p = Collection<int32_t>::newInstance(this->text.length() + 1);
    this->d = Collection<int32_t>::newInstance(this->text.length() + 1);

    setEnum(reader->terms(newLucene<Term>(searchTerm->field(), prefix)));
}

bool FuzzyTermEnum::termCompare(const TermPtr& term) {
    if (field == term->field() && boost::starts_with(term->text(), prefix)) {
        String target(term->text().substr(prefix.length()));
        this->_similarity = similarity(target);
        return (_similarity > minimumSimilarity);
    }
    _endEnum = true;
    return false;
}

double FuzzyTermEnum::difference() {
    return (_similarity - minimumSimilarity) * scale_factor;
}

bool FuzzyTermEnum::endEnum() {
    return _endEnum;
}

double FuzzyTermEnum::similarity(const String& target) {
    int32_t m = target.length();
    int32_t n = text.length();
    if (n == 0) {
        // We don't have anything to compare.  That means if we just add the letters for m we get the new word
        return prefix.empty() ? 0.0 : 1.0 - ((double)m / (double)prefix.length());
    }
    if (m == 0) {
        return prefix.empty() ? 0.0 : 1.0 - ((double)n / (double)prefix.length());
    }

    int32_t maxDistance = calculateMaxDistance(m);

    if (maxDistance < std::abs(m - n)) {
        // Just adding the characters of m to n or vice-versa results in too many edits for example "pre" length
        // is 3 and "prefixes" length is 8.  We can see that given this optimal circumstance, the edit distance
        // cannot be less than 5.  which is 8-3 or more precisely std::abs(3 - 8). if our maximum edit distance
        // is 4, then we can discard this word without looking at it.
        return 0.0;
    }

    // init matrix d
    for (int32_t i = 0; i <= n; ++i) {
        p[i] = i;
    }

    // start computing edit distance
    for (int32_t j = 1; j <= m; ++j) { // iterates through target
        int32_t bestPossibleEditDistance = m;
        wchar_t t_j = target[j - 1]; // jth character of t
        d[0] = j;

        for (int32_t i = 1; i <= n; ++i) { // iterates through text
            // minimum of cell to the left+1, to the top+1, diagonally left and up +(0|1)
            if (t_j != text[i - 1]) {
                d[i] = std::min(std::min(d[i - 1], p[i]), p[i - 1]) + 1;
            } else {
                d[i] = std::min(std::min(d[i - 1] + 1, p[i] + 1), p[i - 1]);
            }
            bestPossibleEditDistance = std::min(bestPossibleEditDistance, d[i]);
        }

        // After calculating row i, the best possible edit distance can be found by found by finding the smallest
        // value in a given column.  If the bestPossibleEditDistance is greater than the max distance, abort.

        if (j > maxDistance && bestPossibleEditDistance > maxDistance) { // equal is okay, but not greater
            // The closest the target can be to the text is just too far away.
            // This target is leaving the party early.
            return 0.0;
        }

        // copy current distance counts to 'previous row' distance counts: swap p and d
        std::swap(p, d);
    }

    // Our last action in the above loop was to switch d and p, so p now actually has the most recent cost counts

    // This will return less than 0.0 when the edit distance is greater than the number of characters in the shorter
    // word. But this was the formula that was previously used in FuzzyTermEnum, so it has not been changed (even
    // though minimumSimilarity must be greater than 0.0)
    return 1.0 - ((double)p[n] / (double)(prefix.length() + std::min(n, m)));
}

int32_t FuzzyTermEnum::calculateMaxDistance(int32_t m) {
    return (int32_t)((1.0 - minimumSimilarity) * (double)(std::min((int32_t)text.length(), m) + prefix.length()));
}

void FuzzyTermEnum::close() {
    p.reset();
    d.reset();
    searchTerm.reset();
    FilteredTermEnum::close(); // call FilteredTermEnum::close() and let the garbage collector do its work.
}

}
