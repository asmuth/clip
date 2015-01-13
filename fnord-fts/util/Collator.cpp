/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include <locale>
#include "Collator.h"

namespace Lucene {

Collator::Collator(std::locale locale) : collate(std::use_facet< std::collate<wchar_t> >(locale)) {
}

Collator::~Collator() {
}

int32_t Collator::compare(const String& first, const String& second) {
    return collate.compare(first.c_str(), first.c_str() + first.length(), second.c_str(), second.c_str() + second.length());
}

}
