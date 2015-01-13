/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "fnord-fts/LuceneInc.h"
#include "fnord-fts/search/FieldDoc.h"

namespace Lucene {

FieldDoc::FieldDoc(int32_t doc, double score, Collection<ComparableValue> fields) : ScoreDoc(doc, score) {
    this->fields = fields;
}

FieldDoc::~FieldDoc() {
}

String FieldDoc::toString() {
    StringStream buffer;
    buffer << ScoreDoc::toString() << L"[";
    for (Collection<ComparableValue>::iterator field = fields.begin(); field != fields.end(); ++field) {
        if (field != fields.begin()) {
            buffer << L", ";
        }
        buffer << *field;
    }
    buffer << L"]";
    return buffer.str();
}

}
