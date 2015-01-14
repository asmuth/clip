/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "fnord-fts/fts.h"
#include "fnord-fts/search/ScoreDoc.h"

namespace Lucene {

ScoreDoc::ScoreDoc(int32_t doc, double score) {
    this->doc = doc;
    this->score = score;
}

ScoreDoc::~ScoreDoc() {
}

String ScoreDoc::toString() {
    StringStream buffer;
    buffer << L"doc=" << doc << L" score=" << score;
    return buffer.str();
}

}
