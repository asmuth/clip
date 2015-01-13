/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "fnord-fts/analysis/LuceneInc.h"
#include "fnord-fts/analysis/TokenFilter.h"

namespace Lucene {

TokenFilter::TokenFilter(const TokenStreamPtr& input) : TokenStream(input) {
    this->input = input;
}

TokenFilter::~TokenFilter() {
}

void TokenFilter::end() {
    input->end();
}

void TokenFilter::close() {
    input->close();
}

void TokenFilter::reset() {
    input->reset();
}

}
