/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "fnord-fts/fts.h"
#include "fnord-fts/search/NumericRangeQuery.h"
#include "fnord-fts/search/_NumericRangeQuery.h"
#include "fnord-fts/index/Term.h"
#include "fnord-fts/index/IndexReader.h"
#include "fnord-fts/util/MiscUtils.h"
#include "fnord-fts/util/StringUtils.h"
#include "fnord-fts/util/VariantUtils.h"

namespace fnord {
namespace fts {

NumericRangeQuery::NumericRangeQuery(const String& field, int32_t precisionStep, int32_t valSize, NumericValue min, NumericValue max, bool minInclusive, bool maxInclusive) {
    BOOST_ASSERT(valSize == 32 || valSize == 64);
    if (precisionStep < 1) {
        boost::throw_exception(IllegalArgumentException(L"precisionStep must be >=1"));
    }
    this->field = field;
    this->precisionStep = precisionStep;
    this->valSize = valSize;
    this->min = min;
    this->max = max;
    this->minInclusive = minInclusive;
    this->maxInclusive = maxInclusive;

    // For bigger precisionSteps this query likely hits too many terms, so set to CONSTANT_SCORE_FILTER
    // right off (especially as the FilteredTermEnum is costly if wasted only for AUTO tests because it
    // creates new enums from IndexReader for each sub-range)
    switch (valSize) {
    case 64:
        setRewriteMethod(precisionStep > 6 ? CONSTANT_SCORE_FILTER_REWRITE() : CONSTANT_SCORE_AUTO_REWRITE_DEFAULT());
        break;
    case 32:
        setRewriteMethod(precisionStep > 8 ? CONSTANT_SCORE_FILTER_REWRITE() : CONSTANT_SCORE_AUTO_REWRITE_DEFAULT());
        break;
    default:
        // should never happen
        boost::throw_exception(IllegalArgumentException(L"valSize must be 32 or 64"));
    }

    // shortcut if upper bound == lower bound
    if (!VariantUtils::isNull(min) && min == max) {
        setRewriteMethod(CONSTANT_SCORE_BOOLEAN_QUERY_REWRITE());
    }
}

NumericRangeQuery::~NumericRangeQuery() {
}

NumericRangeQueryPtr NumericRangeQuery::newLongRange(const String& field, int32_t precisionStep, int64_t min, int64_t max, bool minInclusive, bool maxInclusive) {
    return newNumericRange(field, precisionStep, min, max, minInclusive, maxInclusive);
}

NumericRangeQueryPtr NumericRangeQuery::newLongRange(const String& field, int64_t min, int64_t max, bool minInclusive, bool maxInclusive) {
    return newNumericRange(field, min, max, minInclusive, maxInclusive);
}

NumericRangeQueryPtr NumericRangeQuery::newIntRange(const String& field, int32_t precisionStep, int32_t min, int32_t max, bool minInclusive, bool maxInclusive) {
    return newNumericRange(field, precisionStep, min, max, minInclusive, maxInclusive);
}

NumericRangeQueryPtr NumericRangeQuery::newIntRange(const String& field, int32_t min, int32_t max, bool minInclusive, bool maxInclusive) {
    return newNumericRange(field, min, max, minInclusive, maxInclusive);
}

NumericRangeQueryPtr NumericRangeQuery::newDoubleRange(const String& field, int32_t precisionStep, double min, double max, bool minInclusive, bool maxInclusive) {
    return newNumericRange(field, precisionStep, min, max, minInclusive, maxInclusive);
}

NumericRangeQueryPtr NumericRangeQuery::newDoubleRange(const String& field, double min, double max, bool minInclusive, bool maxInclusive) {
    return newNumericRange(field, min, max, minInclusive, maxInclusive);
}

NumericRangeQueryPtr NumericRangeQuery::newNumericRange(const String& field, int32_t precisionStep, NumericValue min, NumericValue max, bool minInclusive, bool maxInclusive) {
    if (!VariantUtils::equalsType(min, max)) {
        boost::throw_exception(IllegalArgumentException(L"min/max must be of the same type"));
    }
    int32_t valSize = VariantUtils::typeOf<int32_t>(min) ? 32 : 64;
    return newLucene<NumericRangeQuery>(field, precisionStep, valSize, min, max, minInclusive, maxInclusive);
}

NumericRangeQueryPtr NumericRangeQuery::newNumericRange(const String& field, NumericValue min, NumericValue max, bool minInclusive, bool maxInclusive) {
    return newNumericRange(field, NumericUtils::PRECISION_STEP_DEFAULT, min, max, minInclusive, maxInclusive);
}

FilteredTermEnumPtr NumericRangeQuery::getEnum(const IndexReaderPtr& reader) {
    return newLucene<NumericRangeTermEnum>(shared_from_this(), reader);
}

String NumericRangeQuery::getField() {
    return field;
}

bool NumericRangeQuery::includesMin() {
    return minInclusive;
}

bool NumericRangeQuery::includesMax() {
    return maxInclusive;
}

NumericValue NumericRangeQuery::getMin() {
    return min;
}

NumericValue NumericRangeQuery::getMax() {
    return min;
}

LuceneObjectPtr NumericRangeQuery::clone(const LuceneObjectPtr& other) {
    LuceneObjectPtr clone = MultiTermQuery::clone(other ? other : newLucene<NumericRangeQuery>(field, precisionStep, valSize, min, max, minInclusive, maxInclusive));
    NumericRangeQueryPtr cloneQuery(std::dynamic_pointer_cast<NumericRangeQuery>(clone));
    cloneQuery->field = field;
    cloneQuery->precisionStep = precisionStep;
    cloneQuery->valSize = valSize;
    cloneQuery->min = min;
    cloneQuery->max = max;
    cloneQuery->minInclusive = minInclusive;
    cloneQuery->maxInclusive = maxInclusive;
    return cloneQuery;
}

String NumericRangeQuery::toString(const String& field) {
    StringStream buffer;
    if (this->field != field) {
        buffer << this->field << L":";
    }
    buffer << (minInclusive ? L"[" : L"{");
    if (VariantUtils::isNull(min)) {
        buffer << L"*";
    } else {
        buffer << min;
    }
    buffer << L" TO ";
    if (VariantUtils::isNull(max)) {
        buffer << L"*";
    } else {
        buffer << max;
    }
    buffer << (maxInclusive ? L"]" : L"}");
    buffer << boostString();
    return buffer.str();
}

bool NumericRangeQuery::equals(const LuceneObjectPtr& other) {
    if (LuceneObject::equals(other)) {
        return true;
    }
    if (!MultiTermQuery::equals(other)) {
        return false;
    }

    NumericRangeQueryPtr otherNumericRangeQuery(std::dynamic_pointer_cast<NumericRangeQuery>(other));
    if (!otherNumericRangeQuery) {
        return false;
    }

    return (field == otherNumericRangeQuery->field &&
            min == otherNumericRangeQuery->min &&
            max == otherNumericRangeQuery->max &&
            minInclusive == otherNumericRangeQuery->minInclusive &&
            maxInclusive == otherNumericRangeQuery->maxInclusive &&
            precisionStep == otherNumericRangeQuery->precisionStep);
}

int32_t NumericRangeQuery::hashCode() {
    int32_t hash = MultiTermQuery::hashCode();
    hash += StringUtils::hashCode(field) ^ 0x4565fd66 + precisionStep ^ 0x64365465;
    if (!VariantUtils::isNull(min)) {
        hash += VariantUtils::hashCode(min) ^ 0x14fa55fb;
    }
    if (!VariantUtils::isNull(max)) {
        hash += VariantUtils::hashCode(max) ^ 0x733fa5fe;
    }
    return hash + (MiscUtils::hashCode(minInclusive) ^ 0x14fa55fb) + (MiscUtils::hashCode(maxInclusive) ^ 0x733fa5fe);
}

NumericRangeTermEnum::NumericRangeTermEnum(const NumericRangeQueryPtr& query, const IndexReaderPtr& reader) {
    this->_query = query;
    this->reader = reader;
    this->rangeBounds = Collection<String>::newInstance();
    this->termTemplate = newLucene<Term>(query->field);

    switch (query->valSize) {
    case 64: {
        // lower
        int64_t minBound = std::numeric_limits<int64_t>::min();
        if (VariantUtils::typeOf<int64_t>(query->min)) {
            minBound = VariantUtils::get<int64_t>(query->min);
        } else if (VariantUtils::typeOf<double>(query->min)) {
            minBound = NumericUtils::doubleToSortableLong(VariantUtils::get<double>(query->min));
        }
        if (!query->minInclusive && !VariantUtils::isNull(query->min)) {
            if (minBound == std::numeric_limits<int64_t>::max()) {
                break;
            }
            ++minBound;
        }

        // upper
        int64_t maxBound = std::numeric_limits<int64_t>::max();
        if (VariantUtils::typeOf<int64_t>(query->max)) {
            maxBound = VariantUtils::get<int64_t>(query->max);
        } else if (VariantUtils::typeOf<double>(query->max)) {
            maxBound = NumericUtils::doubleToSortableLong(VariantUtils::get<double>(query->max));
        }
        if (!query->maxInclusive && !VariantUtils::isNull(query->max)) {
            if (maxBound == std::numeric_limits<int64_t>::min()) {
                break;
            }
            --maxBound;
        }

        NumericUtils::splitLongRange(newLucene<NumericLongRangeBuilder>(rangeBounds), query->precisionStep, minBound, maxBound);

        break;
    }

    case 32: {
        // lower
        int32_t minBound = INT_MIN;
        if (VariantUtils::typeOf<int32_t>(query->min)) {
            minBound = VariantUtils::get<int32_t>(query->min);
        }
        if (!query->minInclusive && !VariantUtils::isNull(query->min)) {
            if (minBound == INT_MAX) {
                break;
            }
            ++minBound;
        }

        // upper
        int32_t maxBound = INT_MAX;
        if (VariantUtils::typeOf<int32_t>(query->max)) {
            maxBound = VariantUtils::get<int32_t>(query->max);
        }
        if (!query->maxInclusive && !VariantUtils::isNull(query->max)) {
            if (maxBound == INT_MIN) {
                break;
            }
            --maxBound;
        }

        NumericUtils::splitIntRange(newLucene<NumericIntRangeBuilder>(rangeBounds), query->precisionStep, minBound, maxBound);

        break;
    }

    default:
        // should never happen
        boost::throw_exception(IllegalArgumentException(L"valSize must be 32 or 64"));
    }

    // seek to first term
    next();
}

NumericRangeTermEnum::~NumericRangeTermEnum() {
}

double NumericRangeTermEnum::difference() {
    return 1.0;
}

bool NumericRangeTermEnum::endEnum() {
    boost::throw_exception(UnsupportedOperationException(L"not implemented"));
    return false;
}

void NumericRangeTermEnum::setEnum(const TermEnumPtr& actualEnum) {
    boost::throw_exception(UnsupportedOperationException(L"not implemented"));
}

bool NumericRangeTermEnum::termCompare(const TermPtr& term) {
    return (term->field() == NumericRangeQueryPtr(_query)->field && term->text().compare(currentUpperBound) <= 0);
}

bool NumericRangeTermEnum::next() {
    // if a current term exists, the actual enum is initialized: try change to next term, if no
    // such term exists, fall-through
    if (currentTerm) {
        BOOST_ASSERT(actualEnum);
        if (actualEnum->next()) {
            currentTerm = actualEnum->term();
            if (termCompare(currentTerm)) {
                return true;
            }
        }
    }

    // if all above fails, we go forward to the next enum, if one is available
    currentTerm.reset();
    while (rangeBounds.size() >= 2) {
        BOOST_ASSERT(rangeBounds.size() % 2 == 0);
        // close the current enum and read next bounds
        if (actualEnum) {
            actualEnum->close();
            actualEnum.reset();
        }
        String lowerBound(rangeBounds.removeFirst());
        currentUpperBound = rangeBounds.removeFirst();
        // create a new enum
        actualEnum = reader->terms(termTemplate->createTerm(lowerBound));
        currentTerm = actualEnum->term();
        if (currentTerm && termCompare(currentTerm)) {
            return true;
        }
        // clear the current term for next iteration
        currentTerm.reset();
    }

    // no more sub-range enums available
    BOOST_ASSERT(rangeBounds.empty() && !currentTerm);
    return false;
}

void NumericRangeTermEnum::close() {
    rangeBounds.clear();
    currentUpperBound.clear();
    FilteredTermEnum::close();
}

NumericLongRangeBuilder::NumericLongRangeBuilder(Collection<String> rangeBounds) {
    this->rangeBounds = rangeBounds;
}

NumericLongRangeBuilder::~NumericLongRangeBuilder() {
}

void NumericLongRangeBuilder::addRange(const String& minPrefixCoded, const String& maxPrefixCoded) {
    rangeBounds.add(minPrefixCoded);
    rangeBounds.add(maxPrefixCoded);
}

NumericIntRangeBuilder::NumericIntRangeBuilder(Collection<String> rangeBounds) {
    this->rangeBounds = rangeBounds;
}

NumericIntRangeBuilder::~NumericIntRangeBuilder() {
}

void NumericIntRangeBuilder::addRange(const String& minPrefixCoded, const String& maxPrefixCoded) {
    rangeBounds.add(minPrefixCoded);
    rangeBounds.add(maxPrefixCoded);
}

}

}
