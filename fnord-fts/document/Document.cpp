/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "Document.h"
#include "Fieldable.h"
#include "Field.h"

namespace Lucene {

Document::Document() {
    fields = Collection<FieldablePtr>::newInstance();
    boost = 1.0;
}

Document::~Document() {
}

void Document::setBoost(double boost) {
    this->boost = boost;
}

double Document::getBoost() {
    return boost;
}

void Document::add(const FieldablePtr& field) {
    fields.add(field);
}

/// Utility functor for comparing fieldable names.
/// see {@link Document}.
struct equalFieldableName {
    equalFieldableName(const String& name) : equalName(name) {}
    inline bool operator()(const FieldablePtr& other) const {
        return (equalName == other->name());
    }
    const String& equalName;
};

void Document::removeField(const String& name) {
    Collection<FieldablePtr>::iterator field = fields.find_if(equalFieldableName(name));
    if (field != fields.end()) {
        fields.remove(field);
    }
}

void Document::removeFields(const String& name) {
    fields.remove_if(equalFieldableName(name));
}

FieldPtr Document::getField(const String& name) {
    return boost::static_pointer_cast<Field>(getFieldable(name));
}

FieldablePtr Document::getFieldable(const String& name) {
    Collection<FieldablePtr>::iterator field = fields.find_if(equalFieldableName(name));
    return field == fields.end() ? FieldablePtr() : *field;
}

String Document::get(const String& name) {
    for (Collection<FieldablePtr>::iterator field = fields.begin(); field != fields.end(); ++field) {
        if ((*field)->name() == name && !(*field)->isBinary()) {
            return (*field)->stringValue();
        }
    }
    return L"";
}

Collection<FieldablePtr> Document::getFields() {
    return fields;
}

Collection<FieldPtr> Document::getFields(const String& name) {
    Collection<FieldPtr> result(Collection<FieldPtr>::newInstance());
    for (Collection<FieldablePtr>::iterator field = fields.begin(); field != fields.end(); ++field) {
        if ((*field)->name() == name) {
            result.add(boost::static_pointer_cast<Field>(*field));
        }
    }
    return result;
}

Collection<FieldablePtr> Document::getFieldables(const String& name) {
    Collection<FieldablePtr> result(Collection<FieldablePtr>::newInstance());
    for (Collection<FieldablePtr>::iterator field = fields.begin(); field != fields.end(); ++field) {
        if ((*field)->name() == name) {
            result.add(*field);
        }
    }
    return result;
}

Collection<String> Document::getValues(const String& name) {
    Collection<String> result(Collection<String>::newInstance());
    for (Collection<FieldablePtr>::iterator field = fields.begin(); field != fields.end(); ++field) {
        if ((*field)->name() == name && !(*field)->isBinary()) {
            result.add((*field)->stringValue());
        }
    }
    return result;
}

Collection<ByteArray> Document::getBinaryValues(const String& name) {
    Collection<ByteArray> result(Collection<ByteArray>::newInstance());
    for (Collection<FieldablePtr>::iterator field = fields.begin(); field != fields.end(); ++field) {
        if ((*field)->name() == name && (*field)->isBinary()) {
            result.add((*field)->getBinaryValue());
        }
    }
    return result;
}

ByteArray Document::getBinaryValue(const String& name) {
    for (Collection<FieldablePtr>::iterator field = fields.begin(); field != fields.end(); ++field) {
        if ((*field)->name() == name && (*field)->isBinary()) {
            return (*field)->getBinaryValue();
        }
    }
    return ByteArray();
}

String Document::toString() {
    StringStream buffer;
    buffer << L"Document<";
    for (Collection<FieldablePtr>::iterator field = fields.begin(); field != fields.end(); ++field) {
        if (field != fields.begin()) {
            buffer << L" ";
        }
        buffer << (*field)->stringValue();
    }
    buffer << L">";
    return buffer.str();
}

}
