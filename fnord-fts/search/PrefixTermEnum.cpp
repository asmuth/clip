/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "fnord-fts/LuceneInc.h"
#include <boost/algorithm/string.hpp>
#include "fnord-fts/search/PrefixTermEnum.h"
#include "fnord-fts/index/IndexReader.h"
#include "fnord-fts/index/Term.h"

namespace Lucene {

PrefixTermEnum::PrefixTermEnum(const IndexReaderPtr& reader, const TermPtr& prefix) {
    this->_endEnum = false;
    this->prefix = prefix;

    setEnum(reader->terms(newLucene<Term>(prefix->field(), prefix->text())));
}

PrefixTermEnum::~PrefixTermEnum() {
}

double PrefixTermEnum::difference() {
    return 1.0;
}

bool PrefixTermEnum::endEnum() {
    return _endEnum;
}

TermPtr PrefixTermEnum::getPrefixTerm() {
    return prefix;
}

bool PrefixTermEnum::termCompare(const TermPtr& term) {
    if (term->field() == prefix->field() && boost::starts_with(term->text(), prefix->text())) {
        return true;
    }
    _endEnum = true;
    return false;
}

}
