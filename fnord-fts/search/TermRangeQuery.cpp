/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "fnord-fts/LuceneInc.h"
#include "fnord-fts/search/TermRangeQuery.h"
#include "fnord-fts/search/TermRangeTermEnum.h"
#include "fnord-fts/search/Collator.h"
#include "fnord-fts/util/StringUtils.h"
#include "fnord-fts/util/VariantUtils.h"

namespace Lucene {

TermRangeQuery::TermRangeQuery(const String& fieldName, StringValue lowerTerm, StringValue upperTerm, bool includeLower,
                               bool includeUpper, CollatorPtr collator) {
    this->field = fieldName;
    this->lowerTerm = lowerTerm;
    this->upperTerm = upperTerm;
    this->includeLower = includeLower;
    this->includeUpper = includeUpper;
    this->collator = collator;
}

TermRangeQuery::~TermRangeQuery() {
}

String TermRangeQuery::getField() {
    return field;
}

String TermRangeQuery::getLowerTerm() {
    return VariantUtils::get<String>(lowerTerm);
}

String TermRangeQuery::getUpperTerm() {
    return VariantUtils::get<String>(upperTerm);
}

bool TermRangeQuery::includesLower() {
    return includeLower;
}

bool TermRangeQuery::includesUpper() {
    return includeUpper;
}

CollatorPtr TermRangeQuery::getCollator() {
    return collator;
}

FilteredTermEnumPtr TermRangeQuery::getEnum(const IndexReaderPtr& reader) {
    return newLucene<TermRangeTermEnum>(reader, field, lowerTerm, upperTerm, includeLower, includeUpper, collator);
}

LuceneObjectPtr TermRangeQuery::clone(const LuceneObjectPtr& other) {
    LuceneObjectPtr clone = MultiTermQuery::clone(other ? other : newLucene<TermRangeQuery>(field, lowerTerm, upperTerm, includeLower, includeUpper, collator));
    TermRangeQueryPtr cloneQuery(std::dynamic_pointer_cast<TermRangeQuery>(clone));
    cloneQuery->lowerTerm = lowerTerm;
    cloneQuery->upperTerm = upperTerm;
    cloneQuery->collator = collator;
    cloneQuery->field = field;
    cloneQuery->includeLower = includeLower;
    cloneQuery->includeUpper = includeUpper;
    return cloneQuery;
}

String TermRangeQuery::toString(const String& field) {
    StringStream buffer;
    if (getField() != field) {
        buffer << getField() << L":";
    }
    buffer << (includeLower ? L"[" : L"{");
    if (VariantUtils::isNull(lowerTerm)) {
        buffer << L"*";
    } else {
        buffer << lowerTerm;
    }
    buffer << L" TO ";
    if (VariantUtils::isNull(upperTerm)) {
        buffer << L"*";
    } else {
        buffer << upperTerm;
    }
    buffer << (includeUpper ? L"]" : L"}");
    buffer << boostString();
    return buffer.str();
}

bool TermRangeQuery::equals(const LuceneObjectPtr& other) {
    if (LuceneObject::equals(other)) {
        return true;
    }
    if (!MultiTermQuery::equals(other)) {
        return false;
    }
    if (!MiscUtils::equalTypes(shared_from_this(), other)) {
        return false;
    }
    TermRangeQueryPtr otherQuery(std::dynamic_pointer_cast<TermRangeQuery>(other));
    if (!otherQuery) {
        return false;
    }
    if (!collator) {
        if (otherQuery->collator) {
            return false;
        }
    } else if (!collator->equals(otherQuery->collator)) {
        return false;
    }
    if (field != otherQuery->field) {
        return false;
    }
    if (includeLower != otherQuery->includeLower) {
        return false;
    }
    if (includeUpper != otherQuery->includeUpper) {
        return false;
    }
    if (VariantUtils::isNull(lowerTerm)) {
        if (!VariantUtils::isNull(otherQuery->lowerTerm)) {
            return false;
        }
    } else if (!VariantUtils::equals(lowerTerm, otherQuery->lowerTerm)) {
        return false;
    }
    if (VariantUtils::isNull(upperTerm)) {
        if (!VariantUtils::isNull(otherQuery->upperTerm)) {
            return false;
        }
    } else if (!VariantUtils::equals(upperTerm, otherQuery->upperTerm)) {
        return false;
    }
    return true;
}

int32_t TermRangeQuery::hashCode() {
    int32_t prime = 31;
    int32_t result = MultiTermQuery::hashCode();
    result = prime * result + (collator ? collator->hashCode() : 0);
    result = prime * result + (field.empty() ? 0 : StringUtils::hashCode(field));
    result = prime * result + (includeLower ? 1231 : 1237);
    result = prime * result + (includeUpper ? 1231 : 1237);
    result = prime * result + (VariantUtils::isNull(lowerTerm) ? 0 : StringUtils::hashCode(VariantUtils::get<String>(lowerTerm)));
    result = prime * result + (VariantUtils::isNull(upperTerm) ? 0 : StringUtils::hashCode(VariantUtils::get<String>(upperTerm)));
    return result;
}

}
