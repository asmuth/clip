/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "ISOLatin1AccentFilter.h"
#include "TermAttribute.h"

namespace Lucene {

ISOLatin1AccentFilter::ISOLatin1AccentFilter(const TokenStreamPtr& input) : TokenFilter(input) {
    output = CharArray::newInstance(256);
    outputPos = 0;
    termAtt = addAttribute<TermAttribute>();
}

ISOLatin1AccentFilter::~ISOLatin1AccentFilter() {
}

bool ISOLatin1AccentFilter::incrementToken() {
    if (input->incrementToken()) {
        wchar_t* buffer = termAtt->termBufferArray();
        int32_t length = termAtt->termLength();

        // If no characters actually require rewriting then we just return token as-is
        for (int32_t i = 0; i < length; ++i) {
            wchar_t c = buffer[i];
            if (c >= 0x00c0 && c <= 0xfb06) {
                removeAccents(buffer, length);
                termAtt->setTermBuffer(output.get(), 0, outputPos);
                break;
            }
        }
        return true;
    } else {
        return false;
    }
}

void ISOLatin1AccentFilter::removeAccents(const wchar_t* input, int32_t length) {
    // Worst-case length required
    int32_t maxSizeNeeded = 2 * length;

    int32_t size = output.size();
    while (size < maxSizeNeeded) {
        size *= 2;
    }

    if (size != output.size()) {
        output.resize(size);
    }

    outputPos = 0;
    int32_t pos = 0;

    wchar_t* output = this->output.get();

    for (int32_t i = 0; i < length; ++i, ++pos) {
        wchar_t c = input[pos];

        // Quick test: if it's not in range then just keep current character
        if (c < 0x00C0 || c > 0xFB06) {
            output[outputPos++] = c;
        } else {
            switch (c) {
            case 0x00C0:
            case 0x00C1:
            case 0x00C2:
            case 0x00C3:
            case 0x00C4:
            case 0x00C5:
                output[outputPos++] = L'A';
                break;
            case 0x00C6:
                output[outputPos++] = L'A';
                output[outputPos++] = L'E';
                break;
            case 0x00C7:
                output[outputPos++] = L'C';
                break;
            case 0x00C8:
            case 0x00C9:
            case 0x00CA:
            case 0x00CB:
                output[outputPos++] = L'E';
                break;
            case 0x00CC:
            case 0x00CD:
            case 0x00CE:
            case 0x00CF:
                output[outputPos++] = L'I';
                break;
            case 0x0132:
                output[outputPos++] = L'I';
                output[outputPos++] = L'J';
                break;
            case 0x00D0:
                output[outputPos++] = L'D';
                break;
            case 0x00D1:
                output[outputPos++] = L'N';
                break;
            case 0x00D2:
            case 0x00D3:
            case 0x00D4:
            case 0x00D5:
            case 0x00D6:
            case 0x00D8:
                output[outputPos++] = L'O';
                break;
            case 0x0152:
                output[outputPos++] = L'O';
                output[outputPos++] = L'E';
                break;
            case 0x00DE:
                output[outputPos++] = L'T';
                output[outputPos++] = L'H';
                break;
            case 0x00D9:
            case 0x00DA:
            case 0x00DB:
            case 0x00DC:
                output[outputPos++] = L'U';
                break;
            case 0x00DD:
            case 0x0178:
                output[outputPos++] = L'Y';
                break;
            case 0x00E0:
            case 0x00E1:
            case 0x00E2:
            case 0x00E3:
            case 0x00E4:
            case 0x00E5:
                output[outputPos++] = L'a';
                break;
            case 0x00E6:
                output[outputPos++] = L'a';
                output[outputPos++] = L'e';
                break;
            case 0x00E7:
                output[outputPos++] = L'c';
                break;
            case 0x00E8:
            case 0x00E9:
            case 0x00EA:
            case 0x00EB:
                output[outputPos++] = L'e';
                break;
            case 0x00EC:
            case 0x00ED:
            case 0x00EE:
            case 0x00EF:
                output[outputPos++] = L'i';
                break;
            case 0x0133:
                output[outputPos++] = L'i';
                output[outputPos++] = L'j';
                break;
            case 0x00F0:
                output[outputPos++] = L'd';
                break;
            case 0x00F1:
                output[outputPos++] = L'n';
                break;
            case 0x00F2:
            case 0x00F3:
            case 0x00F4:
            case 0x00F5:
            case 0x00F6:
            case 0x00F8:
                output[outputPos++] = L'o';
                break;
            case 0x0153:
                output[outputPos++] = L'o';
                output[outputPos++] = L'e';
                break;
            case 0x00DF:
                output[outputPos++] = L's';
                output[outputPos++] = L's';
                break;
            case 0x00FE:
                output[outputPos++] = L't';
                output[outputPos++] = L'h';
                break;
            case 0x00F9:
            case 0x00FA:
            case 0x00FB:
            case 0x00FC:
                output[outputPos++] = L'u';
                break;
            case 0x00FD:
            case 0x00FF:
                output[outputPos++] = L'y';
                break;
            case 0xFB00:
                output[outputPos++] = L'f';
                output[outputPos++] = L'f';
                break;
            case 0xFB01:
                output[outputPos++] = L'f';
                output[outputPos++] = L'i';
                break;
            case 0xFB02:
                output[outputPos++] = L'f';
                output[outputPos++] = L'l';
                break;
            case 0xFB05:
                output[outputPos++] = L'f';
                output[outputPos++] = L't';
                break;
            case 0xFB06:
                output[outputPos++] = L's';
                output[outputPos++] = L't';
                break;
            default :
                output[outputPos++] = c;
                break;
            }
        }
    }
}

}
