/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "DocValues.h"
#include "Explanation.h"
#include "MiscUtils.h"
#include "StringUtils.h"
#include "VariantUtils.h"

namespace Lucene {

DocValues::DocValues() {
    minVal = std::numeric_limits<double>::quiet_NaN();
    maxVal = std::numeric_limits<double>::quiet_NaN();
    avgVal = std::numeric_limits<double>::quiet_NaN();
    computed = false;
}

DocValues::~DocValues() {
}

int32_t DocValues::intVal(int32_t doc) {
    return (int32_t)doubleVal(doc);
}

int64_t DocValues::longVal(int32_t doc) {
    return (int64_t)doubleVal(doc);
}

String DocValues::strVal(int32_t doc) {
    return StringUtils::toString(doubleVal(doc));
}

ExplanationPtr DocValues::explain(int32_t doc) {
    return newLucene<Explanation>(doubleVal(doc), toString(doc));
}

CollectionValue DocValues::getInnerArray() {
    boost::throw_exception(UnsupportedOperationException(L"This optional method is for test purposes only"));
    return VariantUtils::null();
}

void DocValues::compute() {
    if (computed) {
        return;
    }
    double sum = 0;
    int32_t n = 0;
    while (true) {
        double val;
        try {
            val = doubleVal(n);
        } catch (IndexOutOfBoundsException&) {
            break;
        }
        sum += val;
        minVal = MiscUtils::isNaN(minVal) ? val : std::min(minVal, val);
        maxVal = MiscUtils::isNaN(maxVal) ? val : std::max(maxVal, val);
        ++n;
    }

    avgVal = n == 0 ? std::numeric_limits<double>::quiet_NaN() : sum / (double)n;
    computed = true;
}

double DocValues::getMinValue() {
    compute();
    return minVal;
}

double DocValues::getMaxValue() {
    compute();
    return maxVal;
}

double DocValues::getAverageValue() {
    compute();
    return avgVal;
}

}
