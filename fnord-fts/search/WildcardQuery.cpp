/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "fnord-fts/fts.h"
#include <boost/algorithm/string.hpp>
#include "fnord-fts/search/WildcardQuery.h"
#include "fnord-fts/search/WildcardTermEnum.h"
#include "fnord-fts/index/Term.h"
#include "fnord-fts/search/PrefixQuery.h"
#include "fnord-fts/search/SingleTermEnum.h"
#include "fnord-fts/util/MiscUtils.h"

namespace Lucene {

WildcardQuery::WildcardQuery(const TermPtr& term) {
    this->term = term;
    String text(term->text());
    this->termContainsWildcard = boost::contains(text, L"*") || boost::contains(text, L"?");
    this->termIsPrefix = termContainsWildcard &&
                         !boost::contains(text, L"?") &&
                         text.find_first_of(L"*") == text.length() - 1;
}

WildcardQuery::~WildcardQuery() {
}

FilteredTermEnumPtr WildcardQuery::getEnum(const IndexReaderPtr& reader) {
    if (termContainsWildcard) {
        return newLucene<WildcardTermEnum>(reader, getTerm());
    } else {
        return newLucene<SingleTermEnum>(reader, getTerm());
    }
}

TermPtr WildcardQuery::getTerm() {
    return term;
}

QueryPtr WildcardQuery::rewrite(const IndexReaderPtr& reader) {
    if (termIsPrefix) {
        MultiTermQueryPtr rewritten(newLucene<PrefixQuery>(term->createTerm(term->text().substr(0, term->text().find('*')))));
        rewritten->setBoost(getBoost());
        rewritten->setRewriteMethod(getRewriteMethod());
        return rewritten;
    } else {
        return MultiTermQuery::rewrite(reader);
    }
}

String WildcardQuery::toString(const String& field) {
    StringStream buffer;
    if (term->field() != field) {
        buffer << term->field() << L":";
    }
    buffer << term->text() << boostString();
    return buffer.str();
}

LuceneObjectPtr WildcardQuery::clone(const LuceneObjectPtr& other) {
    LuceneObjectPtr clone = MultiTermQuery::clone(other ? other : newLucene<WildcardQuery>(term));
    WildcardQueryPtr cloneQuery(std::dynamic_pointer_cast<WildcardQuery>(clone));
    cloneQuery->termContainsWildcard = termContainsWildcard;
    cloneQuery->termIsPrefix = termIsPrefix;
    cloneQuery->term = term;
    return cloneQuery;
}

int32_t WildcardQuery::hashCode() {
    int32_t prime = 31;
    int32_t result = MultiTermQuery::hashCode();
    result = prime * result + (term ? term->hashCode() : 0);
    return result;
}

bool WildcardQuery::equals(const LuceneObjectPtr& other) {
    if (LuceneObject::equals(other)) {
        return true;
    }
    if (!MultiTermQuery::equals(other)) {
        return false;
    }
    if (!MiscUtils::equalTypes(shared_from_this(), other)) {
        return false;
    }
    WildcardQueryPtr otherWildcardQuery(std::dynamic_pointer_cast<WildcardQuery>(other));
    if (!otherWildcardQuery) {
        return false;
    }
    if (!term) {
        if (otherWildcardQuery->term) {
            return false;
        }
    } else if (!term->equals(otherWildcardQuery->term)) {
        return false;
    }
    return true;
}

}
