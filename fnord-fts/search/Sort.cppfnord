/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "fnord-fts/LuceneInc.h"
#include "fnord-fts/Sort.h"
#include "fnord-fts/SortField.h"
#include "fnord-fts/MiscUtils.h"

namespace Lucene {

Sort::Sort() {
    setSort(SortField::FIELD_SCORE());
}

Sort::Sort(const SortFieldPtr& field) {
    setSort(field);
}

Sort::Sort(Collection<SortFieldPtr> fields) {
    setSort(fields);
}

Sort::~Sort() {
}

SortPtr Sort::RELEVANCE() {
    static SortPtr _RELEVANCE;
    if (!_RELEVANCE) {
        _RELEVANCE = newLucene<Sort>();
        CycleCheck::addStatic(_RELEVANCE);
    }
    return _RELEVANCE;
}

SortPtr Sort::INDEXORDER() {
    static SortPtr _INDEXORDER;
    if (!_INDEXORDER) {
        _INDEXORDER = newLucene<Sort>(SortField::FIELD_DOC());
        CycleCheck::addStatic(_INDEXORDER);
    }
    return _INDEXORDER;
}

void Sort::setSort(const SortFieldPtr& field) {
    this->fields = newCollection<SortFieldPtr>(field);
}

void Sort::setSort(Collection<SortFieldPtr> fields) {
    this->fields = fields;
}

Collection<SortFieldPtr> Sort::getSort() {
    return fields;
}

String Sort::toString() {
    StringStream buffer;
    for (Collection<SortFieldPtr>::iterator field = fields.begin(); field != fields.end(); ++field) {
        if (field != fields.begin()) {
            buffer << L",";
        }
        buffer << (*field)->toString();
    }
    return buffer.str();
}

bool Sort::equals(const LuceneObjectPtr& other) {
    if (LuceneObject::equals(other)) {
        return true;
    }

    SortPtr otherSort(std::dynamic_pointer_cast<Sort>(other));
    if (!otherSort) {
        return false;
    }
    return fields.equals(otherSort->fields);
}

int32_t Sort::hashCode() {
    return 0x45aaf665 + MiscUtils::hashCode(fields.begin(), fields.end(), MiscUtils::hashLucene<SortFieldPtr>);
}

}
