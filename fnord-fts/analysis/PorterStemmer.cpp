/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "PorterStemmer.h"

namespace Lucene {

PorterStemmer::PorterStemmer() {
    b = NULL;
    k = 0;
    j = 0;
    i = 0;
    dirty = false;
}

PorterStemmer::~PorterStemmer() {
}

bool PorterStemmer::stem(CharArray word) {
    return stem(word.get(), word.size() - 1);
}

bool PorterStemmer::stem(wchar_t* b, int32_t k) {
    this->b = b;
    this->k = k;
    this->j = 0;
    this->i = k;
    dirty = false;

    if (k <= 1) {
        return false;    // DEPARTURE
    }

    // With these lines, strings of length 1 or 2 don't go through the stemming process, although no mention
    // is made of this in the published algorithm. Remove the line to match the published algorithm.
    step1ab();
    step1c();
    step2();
    step3();
    step4();
    step5();

    if (i != this->k) {
        dirty = true;
    }
    return dirty;
}

wchar_t* PorterStemmer::getResultBuffer() {
    return b;
}

int32_t PorterStemmer::getResultLength() {
    return k + 1;
}

bool PorterStemmer::cons(int32_t i) {
    switch (b[i]) {
    case L'a':
    case L'e':
    case L'i':
    case L'o':
    case L'u':
        return false;
    case L'y':
        return (i == 0) ? true : !cons(i - 1);
    default:
        return true;
    }
}

int32_t PorterStemmer::m() {
    int32_t n = 0;
    int32_t i = 0;
    while (true) {
        if (i > j) {
            return n;
        }
        if (!cons(i)) {
            break;
        }
        ++i;
    }
    ++i;
    while (true) {
        while (true) {
            if (i > j) {
                return n;
            }
            if (cons(i)) {
                break;
            }
            ++i;
        }
        ++i;
        ++n;
        while (true) {
            if (i > j) {
                return n;
            }
            if (!cons(i)) {
                break;
            }
            ++i;
        }
        ++i;
    }
}

bool PorterStemmer::vowelinstem() {
    for (int32_t i = 0; i <= j; ++i) {
        if (!cons(i)) {
            return true;
        }
    }
    return false;
}

bool PorterStemmer::doublec(int32_t j) {
    if (j < 1) {
        return false;
    }
    if (b[j] != b[j - 1]) {
        return false;
    }
    return cons(j);
}

bool PorterStemmer::cvc(int32_t i) {
    if (i < 2 || !cons(i) || cons(i - 1) || !cons(i - 2)) {
        return false;
    }
    int32_t ch = b[i];
    if (ch == L'w' || ch == L'x' || ch == L'y') {
        return false;
    }
    return true;
}

bool PorterStemmer::ends(const wchar_t* s) {
    int32_t length = s[0];
    if (s[length] != b[k]) {
        return false;    // tiny speed-up
    }
    if (length > k + 1) {
        return false;
    }
    if (std::memcmp(b + k - length + 1, s + 1, length) != 0) {
        return false;
    }
    j = k - length;
    return true;
}

void PorterStemmer::setto(const wchar_t* s) {
    int32_t length = s[0];
    std::memmove(b + j + 1, s + 1, length);
    k = j + length;
    dirty = true;
}

void PorterStemmer::r(const wchar_t* s) {
    if (m() > 0) {
        setto(s);
    }
}

void PorterStemmer::step1ab() {
    if (b[k] == L's') {
        if (ends(L"\04" L"sses")) {
            k -= 2;
        } else if (ends(L"\03" L"ies")) {
            setto(L"\01" L"i");
        } else if (b[k - 1] != L's') {
            --k;
        }
    }
    if (ends(L"\03" L"eed")) {
        if (m() > 0) {
            --k;
        }
    } else if ((ends(L"\02" L"ed") || ends(L"\03" L"ing")) && vowelinstem()) {
        k = j;
        if (ends(L"\02" L"at")) {
            setto(L"\03" L"ate");
        } else if (ends(L"\02" L"bl")) {
            setto(L"\03" L"ble");
        } else if (ends(L"\02" L"iz")) {
            setto(L"\03" L"ize");
        } else if (doublec(k)) {
            --k;
            int32_t ch = b[k];
            if (ch == L'l' || ch == L's' || ch == L'z') {
                ++k;
            }
        } else if (m() == 1 && cvc(k)) {
            setto(L"\01" L"e");
        }
    }
}

void PorterStemmer::step1c() {
    if (ends(L"\01" L"y") && vowelinstem()) {
        b[k] = L'i';
        dirty = true;
    }
}

void PorterStemmer::step2() {
    if (k == 0) {
        return;
    }
    switch (b[k - 1]) {
    case L'a':
        if (ends(L"\07" L"ational")) {
            r(L"\03" L"ate");
            break;
        }
        if (ends(L"\06" L"tional")) {
            r(L"\04" L"tion");
            break;
        }
        break;
    case L'c':
        if (ends(L"\04" L"enci")) {
            r(L"\04" L"ence");
            break;
        }
        if (ends(L"\04" L"anci")) {
            r(L"\04" L"ance");
            break;
        }
        break;
    case L'e':
        if (ends(L"\04" L"izer")) {
            r(L"\03" L"ize");
            break;
        }
        break;
    case L'l':
        if (ends(L"\03" L"bli")) { // DEPARTURE
            r(L"\03" L"ble");
            break;
        }
        if (ends(L"\04" L"alli")) {
            r(L"\02" L"al");
            break;
        }
        if (ends(L"\05" L"entli")) {
            r(L"\03" L"ent");
            break;
        }
        if (ends(L"\03" L"eli")) {
            r(L"\01" L"e");
            break;
        }
        if (ends(L"\05" L"ousli")) {
            r(L"\03" L"ous");
            break;
        }
        break;
    case L'o':
        if (ends(L"\07" L"ization")) {
            r(L"\03" L"ize");
            break;
        }
        if (ends(L"\05" L"ation")) {
            r(L"\03" L"ate");
            break;
        }
        if (ends(L"\04" L"ator")) {
            r(L"\03" L"ate");
            break;
        }
        break;
    case L's':
        if (ends(L"\05" L"alism")) {
            r(L"\02" L"al");
            break;
        }
        if (ends(L"\07" L"iveness")) {
            r(L"\03" L"ive");
            break;
        }
        if (ends(L"\07" L"fulness")) {
            r(L"\03" L"ful");
            break;
        }
        if (ends(L"\07" L"ousness")) {
            r(L"\03" L"ous");
            break;
        }
        break;
    case L't':
        if (ends(L"\05" L"aliti")) {
            r(L"\02" L"al");
            break;
        }
        if (ends(L"\05" L"iviti")) {
            r(L"\03" L"ive");
            break;
        }
        if (ends(L"\06" L"biliti")) {
            r(L"\03" L"ble");
            break;
        }
        break;
    case L'g':
        if (ends(L"\04" L"logi")) { // DEPARTURE
            r(L"\03" L"log");
            break;
        }
    }
}

void PorterStemmer::step3() {
    switch (b[k]) {
    case L'e':
        if (ends(L"\05" L"icate")) {
            r(L"\02" L"ic");
            break;
        }
        if (ends(L"\05" L"ative")) {
            r(L"\00" L"");
            break;
        }
        if (ends(L"\05" L"alize")) {
            r(L"\02" L"al");
            break;
        }
        break;
    case L'i':
        if (ends(L"\05" L"iciti")) {
            r(L"\02" L"ic");
            break;
        }
        break;
    case L'l':
        if (ends(L"\04" L"ical")) {
            r(L"\02" L"ic");
            break;
        }
        if (ends(L"\03" L"ful")) {
            r(L"\00" L"");
            break;
        }
        break;
    case L's':
        if (ends(L"\04" L"ness")) {
            r(L"\00" L"");
            break;
        }
        break;
    }
}

void PorterStemmer::step4() {
    if (k == 0) {
        return;
    }
    switch (b[k - 1]) {
    case L'a':
        if (ends(L"\02" L"al")) {
            break;
        }
        return;
    case L'c':
        if (ends(L"\04" L"ance")) {
            break;
        }
        if (ends(L"\04" L"ence")) {
            break;
        }
        return;
    case L'e':
        if (ends(L"\02" L"er")) {
            break;
        }
        return;
    case L'i':
        if (ends(L"\02" L"ic")) {
            break;
        }
        return;
    case L'l':
        if (ends(L"\04" L"able")) {
            break;
        }
        if (ends(L"\04" L"ible")) {
            break;
        }
        return;
    case L'n':
        if (ends(L"\03" L"ant")) {
            break;
        }
        if (ends(L"\05" L"ement")) {
            break;
        }
        if (ends(L"\04" L"ment")) {
            break;
        }
        if (ends(L"\03" L"ent")) {
            break;
        }
        return;
    case L'o':
        if (ends(L"\03" L"ion") && (b[j] == L's' || b[j] == L't')) {
            break;
        }
        if (ends(L"\02" L"ou")) {
            break;
        }
        return;
    // takes care of -ous
    case L's':
        if (ends(L"\03" L"ism")) {
            break;
        }
        return;
    case L't':
        if (ends(L"\03" L"ate")) {
            break;
        }
        if (ends(L"\03" L"iti")) {
            break;
        }
        return;
    case L'u':
        if (ends(L"\03" L"ous")) {
            break;
        }
        return;
    case L'v':
        if (ends(L"\03" L"ive")) {
            break;
        }
        return;
    case L'z':
        if (ends(L"\03" L"ize")) {
            break;
        }
        return;
    default:
        return;
    }
    if (m() > 1) {
        k = j;
    }
}

void PorterStemmer::step5() {
    j = k;
    if (b[k] == L'e') {
        int32_t a = m();
        if (a > 1 || (a == 1 && !cvc(k - 1))) {
            --k;
        }
    }
    if (b[k] == L'l' && doublec(k) && m() > 1) {
        --k;
    }
}

}
