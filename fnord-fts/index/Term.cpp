/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "Term.h"
#include "MiscUtils.h"
#include "StringUtils.h"

namespace Lucene {

Term::Term(const String& fld, const String& txt) : _field(fld), _text(txt) {
}

Term::~Term() {
}

String Term::field() {
    return _field;
}

String Term::text() {
    return _text;
}

TermPtr Term::createTerm(const String& text) {
    return newLucene<Term>(_field, text);
}

bool Term::equals(const LuceneObjectPtr& other) {
    if (LuceneObject::equals(other)) {
        return true;
    }
    if (!other) {
        return false;
    }
    if (!MiscUtils::equalTypes(shared_from_this(), other)) {
        return false;
    }
    TermPtr otherTerm(boost::dynamic_pointer_cast<Term>(other));
    if (!otherTerm) {
        return false;
    }
    return (_field == otherTerm->_field && _text == otherTerm->_text);
}

int32_t Term::hashCode() {
    int32_t prime = 31;
    int32_t result = 1;
    result = prime * result + (_field.empty() ? 0 : StringUtils::hashCode(_field));
    result = prime * result + (_text.empty() ? 0 : StringUtils::hashCode(_text));
    return result;
}

int32_t Term::compareTo(const LuceneObjectPtr& other) {
    TermPtr otherTerm(boost::static_pointer_cast<Term>(other));
    if (_field == otherTerm->_field) {
        return _text.compare(otherTerm->_text);
    } else {
        return _field.compare(otherTerm->_field);
    }
}

void Term::set(const String& fld, const String& txt) {
    _field = fld;
    _text = txt;
}

String Term::toString() {
    return _field + L":" + _text;
}

}
