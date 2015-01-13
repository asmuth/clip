/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "AveragePayloadFunction.h"
#include "MiscUtils.h"
#include "StringUtils.h"

namespace Lucene {

AveragePayloadFunction::~AveragePayloadFunction() {
}

double AveragePayloadFunction::currentScore(int32_t docId, const String& field, int32_t start, int32_t end,
        int32_t numPayloadsSeen, double currentScore, double currentPayloadScore) {
    return currentPayloadScore + currentScore;
}

double AveragePayloadFunction::docScore(int32_t docId, const String& field, int32_t numPayloadsSeen, double payloadScore) {
    return numPayloadsSeen > 0 ? (payloadScore / (double)numPayloadsSeen) : 1.0;
}

int32_t AveragePayloadFunction::hashCode() {
    int32_t prime = 31;
    int32_t result = 1;
    result = prime * result + StringUtils::hashCode(getClassName());
    return result;
}

bool AveragePayloadFunction::equals(const LuceneObjectPtr& other) {
    if (LuceneObject::equals(other)) {
        return true;
    }
    if (!other) {
        return false;
    }
    if (!MiscUtils::equalTypes(shared_from_this(), other)) {
        return false;
    }
    return true;
}

}
