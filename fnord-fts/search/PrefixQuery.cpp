/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "fnord-fts/LuceneInc.h"
#include "fnord-fts/search/PrefixQuery.h"
#include "fnord-fts/search/PrefixTermEnum.h"
#include "fnord-fts/index/Term.h"
#include "fnord-fts/util/MiscUtils.h"

namespace Lucene {

PrefixQuery::PrefixQuery(const TermPtr& prefix) {
    this->prefix = prefix;
}

PrefixQuery::~PrefixQuery() {
}

TermPtr PrefixQuery::getPrefix() {
    return prefix;
}

FilteredTermEnumPtr PrefixQuery::getEnum(const IndexReaderPtr& reader) {
    return newLucene<PrefixTermEnum>(reader, prefix);
}

String PrefixQuery::toString(const String& field) {
    StringStream buffer;
    if (prefix->field() != field) {
        buffer << prefix->field() << L":";
    }
    buffer << prefix->text() << L"*" << boostString();
    return buffer.str();
}

LuceneObjectPtr PrefixQuery::clone(const LuceneObjectPtr& other) {
    LuceneObjectPtr clone = MultiTermQuery::clone(other ? other : newLucene<PrefixQuery>(prefix));
    PrefixQueryPtr cloneQuery(std::dynamic_pointer_cast<PrefixQuery>(clone));
    cloneQuery->prefix = prefix;
    return cloneQuery;
}

int32_t PrefixQuery::hashCode() {
    int32_t prime = 31;
    int32_t result = MultiTermQuery::hashCode();
    result = prime * result + (prefix ? prefix->hashCode() : 0);
    return result;
}

bool PrefixQuery::equals(const LuceneObjectPtr& other) {
    if (LuceneObject::equals(other)) {
        return true;
    }
    if (!MultiTermQuery::equals(other)) {
        return false;
    }
    if (!MiscUtils::equalTypes(shared_from_this(), other)) {
        return false;
    }
    PrefixQueryPtr otherPrefixQuery(std::dynamic_pointer_cast<PrefixQuery>(other));
    if (!otherPrefixQuery) {
        return false;
    }
    if (!prefix) {
        if (otherPrefixQuery->prefix) {
            return false;
        }
    } else if (!prefix->equals(otherPrefixQuery->prefix)) {
        return false;
    }
    return true;
}

}
