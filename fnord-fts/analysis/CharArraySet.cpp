/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "CharArraySet.h"
#include "StringUtils.h"

namespace Lucene {

CharArraySet::CharArraySet(bool ignoreCase) {
    this->ignoreCase = ignoreCase;
    this->entries = HashSet<String>::newInstance();
}

CharArraySet::CharArraySet(HashSet<String> entries, bool ignoreCase) {
    this->ignoreCase = ignoreCase;
    this->entries = HashSet<String>::newInstance();
    if (entries) {
        for (HashSet<String>::iterator entry = entries.begin(); entry != entries.end(); ++entry) {
            add(*entry);
        }
    }
}

CharArraySet::CharArraySet(Collection<String> entries, bool ignoreCase) {
    this->ignoreCase = ignoreCase;
    this->entries = HashSet<String>::newInstance();
    if (entries) {
        for (Collection<String>::iterator entry = entries.begin(); entry != entries.end(); ++entry) {
            add(*entry);
        }
    }
}

CharArraySet::~CharArraySet() {
}

bool CharArraySet::contains(const String& text) {
    return entries.contains(ignoreCase ? StringUtils::toLower(text) : text);
}

bool CharArraySet::contains(const wchar_t* text, int32_t offset, int32_t length) {
    return contains(String(text + offset, length));
}

bool CharArraySet::add(const String& text) {
    return entries.add(ignoreCase ? StringUtils::toLower(text) : text);
}

bool CharArraySet::add(CharArray text) {
    return add(String(text.get(), text.size()));
}

int32_t CharArraySet::size() {
    return entries.size();
}

bool CharArraySet::isEmpty() {
    return entries.empty();
}

HashSet<String>::iterator CharArraySet::begin() {
    return entries.begin();
}

HashSet<String>::iterator CharArraySet::end() {
    return entries.end();
}

}
