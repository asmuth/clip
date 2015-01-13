/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "fnord-fts/LuceneInc.h"
#include "fnord-fts/search/TermRangeTermEnum.h"
#include "fnord-fts/index/IndexReader.h"
#include "fnord-fts/search/Term.h"
#include "fnord-fts/search/Collator.h"
#include "fnord-fts/util/StringUtils.h"
#include "fnord-fts/search/VariantUtils.h"

namespace Lucene {

TermRangeTermEnum::TermRangeTermEnum(const IndexReaderPtr& reader, const String& field, StringValue lowerTermText,
                                     StringValue upperTermText, bool includeLower, bool includeUpper, const CollatorPtr& collator) {
    this->collator = collator;
    this->_endEnum = false;
    this->upperTermText = upperTermText;
    this->lowerTermText = lowerTermText;
    this->includeLower = includeLower;
    this->includeUpper = includeUpper;
    this->field = field;

    // do a little bit of normalization: open ended range queries should always be inclusive.
    if (VariantUtils::isNull(this->lowerTermText)) {
        this->includeLower = true;
    }

    if (VariantUtils::isNull(this->upperTermText)) {
        this->includeUpper = true;
    }

    String startTermText(collator ? L"" : VariantUtils::get<String>(this->lowerTermText));
    setEnum(reader->terms(newLucene<Term>(this->field, startTermText)));
}

TermRangeTermEnum::~TermRangeTermEnum() {
}

double TermRangeTermEnum::difference() {
    return 1.0;
}

bool TermRangeTermEnum::endEnum() {
    return _endEnum;
}

bool TermRangeTermEnum::termCompare(const TermPtr& term) {
    if (!collator) {
        // Use Unicode code point ordering
        bool checkLower = false;
        if (!includeLower) { // make adjustments to set to exclusive
            checkLower = true;
        }
        if (term && term->field() == field) {
            if (!checkLower || VariantUtils::isNull(lowerTermText) || term->text().compare(VariantUtils::get<String>(lowerTermText)) > 0) {
                checkLower = false;
                if (!VariantUtils::isNull(upperTermText)) {
                    int32_t compare = VariantUtils::get<String>(upperTermText).compare(term->text());
                    // if beyond the upper term, or is exclusive and this is equal to the upper term, break out
                    if (compare < 0 || (!includeUpper && compare == 0)) {
                        _endEnum = true;
                        return false;
                    }
                }
                return true;
            }
        } else {
            // break
            _endEnum = true;
            return false;
        }
        return false;
    } else {
        if (term && term->field() == field) {
            if ((VariantUtils::isNull(lowerTermText) ||
                    (includeLower ? collator->compare(term->text(), VariantUtils::get<String>(lowerTermText)) >= 0 :
                     collator->compare(term->text(), VariantUtils::get<String>(lowerTermText)) > 0)) &&
                    (VariantUtils::isNull(upperTermText) ||
                     (includeUpper ? collator->compare(term->text(), VariantUtils::get<String>(upperTermText)) <= 0 :
                      collator->compare(term->text(), VariantUtils::get<String>(upperTermText)) < 0))) {
                return true;
            }
            return false;
        }
        _endEnum = true;
        return false;
    }
}

}
