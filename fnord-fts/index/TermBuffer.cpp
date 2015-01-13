/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "TermBuffer.h"
#include "IndexInput.h"
#include "FieldInfos.h"
#include "Term.h"
#include "MiscUtils.h"
#include "UnicodeUtils.h"
#include "StringUtils.h"

namespace Lucene {

TermBuffer::TermBuffer() {
    preUTF8Strings = false;
    text = newLucene<UnicodeResult>();
    bytes = newLucene<UTF8Result>();
}

TermBuffer::~TermBuffer() {
}

int32_t TermBuffer::compareTo(const LuceneObjectPtr& other) {
    TermBufferPtr otherTermBuffer(boost::static_pointer_cast<TermBuffer>(other));
    if (field == otherTermBuffer->field) {
        return compareChars(text->result.get(), text->length, otherTermBuffer->text->result.get(), otherTermBuffer->text->length);
    } else {
        return field.compare(otherTermBuffer->field);
    }
}

int32_t TermBuffer::compareChars(wchar_t* chars1, int32_t len1, wchar_t* chars2, int32_t len2) {
    int32_t end = len1 < len2 ? len1 : len2;
    for (int32_t k = 0; k < end; ++k) {
        wchar_t c1 = chars1[k];
        wchar_t c2 = chars2[k];
        if (c1 != c2) {
            return c1 - c2;
        }
    }
    return len1 - len2;
}

void TermBuffer::setPreUTF8Strings() {
    preUTF8Strings = true;
}

void TermBuffer::read(const IndexInputPtr& input, const FieldInfosPtr& fieldInfos) {
    this->term.reset(); // invalidate cache
    int32_t start = input->readVInt();
    int32_t length = input->readVInt();
    int32_t totalLength = start + length;
    if (preUTF8Strings) {
        text->setLength(totalLength);
        text->setLength(start + input->readChars(text->result.get(), start, length));
    } else {
        StringUtils::toUTF8(text->result.get(), text->length, bytes);
        bytes->setLength(totalLength);
        input->readBytes(bytes->result.get(), start, length);
        StringUtils::toUnicode(bytes->result.get(), totalLength, text);
    }
    this->field = fieldInfos->fieldName(input->readVInt());
}

void TermBuffer::set(const TermPtr& term) {
    if (!term) {
        reset();
        return;
    }
    String termText(term->text());
    int32_t termLen = termText.length();
    text->setLength(termLen);
    MiscUtils::arrayCopy(termText.begin(), 0, text->result.get(), 0, termLen);
    field = term->field();
    this->term = term;
}

void TermBuffer::set(const TermBufferPtr& other) {
    text->copyText(other->text);
    field = other->field;
    term = other->term;
}

void TermBuffer::reset() {
    field.clear();
    text->setLength(0);
    term.reset();
}

TermPtr TermBuffer::toTerm() {
    if (field.empty()) { // unset
        return TermPtr();
    }

    if (!term) {
        term = newLucene<Term>(field, String(text->result.get(), text->length));
    }

    return term;
}

LuceneObjectPtr TermBuffer::clone(const LuceneObjectPtr& other) {
    LuceneObjectPtr clone = other ? other : newLucene<TermBuffer>();
    TermBufferPtr cloneBuffer(boost::dynamic_pointer_cast<TermBuffer>(LuceneObject::clone(clone)));
    cloneBuffer->field = field;
    cloneBuffer->term = term;
    cloneBuffer->preUTF8Strings = preUTF8Strings;

    cloneBuffer->bytes = newLucene<UTF8Result>();
    cloneBuffer->text = newLucene<UnicodeResult>();
    cloneBuffer->text->copyText(text);
    return cloneBuffer;
}

}
