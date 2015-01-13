/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "TokenStream.h"

namespace Lucene {

TokenStream::TokenStream() {
}

TokenStream::TokenStream(const AttributeSourcePtr& input) : AttributeSource(input) {
}

TokenStream::TokenStream(const AttributeFactoryPtr& factory) : AttributeSource(factory) {
}

TokenStream::~TokenStream() {
}

void TokenStream::end() {
    // do nothing by default
}

void TokenStream::reset() {
}

void TokenStream::close() {
}

}
