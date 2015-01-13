/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "fnord-fts/LuceneInc.h"
#include "fnord-fts/SingleTermEnum.h"
#include "fnord-fts/IndexReader.h"
#include "fnord-fts/Term.h"

namespace Lucene {

SingleTermEnum::SingleTermEnum(const IndexReaderPtr& reader, const TermPtr& singleTerm) {
    this->_endEnum = false;
    this->singleTerm = singleTerm;
    setEnum(reader->terms(singleTerm));
}

SingleTermEnum::~SingleTermEnum() {
}

double SingleTermEnum::difference() {
    return 1.0;
}

bool SingleTermEnum::endEnum() {
    return _endEnum;
}

bool SingleTermEnum::termCompare(const TermPtr& term) {
    if (term->equals(singleTerm)) {
        return true;
    }
    _endEnum = true;
    return false;
}

}
