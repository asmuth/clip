/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "fnord-fts/LuceneInc.h"
#include "fnord-fts/search/ComplexExplanation.h"
#include "fnord-fts/util/StringUtils.h"

namespace Lucene {

ComplexExplanation::ComplexExplanation(bool match, double value, const String& description) : Explanation(value, description) {
    this->match = match;
}

ComplexExplanation::~ComplexExplanation() {
}

bool ComplexExplanation::getMatch() {
    return match;
}

void ComplexExplanation::setMatch(bool match) {
    this->match = match;
}

bool ComplexExplanation::isMatch() {
    return getMatch();
}

String ComplexExplanation::getSummary() {
    return StringUtils::toString(getValue()) + L" = " + (isMatch() ? L"(MATCH) " : L"(NON-MATCH) ") + getDescription();
}

}
