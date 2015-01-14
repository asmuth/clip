/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "fnord-fts/fts.h"
#include "fnord-fts/search/FilteredTermEnum.h"

namespace fnord {
namespace fts {

FilteredTermEnum::~FilteredTermEnum() {
}

void FilteredTermEnum::setEnum(const TermEnumPtr& actualEnum) {
    this->actualEnum = actualEnum;
    // Find the first term that matches
    TermPtr term(actualEnum->term());
    if (term && termCompare(term)) {
        currentTerm = term;
    } else {
        next();
    }
}

int32_t FilteredTermEnum::docFreq() {
    if (!currentTerm) {
        return -1;
    }
    BOOST_ASSERT(actualEnum);
    return actualEnum->docFreq();
}

bool FilteredTermEnum::next() {
    if (!actualEnum) {
        return false;    // the actual enumerator is not initialized
    }
    currentTerm.reset();
    while (!currentTerm) {
        if (endEnum()) {
            return false;
        }
        if (actualEnum->next()) {
            TermPtr term(actualEnum->term());
            if (termCompare(term)) {
                currentTerm = term;
                return true;
            }
        } else {
            return false;
        }
    }
    currentTerm.reset();
    return false;
}

TermPtr FilteredTermEnum::term() {
    return currentTerm;
}

void FilteredTermEnum::close() {
    if (actualEnum) {
        actualEnum->close();
    }
    currentTerm.reset();
    actualEnum.reset();
}

}

}
