/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "fnord-fts/LuceneInc.h"
#include "fnord-fts/TermRangeFilter.h"
#include "fnord-fts/TermRangeQuery.h"
#include "fnord-fts/StringUtils.h"
#include "fnord-fts/VariantUtils.h"

namespace Lucene {

TermRangeFilter::TermRangeFilter(const String& fieldName, StringValue lowerTerm, StringValue upperTerm, bool includeLower,
                                 bool includeUpper, CollatorPtr collator) :
    MultiTermQueryWrapperFilter(newLucene<TermRangeQuery>(fieldName, lowerTerm, upperTerm,
                                includeLower, includeUpper, collator)) {
}

TermRangeFilter::~TermRangeFilter() {
}

TermRangeFilterPtr TermRangeFilter::Less(const String& fieldName, StringValue upperTerm) {
    return newLucene<TermRangeFilter>(fieldName, VariantUtils::null(), upperTerm, false, true);
}

TermRangeFilterPtr TermRangeFilter::More(const String& fieldName, StringValue lowerTerm) {
    return newLucene<TermRangeFilter>(fieldName, lowerTerm, VariantUtils::null(), true, false);
}

String TermRangeFilter::getField() {
    return std::static_pointer_cast<TermRangeQuery>(query)->getField();
}

String TermRangeFilter::getLowerTerm() {
    return std::static_pointer_cast<TermRangeQuery>(query)->getLowerTerm();
}

String TermRangeFilter::getUpperTerm() {
    return std::static_pointer_cast<TermRangeQuery>(query)->getUpperTerm();
}

bool TermRangeFilter::includesLower() {
    return std::static_pointer_cast<TermRangeQuery>(query)->includesLower();
}

bool TermRangeFilter::includesUpper() {
    return std::static_pointer_cast<TermRangeQuery>(query)->includesUpper();
}

CollatorPtr TermRangeFilter::getCollator() {
    return std::static_pointer_cast<TermRangeQuery>(query)->getCollator();
}

}
