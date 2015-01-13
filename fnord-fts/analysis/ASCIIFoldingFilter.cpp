/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "ASCIIFoldingFilter.h"
#include "TermAttribute.h"
#include "MiscUtils.h"

namespace Lucene {

ASCIIFoldingFilter::ASCIIFoldingFilter(const TokenStreamPtr& input) : TokenFilter(input) {
    output = CharArray::newInstance(512);
    outputPos = 0;
    termAtt = addAttribute<TermAttribute>();
}

ASCIIFoldingFilter::~ASCIIFoldingFilter() {
}

bool ASCIIFoldingFilter::incrementToken() {
    if (input->incrementToken()) {
        wchar_t* buffer = termAtt->termBufferArray();
        int32_t length = termAtt->termLength();

        // If no characters actually require rewriting then we just return token as-is
        for (int32_t i = 0; i < length; ++i) {
            wchar_t c = buffer[i];
            if (c >= 0x0080) {
                foldToASCII(buffer, length);
                termAtt->setTermBuffer(output.get(), 0, outputPos);
                break;
            }
        }
        return true;
    } else {
        return false;
    }
}

void ASCIIFoldingFilter::foldToASCII(const wchar_t* input, int32_t length) {
    // Worst-case length required
    int32_t maxSizeNeeded = 4 * length;
    if (output.size() < maxSizeNeeded) {
        output.resize(MiscUtils::getNextSize(maxSizeNeeded));
    }

    outputPos = 0;
    wchar_t* output = this->output.get();

    for (int32_t pos = 0; pos < length; ++pos) {
        wchar_t c = input[pos];

        // Quick test: if it's not in range then just keep current character
        if (c < 0x0080) {
            output[outputPos++] = c;
        } else {
            switch (c) {
            case 0x00C0: // [LATIN CAPITAL LETTER A WITH GRAVE]
            case 0x00C1: // [LATIN CAPITAL LETTER A WITH ACUTE]
            case 0x00C2: // [LATIN CAPITAL LETTER A WITH CIRCUMFLEX]
            case 0x00C3: // [LATIN CAPITAL LETTER A WITH TILDE]
            case 0x00C4: // [LATIN CAPITAL LETTER A WITH DIAERESIS]
            case 0x00C5: // [LATIN CAPITAL LETTER A WITH RING ABOVE]
            case 0x0100: // [LATIN CAPITAL LETTER A WITH MACRON]
            case 0x0102: // [LATIN CAPITAL LETTER A WITH BREVE]
            case 0x0104: // [LATIN CAPITAL LETTER A WITH OGONEK]
            case 0x018F: // [LATIN CAPITAL LETTER SCHWA]
            case 0x01CD: // [LATIN CAPITAL LETTER A WITH CARON]
            case 0x01DE: // [LATIN CAPITAL LETTER A WITH DIAERESIS AND MACRON]
            case 0x01E0: // [LATIN CAPITAL LETTER A WITH DOT ABOVE AND MACRON]
            case 0x01FA: // [LATIN CAPITAL LETTER A WITH RING ABOVE AND ACUTE]
            case 0x0200: // [LATIN CAPITAL LETTER A WITH DOUBLE GRAVE]
            case 0x0202: // [LATIN CAPITAL LETTER A WITH INVERTED BREVE]
            case 0x0226: // [LATIN CAPITAL LETTER A WITH DOT ABOVE]
            case 0x023A: // [LATIN CAPITAL LETTER A WITH STROKE]
            case 0x1D00: // [LATIN LETTER SMALL CAPITAL A]
            case 0x1E00: // [LATIN CAPITAL LETTER A WITH RING BELOW]
            case 0x1EA0: // [LATIN CAPITAL LETTER A WITH DOT BELOW]
            case 0x1EA2: // [LATIN CAPITAL LETTER A WITH HOOK ABOVE]
            case 0x1EA4: // [LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND ACUTE]
            case 0x1EA6: // [LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND GRAVE]
            case 0x1EA8: // [LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND HOOK ABOVE]
            case 0x1EAA: // [LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND TILDE]
            case 0x1EAC: // [LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND DOT BELOW]
            case 0x1EAE: // [LATIN CAPITAL LETTER A WITH BREVE AND ACUTE]
            case 0x1EB0: // [LATIN CAPITAL LETTER A WITH BREVE AND GRAVE]
            case 0x1EB2: // [LATIN CAPITAL LETTER A WITH BREVE AND HOOK ABOVE]
            case 0x1EB4: // [LATIN CAPITAL LETTER A WITH BREVE AND TILDE]
            case 0x1EB6: // [LATIN CAPITAL LETTER A WITH BREVE AND DOT BELOW]
            case 0x24B6: // [CIRCLED LATIN CAPITAL LETTER A]
            case 0xFF21: // [FULLWIDTH LATIN CAPITAL LETTER A]
                output[outputPos++] = L'A';
                break;
            case 0x00E0: // [LATIN SMALL LETTER A WITH GRAVE]
            case 0x00E1: // [LATIN SMALL LETTER A WITH ACUTE]
            case 0x00E2: // [LATIN SMALL LETTER A WITH CIRCUMFLEX]
            case 0x00E3: // [LATIN SMALL LETTER A WITH TILDE]
            case 0x00E4: // [LATIN SMALL LETTER A WITH DIAERESIS]
            case 0x00E5: // [LATIN SMALL LETTER A WITH RING ABOVE]
            case 0x0101: // [LATIN SMALL LETTER A WITH MACRON]
            case 0x0103: // [LATIN SMALL LETTER A WITH BREVE]
            case 0x0105: // [LATIN SMALL LETTER A WITH OGONEK]
            case 0x01CE: // [LATIN SMALL LETTER A WITH CARON]
            case 0x01DF: // [LATIN SMALL LETTER A WITH DIAERESIS AND MACRON]
            case 0x01E1: // [LATIN SMALL LETTER A WITH DOT ABOVE AND MACRON]
            case 0x01FB: // [LATIN SMALL LETTER A WITH RING ABOVE AND ACUTE]
            case 0x0201: // [LATIN SMALL LETTER A WITH DOUBLE GRAVE]
            case 0x0203: // [LATIN SMALL LETTER A WITH INVERTED BREVE]
            case 0x0227: // [LATIN SMALL LETTER A WITH DOT ABOVE]
            case 0x0250: // [LATIN SMALL LETTER TURNED A]
            case 0x0259: // [LATIN SMALL LETTER SCHWA]
            case 0x025A: // [LATIN SMALL LETTER SCHWA WITH HOOK]
            case 0x1D8F: // [LATIN SMALL LETTER A WITH RETROFLEX HOOK]
            case 0x1D95: // [LATIN SMALL LETTER SCHWA WITH RETROFLEX HOOK]
            case 0x1E01: // [LATIN SMALL LETTER A WITH RING BELOW]
            case 0x1E9A: // [LATIN SMALL LETTER A WITH RIGHT HALF RING]
            case 0x1EA1: // [LATIN SMALL LETTER A WITH DOT BELOW]
            case 0x1EA3: // [LATIN SMALL LETTER A WITH HOOK ABOVE]
            case 0x1EA5: // [LATIN SMALL LETTER A WITH CIRCUMFLEX AND ACUTE]
            case 0x1EA7: // [LATIN SMALL LETTER A WITH CIRCUMFLEX AND GRAVE]
            case 0x1EA9: // [LATIN SMALL LETTER A WITH CIRCUMFLEX AND HOOK ABOVE]
            case 0x1EAB: // [LATIN SMALL LETTER A WITH CIRCUMFLEX AND TILDE]
            case 0x1EAD: // [LATIN SMALL LETTER A WITH CIRCUMFLEX AND DOT BELOW]
            case 0x1EAF: // [LATIN SMALL LETTER A WITH BREVE AND ACUTE]
            case 0x1EB1: // [LATIN SMALL LETTER A WITH BREVE AND GRAVE]
            case 0x1EB3: // [LATIN SMALL LETTER A WITH BREVE AND HOOK ABOVE]
            case 0x1EB5: // [LATIN SMALL LETTER A WITH BREVE AND TILDE]
            case 0x1EB7: // [LATIN SMALL LETTER A WITH BREVE AND DOT BELOW]
            case 0x2090: // [LATIN SUBSCRIPT SMALL LETTER A]
            case 0x2094: // [LATIN SUBSCRIPT SMALL LETTER SCHWA]
            case 0x24D0: // [CIRCLED LATIN SMALL LETTER A]
            case 0x2C65: // [LATIN SMALL LETTER A WITH STROKE]
            case 0x2C6F: // [LATIN CAPITAL LETTER TURNED A]
            case 0xFF41: // [FULLWIDTH LATIN SMALL LETTER A]
                output[outputPos++] = L'a';
                break;
            case 0xA732: // [LATIN CAPITAL LETTER AA]
                output[outputPos++] = L'A';
                output[outputPos++] = L'A';
                break;
            case 0x00C6: // [LATIN CAPITAL LETTER AE]
            case 0x01E2: // [LATIN CAPITAL LETTER AE WITH MACRON]
            case 0x01FC: // [LATIN CAPITAL LETTER AE WITH ACUTE]
            case 0x1D01: // [LATIN LETTER SMALL CAPITAL AE]
                output[outputPos++] = L'A';
                output[outputPos++] = L'E';
                break;
            case 0xA734: // [LATIN CAPITAL LETTER AO]
                output[outputPos++] = L'A';
                output[outputPos++] = L'O';
                break;
            case 0xA736: // [LATIN CAPITAL LETTER AU]
                output[outputPos++] = L'A';
                output[outputPos++] = L'U';
                break;
            case 0xA738: // [LATIN CAPITAL LETTER AV]
            case 0xA73A: // [LATIN CAPITAL LETTER AV WITH HORIZONTAL BAR]
                output[outputPos++] = L'A';
                output[outputPos++] = L'V';
                break;
            case 0xA73C: // [LATIN CAPITAL LETTER AY]
                output[outputPos++] = L'A';
                output[outputPos++] = L'Y';
                break;
            case 0x249C: // [PARENTHESIZED LATIN SMALL LETTER A]
                output[outputPos++] = L'(';
                output[outputPos++] = L'a';
                output[outputPos++] = L')';
                break;
            case 0xA733: // [LATIN SMALL LETTER AA]
                output[outputPos++] = L'a';
                output[outputPos++] = L'a';
                break;
            case 0x00E6: // [LATIN SMALL LETTER AE]
            case 0x01E3: // [LATIN SMALL LETTER AE WITH MACRON]
            case 0x01FD: // [LATIN SMALL LETTER AE WITH ACUTE]
            case 0x1D02: // [LATIN SMALL LETTER TURNED AE]
                output[outputPos++] = L'a';
                output[outputPos++] = L'e';
                break;
            case 0xA735: // [LATIN SMALL LETTER AO]
                output[outputPos++] = L'a';
                output[outputPos++] = L'o';
                break;
            case 0xA737: // [LATIN SMALL LETTER AU]
                output[outputPos++] = L'a';
                output[outputPos++] = L'u';
                break;
            case 0xA739: // [LATIN SMALL LETTER AV]
            case 0xA73B: // [LATIN SMALL LETTER AV WITH HORIZONTAL BAR]
                output[outputPos++] = L'a';
                output[outputPos++] = L'v';
                break;
            case 0xA73D: // [LATIN SMALL LETTER AY]
                output[outputPos++] = L'a';
                output[outputPos++] = L'y';
                break;
            case 0x0181: // [LATIN CAPITAL LETTER B WITH HOOK]
            case 0x0182: // [LATIN CAPITAL LETTER B WITH TOPBAR]
            case 0x0243: // [LATIN CAPITAL LETTER B WITH STROKE]
            case 0x0299: // [LATIN LETTER SMALL CAPITAL B]
            case 0x1D03: // [LATIN LETTER SMALL CAPITAL BARRED B]
            case 0x1E02: // [LATIN CAPITAL LETTER B WITH DOT ABOVE]
            case 0x1E04: // [LATIN CAPITAL LETTER B WITH DOT BELOW]
            case 0x1E06: // [LATIN CAPITAL LETTER B WITH LINE BELOW]
            case 0x24B7: // [CIRCLED LATIN CAPITAL LETTER B]
            case 0xFF22: // [FULLWIDTH LATIN CAPITAL LETTER B]
                output[outputPos++] = L'B';
                break;
            case 0x0180: // [LATIN SMALL LETTER B WITH STROKE]
            case 0x0183: // [LATIN SMALL LETTER B WITH TOPBAR]
            case 0x0253: // [LATIN SMALL LETTER B WITH HOOK]
            case 0x1D6C: // [LATIN SMALL LETTER B WITH MIDDLE TILDE]
            case 0x1D80: // [LATIN SMALL LETTER B WITH PALATAL HOOK]
            case 0x1E03: // [LATIN SMALL LETTER B WITH DOT ABOVE]
            case 0x1E05: // [LATIN SMALL LETTER B WITH DOT BELOW]
            case 0x1E07: // [LATIN SMALL LETTER B WITH LINE BELOW]
            case 0x24D1: // [CIRCLED LATIN SMALL LETTER B]
            case 0xFF42: // [FULLWIDTH LATIN SMALL LETTER B]
                output[outputPos++] = L'b';
                break;
            case 0x249D: // [PARENTHESIZED LATIN SMALL LETTER B]
                output[outputPos++] = L'(';
                output[outputPos++] = L'b';
                output[outputPos++] = L')';
                break;
            case 0x00C7: // [LATIN CAPITAL LETTER C WITH CEDILLA]
            case 0x0106: // [LATIN CAPITAL LETTER C WITH ACUTE]
            case 0x0108: // [LATIN CAPITAL LETTER C WITH CIRCUMFLEX]
            case 0x010A: // [LATIN CAPITAL LETTER C WITH DOT ABOVE]
            case 0x010C: // [LATIN CAPITAL LETTER C WITH CARON]
            case 0x0187: // [LATIN CAPITAL LETTER C WITH HOOK]
            case 0x023B: // [LATIN CAPITAL LETTER C WITH STROKE]
            case 0x0297: // [LATIN LETTER STRETCHED C]
            case 0x1D04: // [LATIN LETTER SMALL CAPITAL C]
            case 0x1E08: // [LATIN CAPITAL LETTER C WITH CEDILLA AND ACUTE]
            case 0x24B8: // [CIRCLED LATIN CAPITAL LETTER C]
            case 0xFF23: // [FULLWIDTH LATIN CAPITAL LETTER C]
                output[outputPos++] = L'C';
                break;
            case 0x00E7: // [LATIN SMALL LETTER C WITH CEDILLA]
            case 0x0107: // [LATIN SMALL LETTER C WITH ACUTE]
            case 0x0109: // [LATIN SMALL LETTER C WITH CIRCUMFLEX]
            case 0x010B: // [LATIN SMALL LETTER C WITH DOT ABOVE]
            case 0x010D: // [LATIN SMALL LETTER C WITH CARON]
            case 0x0188: // [LATIN SMALL LETTER C WITH HOOK]
            case 0x023C: // [LATIN SMALL LETTER C WITH STROKE]
            case 0x0255: // [LATIN SMALL LETTER C WITH CURL]
            case 0x1E09: // [LATIN SMALL LETTER C WITH CEDILLA AND ACUTE]
            case 0x2184: // [LATIN SMALL LETTER REVERSED C]
            case 0x24D2: // [CIRCLED LATIN SMALL LETTER C]
            case 0xA73E: // [LATIN CAPITAL LETTER REVERSED C WITH DOT]
            case 0xA73F: // [LATIN SMALL LETTER REVERSED C WITH DOT]
            case 0xFF43: // [FULLWIDTH LATIN SMALL LETTER C]
                output[outputPos++] = L'c';
                break;
            case 0x249E: // [PARENTHESIZED LATIN SMALL LETTER C]
                output[outputPos++] = L'(';
                output[outputPos++] = L'c';
                output[outputPos++] = L')';
                break;
            case 0x00D0: // [LATIN CAPITAL LETTER ETH]
            case 0x010E: // [LATIN CAPITAL LETTER D WITH CARON]
            case 0x0110: // [LATIN CAPITAL LETTER D WITH STROKE]
            case 0x0189: // [LATIN CAPITAL LETTER AFRICAN D]
            case 0x018A: // [LATIN CAPITAL LETTER D WITH HOOK]
            case 0x018B: // [LATIN CAPITAL LETTER D WITH TOPBAR]
            case 0x1D05: // [LATIN LETTER SMALL CAPITAL D]
            case 0x1D06: // [LATIN LETTER SMALL CAPITAL ETH]
            case 0x1E0A: // [LATIN CAPITAL LETTER D WITH DOT ABOVE]
            case 0x1E0C: // [LATIN CAPITAL LETTER D WITH DOT BELOW]
            case 0x1E0E: // [LATIN CAPITAL LETTER D WITH LINE BELOW]
            case 0x1E10: // [LATIN CAPITAL LETTER D WITH CEDILLA]
            case 0x1E12: // [LATIN CAPITAL LETTER D WITH CIRCUMFLEX BELOW]
            case 0x24B9: // [CIRCLED LATIN CAPITAL LETTER D]
            case 0xA779: // [LATIN CAPITAL LETTER INSULAR D]
            case 0xFF24: // [FULLWIDTH LATIN CAPITAL LETTER D]
                output[outputPos++] = L'D';
                break;
            case 0x00F0: // [LATIN SMALL LETTER ETH]
            case 0x010F: // [LATIN SMALL LETTER D WITH CARON]
            case 0x0111: // [LATIN SMALL LETTER D WITH STROKE]
            case 0x018C: // [LATIN SMALL LETTER D WITH TOPBAR]
            case 0x0221: // [LATIN SMALL LETTER D WITH CURL]
            case 0x0256: // [LATIN SMALL LETTER D WITH TAIL]
            case 0x0257: // [LATIN SMALL LETTER D WITH HOOK]
            case 0x1D6D: // [LATIN SMALL LETTER D WITH MIDDLE TILDE]
            case 0x1D81: // [LATIN SMALL LETTER D WITH PALATAL HOOK]
            case 0x1D91: // [LATIN SMALL LETTER D WITH HOOK AND TAIL]
            case 0x1E0B: // [LATIN SMALL LETTER D WITH DOT ABOVE]
            case 0x1E0D: // [LATIN SMALL LETTER D WITH DOT BELOW]
            case 0x1E0F: // [LATIN SMALL LETTER D WITH LINE BELOW]
            case 0x1E11: // [LATIN SMALL LETTER D WITH CEDILLA]
            case 0x1E13: // [LATIN SMALL LETTER D WITH CIRCUMFLEX BELOW]
            case 0x24D3: // [CIRCLED LATIN SMALL LETTER D]
            case 0xA77A: // [LATIN SMALL LETTER INSULAR D]
            case 0xFF44: // [FULLWIDTH LATIN SMALL LETTER D]
                output[outputPos++] = L'd';
                break;
            case 0x01C4: // [LATIN CAPITAL LETTER DZ WITH CARON]
            case 0x01F1: // [LATIN CAPITAL LETTER DZ]
                output[outputPos++] = L'D';
                output[outputPos++] = L'Z';
                break;
            case 0x01C5: // [LATIN CAPITAL LETTER D WITH SMALL LETTER Z WITH CARON]
            case 0x01F2: // [LATIN CAPITAL LETTER D WITH SMALL LETTER Z]
                output[outputPos++] = L'D';
                output[outputPos++] = L'z';
                break;
            case 0x249F: // [PARENTHESIZED LATIN SMALL LETTER D]
                output[outputPos++] = L'(';
                output[outputPos++] = L'd';
                output[outputPos++] = L')';
                break;
            case 0x0238: // [LATIN SMALL LETTER DB DIGRAPH]
                output[outputPos++] = L'd';
                output[outputPos++] = L'b';
                break;
            case 0x01C6: // [LATIN SMALL LETTER DZ WITH CARON]
            case 0x01F3: // [LATIN SMALL LETTER DZ]
            case 0x02A3: // [LATIN SMALL LETTER DZ DIGRAPH]
            case 0x02A5: // [LATIN SMALL LETTER DZ DIGRAPH WITH CURL]
                output[outputPos++] = L'd';
                output[outputPos++] = L'z';
                break;
            case 0x00C8: // [LATIN CAPITAL LETTER E WITH GRAVE]
            case 0x00C9: // [LATIN CAPITAL LETTER E WITH ACUTE]
            case 0x00CA: // [LATIN CAPITAL LETTER E WITH CIRCUMFLEX]
            case 0x00CB: // [LATIN CAPITAL LETTER E WITH DIAERESIS]
            case 0x0112: // [LATIN CAPITAL LETTER E WITH MACRON]
            case 0x0114: // [LATIN CAPITAL LETTER E WITH BREVE]
            case 0x0116: // [LATIN CAPITAL LETTER E WITH DOT ABOVE]
            case 0x0118: // [LATIN CAPITAL LETTER E WITH OGONEK]
            case 0x011A: // [LATIN CAPITAL LETTER E WITH CARON]
            case 0x018E: // [LATIN CAPITAL LETTER REVERSED E]
            case 0x0190: // [LATIN CAPITAL LETTER OPEN E]
            case 0x0204: // [LATIN CAPITAL LETTER E WITH DOUBLE GRAVE]
            case 0x0206: // [LATIN CAPITAL LETTER E WITH INVERTED BREVE]
            case 0x0228: // [LATIN CAPITAL LETTER E WITH CEDILLA]
            case 0x0246: // [LATIN CAPITAL LETTER E WITH STROKE]
            case 0x1D07: // [LATIN LETTER SMALL CAPITAL E]
            case 0x1E14: // [LATIN CAPITAL LETTER E WITH MACRON AND GRAVE]
            case 0x1E16: // [LATIN CAPITAL LETTER E WITH MACRON AND ACUTE]
            case 0x1E18: // [LATIN CAPITAL LETTER E WITH CIRCUMFLEX BELOW]
            case 0x1E1A: // [LATIN CAPITAL LETTER E WITH TILDE BELOW]
            case 0x1E1C: // [LATIN CAPITAL LETTER E WITH CEDILLA AND BREVE]
            case 0x1EB8: // [LATIN CAPITAL LETTER E WITH DOT BELOW]
            case 0x1EBA: // [LATIN CAPITAL LETTER E WITH HOOK ABOVE]
            case 0x1EBC: // [LATIN CAPITAL LETTER E WITH TILDE]
            case 0x1EBE: // [LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND ACUTE]
            case 0x1EC0: // [LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND GRAVE]
            case 0x1EC2: // [LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND HOOK ABOVE]
            case 0x1EC4: // [LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND TILDE]
            case 0x1EC6: // [LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND DOT BELOW]
            case 0x24BA: // [CIRCLED LATIN CAPITAL LETTER E]
            case 0x2C7B: // [LATIN LETTER SMALL CAPITAL TURNED E]
            case 0xFF25: // [FULLWIDTH LATIN CAPITAL LETTER E]
                output[outputPos++] = L'E';
                break;
            case 0x00E8: // [LATIN SMALL LETTER E WITH GRAVE]
            case 0x00E9: // [LATIN SMALL LETTER E WITH ACUTE]
            case 0x00EA: // [LATIN SMALL LETTER E WITH CIRCUMFLEX]
            case 0x00EB: // [LATIN SMALL LETTER E WITH DIAERESIS]
            case 0x0113: // [LATIN SMALL LETTER E WITH MACRON]
            case 0x0115: // [LATIN SMALL LETTER E WITH BREVE]
            case 0x0117: // [LATIN SMALL LETTER E WITH DOT ABOVE]
            case 0x0119: // [LATIN SMALL LETTER E WITH OGONEK]
            case 0x011B: // [LATIN SMALL LETTER E WITH CARON]
            case 0x01DD: // [LATIN SMALL LETTER TURNED E]
            case 0x0205: // [LATIN SMALL LETTER E WITH DOUBLE GRAVE]
            case 0x0207: // [LATIN SMALL LETTER E WITH INVERTED BREVE]
            case 0x0229: // [LATIN SMALL LETTER E WITH CEDILLA]
            case 0x0247: // [LATIN SMALL LETTER E WITH STROKE]
            case 0x0258: // [LATIN SMALL LETTER REVERSED E]
            case 0x025B: // [LATIN SMALL LETTER OPEN E]
            case 0x025C: // [LATIN SMALL LETTER REVERSED OPEN E]
            case 0x025D: // [LATIN SMALL LETTER REVERSED OPEN E WITH HOOK]
            case 0x025E: // [LATIN SMALL LETTER CLOSED REVERSED OPEN E]
            case 0x029A: // [LATIN SMALL LETTER CLOSED OPEN E]
            case 0x1D08: // [LATIN SMALL LETTER TURNED OPEN E]
            case 0x1D92: // [LATIN SMALL LETTER E WITH RETROFLEX HOOK]
            case 0x1D93: // [LATIN SMALL LETTER OPEN E WITH RETROFLEX HOOK]
            case 0x1D94: // [LATIN SMALL LETTER REVERSED OPEN E WITH RETROFLEX HOOK]
            case 0x1E15: // [LATIN SMALL LETTER E WITH MACRON AND GRAVE]
            case 0x1E17: // [LATIN SMALL LETTER E WITH MACRON AND ACUTE]
            case 0x1E19: // [LATIN SMALL LETTER E WITH CIRCUMFLEX BELOW]
            case 0x1E1B: // [LATIN SMALL LETTER E WITH TILDE BELOW]
            case 0x1E1D: // [LATIN SMALL LETTER E WITH CEDILLA AND BREVE]
            case 0x1EB9: // [LATIN SMALL LETTER E WITH DOT BELOW]
            case 0x1EBB: // [LATIN SMALL LETTER E WITH HOOK ABOVE]
            case 0x1EBD: // [LATIN SMALL LETTER E WITH TILDE]
            case 0x1EBF: // [LATIN SMALL LETTER E WITH CIRCUMFLEX AND ACUTE]
            case 0x1EC1: // [LATIN SMALL LETTER E WITH CIRCUMFLEX AND GRAVE]
            case 0x1EC3: // [LATIN SMALL LETTER E WITH CIRCUMFLEX AND HOOK ABOVE]
            case 0x1EC5: // [LATIN SMALL LETTER E WITH CIRCUMFLEX AND TILDE]
            case 0x1EC7: // [LATIN SMALL LETTER E WITH CIRCUMFLEX AND DOT BELOW]
            case 0x2091: // [LATIN SUBSCRIPT SMALL LETTER E]
            case 0x24D4: // [CIRCLED LATIN SMALL LETTER E]
            case 0x2C78: // [LATIN SMALL LETTER E WITH NOTCH]
            case 0xFF45: // [FULLWIDTH LATIN SMALL LETTER E]
                output[outputPos++] = L'e';
                break;
            case 0x24A0: // [PARENTHESIZED LATIN SMALL LETTER E]
                output[outputPos++] = L'(';
                output[outputPos++] = L'e';
                output[outputPos++] = L')';
                break;
            case 0x0191: // [LATIN CAPITAL LETTER F WITH HOOK]
            case 0x1E1E: // [LATIN CAPITAL LETTER F WITH DOT ABOVE]
            case 0x24BB: // [CIRCLED LATIN CAPITAL LETTER F]
            case 0xA730: // [LATIN LETTER SMALL CAPITAL F]
            case 0xA77B: // [LATIN CAPITAL LETTER INSULAR F]
            case 0xA7FB: // [LATIN EPIGRAPHIC LETTER REVERSED F]
            case 0xFF26: // [FULLWIDTH LATIN CAPITAL LETTER F]
                output[outputPos++] = L'F';
                break;
            case 0x0192: // [LATIN SMALL LETTER F WITH HOOK]
            case 0x1D6E: // [LATIN SMALL LETTER F WITH MIDDLE TILDE]
            case 0x1D82: // [LATIN SMALL LETTER F WITH PALATAL HOOK]
            case 0x1E1F: // [LATIN SMALL LETTER F WITH DOT ABOVE]
            case 0x1E9B: // [LATIN SMALL LETTER LONG S WITH DOT ABOVE]
            case 0x24D5: // [CIRCLED LATIN SMALL LETTER F]
            case 0xA77C: // [LATIN SMALL LETTER INSULAR F]
            case 0xFF46: // [FULLWIDTH LATIN SMALL LETTER F]
                output[outputPos++] = L'f';
                break;
            case 0x24A1: // [PARENTHESIZED LATIN SMALL LETTER F]
                output[outputPos++] = L'(';
                output[outputPos++] = L'f';
                output[outputPos++] = L')';
                break;
            case 0xFB00: // [LATIN SMALL LIGATURE FF]
                output[outputPos++] = L'f';
                output[outputPos++] = L'f';
                break;
            case 0xFB03: // [LATIN SMALL LIGATURE FFI]
                output[outputPos++] = L'f';
                output[outputPos++] = L'f';
                output[outputPos++] = L'i';
                break;
            case 0xFB04: // [LATIN SMALL LIGATURE FFL]
                output[outputPos++] = L'f';
                output[outputPos++] = L'f';
                output[outputPos++] = L'l';
                break;
            case 0xFB01: // [LATIN SMALL LIGATURE FI]
                output[outputPos++] = L'f';
                output[outputPos++] = L'i';
                break;
            case 0xFB02: // [LATIN SMALL LIGATURE FL]
                output[outputPos++] = L'f';
                output[outputPos++] = L'l';
                break;
            case 0x011C: // [LATIN CAPITAL LETTER G WITH CIRCUMFLEX]
            case 0x011E: // [LATIN CAPITAL LETTER G WITH BREVE]
            case 0x0120: // [LATIN CAPITAL LETTER G WITH DOT ABOVE]
            case 0x0122: // [LATIN CAPITAL LETTER G WITH CEDILLA]
            case 0x0193: // [LATIN CAPITAL LETTER G WITH HOOK]
            case 0x01E4: // [LATIN CAPITAL LETTER G WITH STROKE]
            case 0x01E5: // [LATIN SMALL LETTER G WITH STROKE]
            case 0x01E6: // [LATIN CAPITAL LETTER G WITH CARON]
            case 0x01E7: // [LATIN SMALL LETTER G WITH CARON]
            case 0x01F4: // [LATIN CAPITAL LETTER G WITH ACUTE]
            case 0x0262: // [LATIN LETTER SMALL CAPITAL G]
            case 0x029B: // [LATIN LETTER SMALL CAPITAL G WITH HOOK]
            case 0x1E20: // [LATIN CAPITAL LETTER G WITH MACRON]
            case 0x24BC: // [CIRCLED LATIN CAPITAL LETTER G]
            case 0xA77D: // [LATIN CAPITAL LETTER INSULAR G]
            case 0xA77E: // [LATIN CAPITAL LETTER TURNED INSULAR G]
            case 0xFF27: // [FULLWIDTH LATIN CAPITAL LETTER G]
                output[outputPos++] = L'G';
                break;
            case 0x011D: // [LATIN SMALL LETTER G WITH CIRCUMFLEX]
            case 0x011F: // [LATIN SMALL LETTER G WITH BREVE]
            case 0x0121: // [LATIN SMALL LETTER G WITH DOT ABOVE]
            case 0x0123: // [LATIN SMALL LETTER G WITH CEDILLA]
            case 0x01F5: // [LATIN SMALL LETTER G WITH ACUTE]
            case 0x0260: // [LATIN SMALL LETTER G WITH HOOK]
            case 0x0261: // [LATIN SMALL LETTER SCRIPT G]
            case 0x1D77: // [LATIN SMALL LETTER TURNED G]
            case 0x1D79: // [LATIN SMALL LETTER INSULAR G]
            case 0x1D83: // [LATIN SMALL LETTER G WITH PALATAL HOOK]
            case 0x1E21: // [LATIN SMALL LETTER G WITH MACRON]
            case 0x24D6: // [CIRCLED LATIN SMALL LETTER G]
            case 0xA77F: // [LATIN SMALL LETTER TURNED INSULAR G]
            case 0xFF47: // [FULLWIDTH LATIN SMALL LETTER G]
                output[outputPos++] = L'g';
                break;
            case 0x24A2: // [PARENTHESIZED LATIN SMALL LETTER G]
                output[outputPos++] = L'(';
                output[outputPos++] = L'g';
                output[outputPos++] = L')';
                break;
            case 0x0124: // [LATIN CAPITAL LETTER H WITH CIRCUMFLEX]
            case 0x0126: // [LATIN CAPITAL LETTER H WITH STROKE]
            case 0x021E: // [LATIN CAPITAL LETTER H WITH CARON]
            case 0x029C: // [LATIN LETTER SMALL CAPITAL H]
            case 0x1E22: // [LATIN CAPITAL LETTER H WITH DOT ABOVE]
            case 0x1E24: // [LATIN CAPITAL LETTER H WITH DOT BELOW]
            case 0x1E26: // [LATIN CAPITAL LETTER H WITH DIAERESIS]
            case 0x1E28: // [LATIN CAPITAL LETTER H WITH CEDILLA]
            case 0x1E2A: // [LATIN CAPITAL LETTER H WITH BREVE BELOW]
            case 0x24BD: // [CIRCLED LATIN CAPITAL LETTER H]
            case 0x2C67: // [LATIN CAPITAL LETTER H WITH DESCENDER]
            case 0x2C75: // [LATIN CAPITAL LETTER HALF H]
            case 0xFF28: // [FULLWIDTH LATIN CAPITAL LETTER H]
                output[outputPos++] = L'H';
                break;
            case 0x0125: // [LATIN SMALL LETTER H WITH CIRCUMFLEX]
            case 0x0127: // [LATIN SMALL LETTER H WITH STROKE]
            case 0x021F: // [LATIN SMALL LETTER H WITH CARON]
            case 0x0265: // [LATIN SMALL LETTER TURNED H]
            case 0x0266: // [LATIN SMALL LETTER H WITH HOOK]
            case 0x02AE: // [LATIN SMALL LETTER TURNED H WITH FISHHOOK]
            case 0x02AF: // [LATIN SMALL LETTER TURNED H WITH FISHHOOK AND TAIL]
            case 0x1E23: // [LATIN SMALL LETTER H WITH DOT ABOVE]
            case 0x1E25: // [LATIN SMALL LETTER H WITH DOT BELOW]
            case 0x1E27: // [LATIN SMALL LETTER H WITH DIAERESIS]
            case 0x1E29: // [LATIN SMALL LETTER H WITH CEDILLA]
            case 0x1E2B: // [LATIN SMALL LETTER H WITH BREVE BELOW]
            case 0x1E96: // [LATIN SMALL LETTER H WITH LINE BELOW]
            case 0x24D7: // [CIRCLED LATIN SMALL LETTER H]
            case 0x2C68: // [LATIN SMALL LETTER H WITH DESCENDER]
            case 0x2C76: // [LATIN SMALL LETTER HALF H]
            case 0xFF48: // [FULLWIDTH LATIN SMALL LETTER H]
                output[outputPos++] = L'h';
                break;
            case 0x01F6: // [LATIN CAPITAL LETTER HWAIR]
                output[outputPos++] = L'H';
                output[outputPos++] = L'V';
                break;
            case 0x24A3: // [PARENTHESIZED LATIN SMALL LETTER H]
                output[outputPos++] = L'(';
                output[outputPos++] = L'h';
                output[outputPos++] = L')';
                break;
            case 0x0195: // [LATIN SMALL LETTER HV]
                output[outputPos++] = L'h';
                output[outputPos++] = L'v';
                break;
            case 0x00CC: // [LATIN CAPITAL LETTER I WITH GRAVE]
            case 0x00CD: // [LATIN CAPITAL LETTER I WITH ACUTE]
            case 0x00CE: // [LATIN CAPITAL LETTER I WITH CIRCUMFLEX]
            case 0x00CF: // [LATIN CAPITAL LETTER I WITH DIAERESIS]
            case 0x0128: // [LATIN CAPITAL LETTER I WITH TILDE]
            case 0x012A: // [LATIN CAPITAL LETTER I WITH MACRON]
            case 0x012C: // [LATIN CAPITAL LETTER I WITH BREVE]
            case 0x012E: // [LATIN CAPITAL LETTER I WITH OGONEK]
            case 0x0130: // [LATIN CAPITAL LETTER I WITH DOT ABOVE]
            case 0x0196: // [LATIN CAPITAL LETTER IOTA]
            case 0x0197: // [LATIN CAPITAL LETTER I WITH STROKE]
            case 0x01CF: // [LATIN CAPITAL LETTER I WITH CARON]
            case 0x0208: // [LATIN CAPITAL LETTER I WITH DOUBLE GRAVE]
            case 0x020A: // [LATIN CAPITAL LETTER I WITH INVERTED BREVE]
            case 0x026A: // [LATIN LETTER SMALL CAPITAL I]
            case 0x1D7B: // [LATIN SMALL CAPITAL LETTER I WITH STROKE]
            case 0x1E2C: // [LATIN CAPITAL LETTER I WITH TILDE BELOW]
            case 0x1E2E: // [LATIN CAPITAL LETTER I WITH DIAERESIS AND ACUTE]
            case 0x1EC8: // [LATIN CAPITAL LETTER I WITH HOOK ABOVE]
            case 0x1ECA: // [LATIN CAPITAL LETTER I WITH DOT BELOW]
            case 0x24BE: // [CIRCLED LATIN CAPITAL LETTER I]
            case 0xA7FE: // [LATIN EPIGRAPHIC LETTER I LONGA]
            case 0xFF29: // [FULLWIDTH LATIN CAPITAL LETTER I]
                output[outputPos++] = L'I';
                break;
            case 0x00EC: // [LATIN SMALL LETTER I WITH GRAVE]
            case 0x00ED: // [LATIN SMALL LETTER I WITH ACUTE]
            case 0x00EE: // [LATIN SMALL LETTER I WITH CIRCUMFLEX]
            case 0x00EF: // [LATIN SMALL LETTER I WITH DIAERESIS]
            case 0x0129: // [LATIN SMALL LETTER I WITH TILDE]
            case 0x012B: // [LATIN SMALL LETTER I WITH MACRON]
            case 0x012D: // [LATIN SMALL LETTER I WITH BREVE]
            case 0x012F: // [LATIN SMALL LETTER I WITH OGONEK]
            case 0x0131: // [LATIN SMALL LETTER DOTLESS I]
            case 0x01D0: // [LATIN SMALL LETTER I WITH CARON]
            case 0x0209: // [LATIN SMALL LETTER I WITH DOUBLE GRAVE]
            case 0x020B: // [LATIN SMALL LETTER I WITH INVERTED BREVE]
            case 0x0268: // [LATIN SMALL LETTER I WITH STROKE]
            case 0x1D09: // [LATIN SMALL LETTER TURNED I]
            case 0x1D62: // [LATIN SUBSCRIPT SMALL LETTER I]
            case 0x1D7C: // [LATIN SMALL LETTER IOTA WITH STROKE]
            case 0x1D96: // [LATIN SMALL LETTER I WITH RETROFLEX HOOK]
            case 0x1E2D: // [LATIN SMALL LETTER I WITH TILDE BELOW]
            case 0x1E2F: // [LATIN SMALL LETTER I WITH DIAERESIS AND ACUTE]
            case 0x1EC9: // [LATIN SMALL LETTER I WITH HOOK ABOVE]
            case 0x1ECB: // [LATIN SMALL LETTER I WITH DOT BELOW]
            case 0x2071: // [SUPERSCRIPT LATIN SMALL LETTER I]
            case 0x24D8: // [CIRCLED LATIN SMALL LETTER I]
            case 0xFF49: // [FULLWIDTH LATIN SMALL LETTER I]
                output[outputPos++] = L'i';
                break;
            case 0x0132: // [LATIN CAPITAL LIGATURE IJ]
                output[outputPos++] = L'I';
                output[outputPos++] = L'J';
                break;
            case 0x24A4: // [PARENTHESIZED LATIN SMALL LETTER I]
                output[outputPos++] = L'(';
                output[outputPos++] = L'i';
                output[outputPos++] = L')';
                break;
            case 0x0133: // [LATIN SMALL LIGATURE IJ]
                output[outputPos++] = L'i';
                output[outputPos++] = L'j';
                break;
            case 0x0134: // [LATIN CAPITAL LETTER J WITH CIRCUMFLEX]
            case 0x0248: // [LATIN CAPITAL LETTER J WITH STROKE]
            case 0x1D0A: // [LATIN LETTER SMALL CAPITAL J]
            case 0x24BF: // [CIRCLED LATIN CAPITAL LETTER J]
            case 0xFF2A: // [FULLWIDTH LATIN CAPITAL LETTER J]
                output[outputPos++] = L'J';
                break;
            case 0x0135: // [LATIN SMALL LETTER J WITH CIRCUMFLEX]
            case 0x01F0: // [LATIN SMALL LETTER J WITH CARON]
            case 0x0237: // [LATIN SMALL LETTER DOTLESS J]
            case 0x0249: // [LATIN SMALL LETTER J WITH STROKE]
            case 0x025F: // [LATIN SMALL LETTER DOTLESS J WITH STROKE]
            case 0x0284: // [LATIN SMALL LETTER DOTLESS J WITH STROKE AND HOOK]
            case 0x029D: // [LATIN SMALL LETTER J WITH CROSSED-TAIL]
            case 0x24D9: // [CIRCLED LATIN SMALL LETTER J]
            case 0x2C7C: // [LATIN SUBSCRIPT SMALL LETTER J]
            case 0xFF4A: // [FULLWIDTH LATIN SMALL LETTER J]
                output[outputPos++] = L'j';
                break;
            case 0x24A5: // [PARENTHESIZED LATIN SMALL LETTER J]
                output[outputPos++] = L'(';
                output[outputPos++] = L'j';
                output[outputPos++] = L')';
                break;
            case 0x0136: // [LATIN CAPITAL LETTER K WITH CEDILLA]
            case 0x0198: // [LATIN CAPITAL LETTER K WITH HOOK]
            case 0x01E8: // [LATIN CAPITAL LETTER K WITH CARON]
            case 0x1D0B: // [LATIN LETTER SMALL CAPITAL K]
            case 0x1E30: // [LATIN CAPITAL LETTER K WITH ACUTE]
            case 0x1E32: // [LATIN CAPITAL LETTER K WITH DOT BELOW]
            case 0x1E34: // [LATIN CAPITAL LETTER K WITH LINE BELOW]
            case 0x24C0: // [CIRCLED LATIN CAPITAL LETTER K]
            case 0x2C69: // [LATIN CAPITAL LETTER K WITH DESCENDER]
            case 0xA740: // [LATIN CAPITAL LETTER K WITH STROKE]
            case 0xA742: // [LATIN CAPITAL LETTER K WITH DIAGONAL STROKE]
            case 0xA744: // [LATIN CAPITAL LETTER K WITH STROKE AND DIAGONAL STROKE]
            case 0xFF2B: // [FULLWIDTH LATIN CAPITAL LETTER K]
                output[outputPos++] = L'K';
                break;
            case 0x0137: // [LATIN SMALL LETTER K WITH CEDILLA]
            case 0x0199: // [LATIN SMALL LETTER K WITH HOOK]
            case 0x01E9: // [LATIN SMALL LETTER K WITH CARON]
            case 0x029E: // [LATIN SMALL LETTER TURNED K]
            case 0x1D84: // [LATIN SMALL LETTER K WITH PALATAL HOOK]
            case 0x1E31: // [LATIN SMALL LETTER K WITH ACUTE]
            case 0x1E33: // [LATIN SMALL LETTER K WITH DOT BELOW]
            case 0x1E35: // [LATIN SMALL LETTER K WITH LINE BELOW]
            case 0x24DA: // [CIRCLED LATIN SMALL LETTER K]
            case 0x2C6A: // [LATIN SMALL LETTER K WITH DESCENDER]
            case 0xA741: // [LATIN SMALL LETTER K WITH STROKE]
            case 0xA743: // [LATIN SMALL LETTER K WITH DIAGONAL STROKE]
            case 0xA745: // [LATIN SMALL LETTER K WITH STROKE AND DIAGONAL STROKE]
            case 0xFF4B: // [FULLWIDTH LATIN SMALL LETTER K]
                output[outputPos++] = L'k';
                break;
            case 0x24A6: // [PARENTHESIZED LATIN SMALL LETTER K]
                output[outputPos++] = L'(';
                output[outputPos++] = L'k';
                output[outputPos++] = L')';
                break;
            case 0x0139: // [LATIN CAPITAL LETTER L WITH ACUTE]
            case 0x013B: // [LATIN CAPITAL LETTER L WITH CEDILLA]
            case 0x013D: // [LATIN CAPITAL LETTER L WITH CARON]
            case 0x013F: // [LATIN CAPITAL LETTER L WITH MIDDLE DOT]
            case 0x0141: // [LATIN CAPITAL LETTER L WITH STROKE]
            case 0x023D: // [LATIN CAPITAL LETTER L WITH BAR]
            case 0x029F: // [LATIN LETTER SMALL CAPITAL L]
            case 0x1D0C: // [LATIN LETTER SMALL CAPITAL L WITH STROKE]
            case 0x1E36: // [LATIN CAPITAL LETTER L WITH DOT BELOW]
            case 0x1E38: // [LATIN CAPITAL LETTER L WITH DOT BELOW AND MACRON]
            case 0x1E3A: // [LATIN CAPITAL LETTER L WITH LINE BELOW]
            case 0x1E3C: // [LATIN CAPITAL LETTER L WITH CIRCUMFLEX BELOW]
            case 0x24C1: // [CIRCLED LATIN CAPITAL LETTER L]
            case 0x2C60: // [LATIN CAPITAL LETTER L WITH DOUBLE BAR]
            case 0x2C62: // [LATIN CAPITAL LETTER L WITH MIDDLE TILDE]
            case 0xA746: // [LATIN CAPITAL LETTER BROKEN L]
            case 0xA748: // [LATIN CAPITAL LETTER L WITH HIGH STROKE]
            case 0xA780: // [LATIN CAPITAL LETTER TURNED L]
            case 0xFF2C: // [FULLWIDTH LATIN CAPITAL LETTER L]
                output[outputPos++] = L'L';
                break;
            case 0x013A: // [LATIN SMALL LETTER L WITH ACUTE]
            case 0x013C: // [LATIN SMALL LETTER L WITH CEDILLA]
            case 0x013E: // [LATIN SMALL LETTER L WITH CARON]
            case 0x0140: // [LATIN SMALL LETTER L WITH MIDDLE DOT]
            case 0x0142: // [LATIN SMALL LETTER L WITH STROKE]
            case 0x019A: // [LATIN SMALL LETTER L WITH BAR]
            case 0x0234: // [LATIN SMALL LETTER L WITH CURL]
            case 0x026B: // [LATIN SMALL LETTER L WITH MIDDLE TILDE]
            case 0x026C: // [LATIN SMALL LETTER L WITH BELT]
            case 0x026D: // [LATIN SMALL LETTER L WITH RETROFLEX HOOK]
            case 0x1D85: // [LATIN SMALL LETTER L WITH PALATAL HOOK]
            case 0x1E37: // [LATIN SMALL LETTER L WITH DOT BELOW]
            case 0x1E39: // [LATIN SMALL LETTER L WITH DOT BELOW AND MACRON]
            case 0x1E3B: // [LATIN SMALL LETTER L WITH LINE BELOW]
            case 0x1E3D: // [LATIN SMALL LETTER L WITH CIRCUMFLEX BELOW]
            case 0x24DB: // [CIRCLED LATIN SMALL LETTER L]
            case 0x2C61: // [LATIN SMALL LETTER L WITH DOUBLE BAR]
            case 0xA747: // [LATIN SMALL LETTER BROKEN L]
            case 0xA749: // [LATIN SMALL LETTER L WITH HIGH STROKE]
            case 0xA781: // [LATIN SMALL LETTER TURNED L]
            case 0xFF4C: // [FULLWIDTH LATIN SMALL LETTER L]
                output[outputPos++] = L'l';
                break;
            case 0x01C7: // [LATIN CAPITAL LETTER LJ]
                output[outputPos++] = L'L';
                output[outputPos++] = L'J';
                break;
            case 0x1EFA: // [LATIN CAPITAL LETTER MIDDLE-WELSH LL]
                output[outputPos++] = L'L';
                output[outputPos++] = L'L';
                break;
            case 0x01C8: // [LATIN CAPITAL LETTER L WITH SMALL LETTER J]
                output[outputPos++] = L'L';
                output[outputPos++] = L'j';
                break;
            case 0x24A7: // [PARENTHESIZED LATIN SMALL LETTER L]
                output[outputPos++] = L'(';
                output[outputPos++] = L'l';
                output[outputPos++] = L')';
                break;
            case 0x01C9: // [LATIN SMALL LETTER LJ]
                output[outputPos++] = L'l';
                output[outputPos++] = L'j';
                break;
            case 0x1EFB: // [LATIN SMALL LETTER MIDDLE-WELSH LL]
                output[outputPos++] = L'l';
                output[outputPos++] = L'l';
                break;
            case 0x02AA: // [LATIN SMALL LETTER LS DIGRAPH]
                output[outputPos++] = L'l';
                output[outputPos++] = L's';
                break;
            case 0x02AB: // [LATIN SMALL LETTER LZ DIGRAPH]
                output[outputPos++] = L'l';
                output[outputPos++] = L'z';
                break;
            case 0x019C: // [LATIN CAPITAL LETTER TURNED M]
            case 0x1D0D: // [LATIN LETTER SMALL CAPITAL M]
            case 0x1E3E: // [LATIN CAPITAL LETTER M WITH ACUTE]
            case 0x1E40: // [LATIN CAPITAL LETTER M WITH DOT ABOVE]
            case 0x1E42: // [LATIN CAPITAL LETTER M WITH DOT BELOW]
            case 0x24C2: // [CIRCLED LATIN CAPITAL LETTER M]
            case 0x2C6E: // [LATIN CAPITAL LETTER M WITH HOOK]
            case 0xA7FD: // [LATIN EPIGRAPHIC LETTER INVERTED M]
            case 0xA7FF: // [LATIN EPIGRAPHIC LETTER ARCHAIC M]
            case 0xFF2D: // [FULLWIDTH LATIN CAPITAL LETTER M]
                output[outputPos++] = L'M';
                break;
            case 0x026F: // [LATIN SMALL LETTER TURNED M]
            case 0x0270: // [LATIN SMALL LETTER TURNED M WITH LONG LEG]
            case 0x0271: // [LATIN SMALL LETTER M WITH HOOK]
            case 0x1D6F: // [LATIN SMALL LETTER M WITH MIDDLE TILDE]
            case 0x1D86: // [LATIN SMALL LETTER M WITH PALATAL HOOK]
            case 0x1E3F: // [LATIN SMALL LETTER M WITH ACUTE]
            case 0x1E41: // [LATIN SMALL LETTER M WITH DOT ABOVE]
            case 0x1E43: // [LATIN SMALL LETTER M WITH DOT BELOW]
            case 0x24DC: // [CIRCLED LATIN SMALL LETTER M]
            case 0xFF4D: // [FULLWIDTH LATIN SMALL LETTER M]
                output[outputPos++] = L'm';
                break;
            case 0x24A8: // [PARENTHESIZED LATIN SMALL LETTER M]
                output[outputPos++] = L'(';
                output[outputPos++] = L'm';
                output[outputPos++] = L')';
                break;
            case 0x00D1: // [LATIN CAPITAL LETTER N WITH TILDE]
            case 0x0143: // [LATIN CAPITAL LETTER N WITH ACUTE]
            case 0x0145: // [LATIN CAPITAL LETTER N WITH CEDILLA]
            case 0x0147: // [LATIN CAPITAL LETTER N WITH CARON]
            case 0x014A: // [LATIN CAPITAL LETTER ENG]
            case 0x019D: // [LATIN CAPITAL LETTER N WITH LEFT HOOK]
            case 0x01F8: // [LATIN CAPITAL LETTER N WITH GRAVE]
            case 0x0220: // [LATIN CAPITAL LETTER N WITH LONG RIGHT LEG]
            case 0x0274: // [LATIN LETTER SMALL CAPITAL N]
            case 0x1D0E: // [LATIN LETTER SMALL CAPITAL REVERSED N]
            case 0x1E44: // [LATIN CAPITAL LETTER N WITH DOT ABOVE]
            case 0x1E46: // [LATIN CAPITAL LETTER N WITH DOT BELOW]
            case 0x1E48: // [LATIN CAPITAL LETTER N WITH LINE BELOW]
            case 0x1E4A: // [LATIN CAPITAL LETTER N WITH CIRCUMFLEX BELOW]
            case 0x24C3: // [CIRCLED LATIN CAPITAL LETTER N]
            case 0xFF2E: // [FULLWIDTH LATIN CAPITAL LETTER N]
                output[outputPos++] = L'N';
                break;
            case 0x00F1: // [LATIN SMALL LETTER N WITH TILDE]
            case 0x0144: // [LATIN SMALL LETTER N WITH ACUTE]
            case 0x0146: // [LATIN SMALL LETTER N WITH CEDILLA]
            case 0x0148: // [LATIN SMALL LETTER N WITH CARON]
            case 0x0149: // [LATIN SMALL LETTER N PRECEDED BY APOSTROPHE]
            case 0x014B: // [LATIN SMALL LETTER ENG]
            case 0x019E: // [LATIN SMALL LETTER N WITH LONG RIGHT LEG]
            case 0x01F9: // [LATIN SMALL LETTER N WITH GRAVE]
            case 0x0235: // [LATIN SMALL LETTER N WITH CURL]
            case 0x0272: // [LATIN SMALL LETTER N WITH LEFT HOOK]
            case 0x0273: // [LATIN SMALL LETTER N WITH RETROFLEX HOOK]
            case 0x1D70: // [LATIN SMALL LETTER N WITH MIDDLE TILDE]
            case 0x1D87: // [LATIN SMALL LETTER N WITH PALATAL HOOK]
            case 0x1E45: // [LATIN SMALL LETTER N WITH DOT ABOVE]
            case 0x1E47: // [LATIN SMALL LETTER N WITH DOT BELOW]
            case 0x1E49: // [LATIN SMALL LETTER N WITH LINE BELOW]
            case 0x1E4B: // [LATIN SMALL LETTER N WITH CIRCUMFLEX BELOW]
            case 0x207F: // [SUPERSCRIPT LATIN SMALL LETTER N]
            case 0x24DD: // [CIRCLED LATIN SMALL LETTER N]
            case 0xFF4E: // [FULLWIDTH LATIN SMALL LETTER N]
                output[outputPos++] = L'n';
                break;
            case 0x01CA: // [LATIN CAPITAL LETTER NJ]
                output[outputPos++] = L'N';
                output[outputPos++] = L'J';
                break;
            case 0x01CB: // [LATIN CAPITAL LETTER N WITH SMALL LETTER J]
                output[outputPos++] = L'N';
                output[outputPos++] = L'j';
                break;
            case 0x24A9: // [PARENTHESIZED LATIN SMALL LETTER N]
                output[outputPos++] = L'(';
                output[outputPos++] = L'n';
                output[outputPos++] = L')';
                break;
            case 0x01CC: // [LATIN SMALL LETTER NJ]
                output[outputPos++] = L'n';
                output[outputPos++] = L'j';
                break;
            case 0x00D2: // [LATIN CAPITAL LETTER O WITH GRAVE]
            case 0x00D3: // [LATIN CAPITAL LETTER O WITH ACUTE]
            case 0x00D4: // [LATIN CAPITAL LETTER O WITH CIRCUMFLEX]
            case 0x00D5: // [LATIN CAPITAL LETTER O WITH TILDE]
            case 0x00D6: // [LATIN CAPITAL LETTER O WITH DIAERESIS]
            case 0x00D8: // [LATIN CAPITAL LETTER O WITH STROKE]
            case 0x014C: // [LATIN CAPITAL LETTER O WITH MACRON]
            case 0x014E: // [LATIN CAPITAL LETTER O WITH BREVE]
            case 0x0150: // [LATIN CAPITAL LETTER O WITH DOUBLE ACUTE]
            case 0x0186: // [LATIN CAPITAL LETTER OPEN O]
            case 0x019F: // [LATIN CAPITAL LETTER O WITH MIDDLE TILDE]
            case 0x01A0: // [LATIN CAPITAL LETTER O WITH HORN]
            case 0x01D1: // [LATIN CAPITAL LETTER O WITH CARON]
            case 0x01EA: // [LATIN CAPITAL LETTER O WITH OGONEK]
            case 0x01EC: // [LATIN CAPITAL LETTER O WITH OGONEK AND MACRON]
            case 0x01FE: // [LATIN CAPITAL LETTER O WITH STROKE AND ACUTE]
            case 0x020C: // [LATIN CAPITAL LETTER O WITH DOUBLE GRAVE]
            case 0x020E: // [LATIN CAPITAL LETTER O WITH INVERTED BREVE]
            case 0x022A: // [LATIN CAPITAL LETTER O WITH DIAERESIS AND MACRON]
            case 0x022C: // [LATIN CAPITAL LETTER O WITH TILDE AND MACRON]
            case 0x022E: // [LATIN CAPITAL LETTER O WITH DOT ABOVE]
            case 0x0230: // [LATIN CAPITAL LETTER O WITH DOT ABOVE AND MACRON]
            case 0x1D0F: // [LATIN LETTER SMALL CAPITAL O]
            case 0x1D10: // [LATIN LETTER SMALL CAPITAL OPEN O]
            case 0x1E4C: // [LATIN CAPITAL LETTER O WITH TILDE AND ACUTE]
            case 0x1E4E: // [LATIN CAPITAL LETTER O WITH TILDE AND DIAERESIS]
            case 0x1E50: // [LATIN CAPITAL LETTER O WITH MACRON AND GRAVE]
            case 0x1E52: // [LATIN CAPITAL LETTER O WITH MACRON AND ACUTE]
            case 0x1ECC: // [LATIN CAPITAL LETTER O WITH DOT BELOW]
            case 0x1ECE: // [LATIN CAPITAL LETTER O WITH HOOK ABOVE]
            case 0x1ED0: // [LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND ACUTE]
            case 0x1ED2: // [LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND GRAVE]
            case 0x1ED4: // [LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND HOOK ABOVE]
            case 0x1ED6: // [LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND TILDE]
            case 0x1ED8: // [LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND DOT BELOW]
            case 0x1EDA: // [LATIN CAPITAL LETTER O WITH HORN AND ACUTE]
            case 0x1EDC: // [LATIN CAPITAL LETTER O WITH HORN AND GRAVE]
            case 0x1EDE: // [LATIN CAPITAL LETTER O WITH HORN AND HOOK ABOVE]
            case 0x1EE0: // [LATIN CAPITAL LETTER O WITH HORN AND TILDE]
            case 0x1EE2: // [LATIN CAPITAL LETTER O WITH HORN AND DOT BELOW]
            case 0x24C4: // [CIRCLED LATIN CAPITAL LETTER O]
            case 0xA74A: // [LATIN CAPITAL LETTER O WITH LONG STROKE OVERLAY]
            case 0xA74C: // [LATIN CAPITAL LETTER O WITH LOOP]
            case 0xFF2F: // [FULLWIDTH LATIN CAPITAL LETTER O]
                output[outputPos++] = L'O';
                break;
            case 0x00F2: // [LATIN SMALL LETTER O WITH GRAVE]
            case 0x00F3: // [LATIN SMALL LETTER O WITH ACUTE]
            case 0x00F4: // [LATIN SMALL LETTER O WITH CIRCUMFLEX]
            case 0x00F5: // [LATIN SMALL LETTER O WITH TILDE]
            case 0x00F6: // [LATIN SMALL LETTER O WITH DIAERESIS]
            case 0x00F8: // [LATIN SMALL LETTER O WITH STROKE]
            case 0x014D: // [LATIN SMALL LETTER O WITH MACRON]
            case 0x014F: // [LATIN SMALL LETTER O WITH BREVE]
            case 0x0151: // [LATIN SMALL LETTER O WITH DOUBLE ACUTE]
            case 0x01A1: // [LATIN SMALL LETTER O WITH HORN]
            case 0x01D2: // [LATIN SMALL LETTER O WITH CARON]
            case 0x01EB: // [LATIN SMALL LETTER O WITH OGONEK]
            case 0x01ED: // [LATIN SMALL LETTER O WITH OGONEK AND MACRON]
            case 0x01FF: // [LATIN SMALL LETTER O WITH STROKE AND ACUTE]
            case 0x020D: // [LATIN SMALL LETTER O WITH DOUBLE GRAVE]
            case 0x020F: // [LATIN SMALL LETTER O WITH INVERTED BREVE]
            case 0x022B: // [LATIN SMALL LETTER O WITH DIAERESIS AND MACRON]
            case 0x022D: // [LATIN SMALL LETTER O WITH TILDE AND MACRON]
            case 0x022F: // [LATIN SMALL LETTER O WITH DOT ABOVE]
            case 0x0231: // [LATIN SMALL LETTER O WITH DOT ABOVE AND MACRON]
            case 0x0254: // [LATIN SMALL LETTER OPEN O]
            case 0x0275: // [LATIN SMALL LETTER BARRED O]
            case 0x1D16: // [LATIN SMALL LETTER TOP HALF O]
            case 0x1D17: // [LATIN SMALL LETTER BOTTOM HALF O]
            case 0x1D97: // [LATIN SMALL LETTER OPEN O WITH RETROFLEX HOOK]
            case 0x1E4D: // [LATIN SMALL LETTER O WITH TILDE AND ACUTE]
            case 0x1E4F: // [LATIN SMALL LETTER O WITH TILDE AND DIAERESIS]
            case 0x1E51: // [LATIN SMALL LETTER O WITH MACRON AND GRAVE]
            case 0x1E53: // [LATIN SMALL LETTER O WITH MACRON AND ACUTE]
            case 0x1ECD: // [LATIN SMALL LETTER O WITH DOT BELOW]
            case 0x1ECF: // [LATIN SMALL LETTER O WITH HOOK ABOVE]
            case 0x1ED1: // [LATIN SMALL LETTER O WITH CIRCUMFLEX AND ACUTE]
            case 0x1ED3: // [LATIN SMALL LETTER O WITH CIRCUMFLEX AND GRAVE]
            case 0x1ED5: // [LATIN SMALL LETTER O WITH CIRCUMFLEX AND HOOK ABOVE]
            case 0x1ED7: // [LATIN SMALL LETTER O WITH CIRCUMFLEX AND TILDE]
            case 0x1ED9: // [LATIN SMALL LETTER O WITH CIRCUMFLEX AND DOT BELOW]
            case 0x1EDB: // [LATIN SMALL LETTER O WITH HORN AND ACUTE]
            case 0x1EDD: // [LATIN SMALL LETTER O WITH HORN AND GRAVE]
            case 0x1EDF: // [LATIN SMALL LETTER O WITH HORN AND HOOK ABOVE]
            case 0x1EE1: // [LATIN SMALL LETTER O WITH HORN AND TILDE]
            case 0x1EE3: // [LATIN SMALL LETTER O WITH HORN AND DOT BELOW]
            case 0x2092: // [LATIN SUBSCRIPT SMALL LETTER O]
            case 0x24DE: // [CIRCLED LATIN SMALL LETTER O]
            case 0x2C7A: // [LATIN SMALL LETTER O WITH LOW RING INSIDE]
            case 0xA74B: // [LATIN SMALL LETTER O WITH LONG STROKE OVERLAY]
            case 0xA74D: // [LATIN SMALL LETTER O WITH LOOP]
            case 0xFF4F: // [FULLWIDTH LATIN SMALL LETTER O]
                output[outputPos++] = L'o';
                break;
            case 0x0152: // [LATIN CAPITAL LIGATURE OE]
            case 0x0276: // [LATIN LETTER SMALL CAPITAL OE]
                output[outputPos++] = L'O';
                output[outputPos++] = L'E';
                break;
            case 0xA74E: // [LATIN CAPITAL LETTER OO]
                output[outputPos++] = L'O';
                output[outputPos++] = L'O';
                break;
            case 0x0222: // [LATIN CAPITAL LETTER OU]
            case 0x1D15: // [LATIN LETTER SMALL CAPITAL OU]
                output[outputPos++] = L'O';
                output[outputPos++] = L'U';
                break;
            case 0x24AA: // [PARENTHESIZED LATIN SMALL LETTER O]
                output[outputPos++] = L'(';
                output[outputPos++] = L'o';
                output[outputPos++] = L')';
                break;
            case 0x0153: // [LATIN SMALL LIGATURE OE]
            case 0x1D14: // [LATIN SMALL LETTER TURNED OE]
                output[outputPos++] = L'o';
                output[outputPos++] = L'e';
                break;
            case 0xA74F: // [LATIN SMALL LETTER OO]
                output[outputPos++] = L'o';
                output[outputPos++] = L'o';
                break;
            case 0x0223: // [LATIN SMALL LETTER OU]
                output[outputPos++] = L'o';
                output[outputPos++] = L'u';
                break;
            case 0x01A4: // [LATIN CAPITAL LETTER P WITH HOOK]
            case 0x1D18: // [LATIN LETTER SMALL CAPITAL P]
            case 0x1E54: // [LATIN CAPITAL LETTER P WITH ACUTE]
            case 0x1E56: // [LATIN CAPITAL LETTER P WITH DOT ABOVE]
            case 0x24C5: // [CIRCLED LATIN CAPITAL LETTER P]
            case 0x2C63: // [LATIN CAPITAL LETTER P WITH STROKE]
            case 0xA750: // [LATIN CAPITAL LETTER P WITH STROKE THROUGH DESCENDER]
            case 0xA752: // [LATIN CAPITAL LETTER P WITH FLOURISH]
            case 0xA754: // [LATIN CAPITAL LETTER P WITH SQUIRREL TAIL]
            case 0xFF30: // [FULLWIDTH LATIN CAPITAL LETTER P]
                output[outputPos++] = L'P';
                break;
            case 0x01A5: // [LATIN SMALL LETTER P WITH HOOK]
            case 0x1D71: // [LATIN SMALL LETTER P WITH MIDDLE TILDE]
            case 0x1D7D: // [LATIN SMALL LETTER P WITH STROKE]
            case 0x1D88: // [LATIN SMALL LETTER P WITH PALATAL HOOK]
            case 0x1E55: // [LATIN SMALL LETTER P WITH ACUTE]
            case 0x1E57: // [LATIN SMALL LETTER P WITH DOT ABOVE]
            case 0x24DF: // [CIRCLED LATIN SMALL LETTER P]
            case 0xA751: // [LATIN SMALL LETTER P WITH STROKE THROUGH DESCENDER]
            case 0xA753: // [LATIN SMALL LETTER P WITH FLOURISH]
            case 0xA755: // [LATIN SMALL LETTER P WITH SQUIRREL TAIL]
            case 0xA7FC: // [LATIN EPIGRAPHIC LETTER REVERSED P]
            case 0xFF50: // [FULLWIDTH LATIN SMALL LETTER P]
                output[outputPos++] = L'p';
                break;
            case 0x24AB: // [PARENTHESIZED LATIN SMALL LETTER P]
                output[outputPos++] = L'(';
                output[outputPos++] = L'p';
                output[outputPos++] = L')';
                break;
            case 0x024A: // [LATIN CAPITAL LETTER SMALL Q WITH HOOK TAIL]
            case 0x24C6: // [CIRCLED LATIN CAPITAL LETTER Q]
            case 0xA756: // [LATIN CAPITAL LETTER Q WITH STROKE THROUGH DESCENDER]
            case 0xA758: // [LATIN CAPITAL LETTER Q WITH DIAGONAL STROKE]
            case 0xFF31: // [FULLWIDTH LATIN CAPITAL LETTER Q]
                output[outputPos++] = L'Q';
                break;
            case 0x0138: // [LATIN SMALL LETTER KRA]
            case 0x024B: // [LATIN SMALL LETTER Q WITH HOOK TAIL]
            case 0x02A0: // [LATIN SMALL LETTER Q WITH HOOK]
            case 0x24E0: // [CIRCLED LATIN SMALL LETTER Q]
            case 0xA757: // [LATIN SMALL LETTER Q WITH STROKE THROUGH DESCENDER]
            case 0xA759: // [LATIN SMALL LETTER Q WITH DIAGONAL STROKE]
            case 0xFF51: // [FULLWIDTH LATIN SMALL LETTER Q]
                output[outputPos++] = L'q';
                break;
            case 0x24AC: // [PARENTHESIZED LATIN SMALL LETTER Q]
                output[outputPos++] = L'(';
                output[outputPos++] = L'q';
                output[outputPos++] = L')';
                break;
            case 0x0239: // [LATIN SMALL LETTER QP DIGRAPH]
                output[outputPos++] = L'q';
                output[outputPos++] = L'p';
                break;
            case 0x0154: // [LATIN CAPITAL LETTER R WITH ACUTE]
            case 0x0156: // [LATIN CAPITAL LETTER R WITH CEDILLA]
            case 0x0158: // [LATIN CAPITAL LETTER R WITH CARON]
            case 0x0210: // [LATIN CAPITAL LETTER R WITH DOUBLE GRAVE]
            case 0x0212: // [LATIN CAPITAL LETTER R WITH INVERTED BREVE]
            case 0x024C: // [LATIN CAPITAL LETTER R WITH STROKE]
            case 0x0280: // [LATIN LETTER SMALL CAPITAL R]
            case 0x0281: // [LATIN LETTER SMALL CAPITAL INVERTED R]
            case 0x1D19: // [LATIN LETTER SMALL CAPITAL REVERSED R]
            case 0x1D1A: // [LATIN LETTER SMALL CAPITAL TURNED R]
            case 0x1E58: // [LATIN CAPITAL LETTER R WITH DOT ABOVE]
            case 0x1E5A: // [LATIN CAPITAL LETTER R WITH DOT BELOW]
            case 0x1E5C: // [LATIN CAPITAL LETTER R WITH DOT BELOW AND MACRON]
            case 0x1E5E: // [LATIN CAPITAL LETTER R WITH LINE BELOW]
            case 0x24C7: // [CIRCLED LATIN CAPITAL LETTER R]
            case 0x2C64: // [LATIN CAPITAL LETTER R WITH TAIL]
            case 0xA75A: // [LATIN CAPITAL LETTER R ROTUNDA]
            case 0xA782: // [LATIN CAPITAL LETTER INSULAR R]
            case 0xFF32: // [FULLWIDTH LATIN CAPITAL LETTER R]
                output[outputPos++] = L'R';
                break;
            case 0x0155: // [LATIN SMALL LETTER R WITH ACUTE]
            case 0x0157: // [LATIN SMALL LETTER R WITH CEDILLA]
            case 0x0159: // [LATIN SMALL LETTER R WITH CARON]
            case 0x0211: // [LATIN SMALL LETTER R WITH DOUBLE GRAVE]
            case 0x0213: // [LATIN SMALL LETTER R WITH INVERTED BREVE]
            case 0x024D: // [LATIN SMALL LETTER R WITH STROKE]
            case 0x027C: // [LATIN SMALL LETTER R WITH LONG LEG]
            case 0x027D: // [LATIN SMALL LETTER R WITH TAIL]
            case 0x027E: // [LATIN SMALL LETTER R WITH FISHHOOK]
            case 0x027F: // [LATIN SMALL LETTER REVERSED R WITH FISHHOOK]
            case 0x1D63: // [LATIN SUBSCRIPT SMALL LETTER R]
            case 0x1D72: // [LATIN SMALL LETTER R WITH MIDDLE TILDE]
            case 0x1D73: // [LATIN SMALL LETTER R WITH FISHHOOK AND MIDDLE TILDE]
            case 0x1D89: // [LATIN SMALL LETTER R WITH PALATAL HOOK]
            case 0x1E59: // [LATIN SMALL LETTER R WITH DOT ABOVE]
            case 0x1E5B: // [LATIN SMALL LETTER R WITH DOT BELOW]
            case 0x1E5D: // [LATIN SMALL LETTER R WITH DOT BELOW AND MACRON]
            case 0x1E5F: // [LATIN SMALL LETTER R WITH LINE BELOW]
            case 0x24E1: // [CIRCLED LATIN SMALL LETTER R]
            case 0xA75B: // [LATIN SMALL LETTER R ROTUNDA]
            case 0xA783: // [LATIN SMALL LETTER INSULAR R]
            case 0xFF52: // [FULLWIDTH LATIN SMALL LETTER R]
                output[outputPos++] = L'r';
                break;
            case 0x24AD: // [PARENTHESIZED LATIN SMALL LETTER R]
                output[outputPos++] = L'(';
                output[outputPos++] = L'r';
                output[outputPos++] = L')';
                break;
            case 0x015A: // [LATIN CAPITAL LETTER S WITH ACUTE]
            case 0x015C: // [LATIN CAPITAL LETTER S WITH CIRCUMFLEX]
            case 0x015E: // [LATIN CAPITAL LETTER S WITH CEDILLA]
            case 0x0160: // [LATIN CAPITAL LETTER S WITH CARON]
            case 0x0218: // [LATIN CAPITAL LETTER S WITH COMMA BELOW]
            case 0x1E60: // [LATIN CAPITAL LETTER S WITH DOT ABOVE]
            case 0x1E62: // [LATIN CAPITAL LETTER S WITH DOT BELOW]
            case 0x1E64: // [LATIN CAPITAL LETTER S WITH ACUTE AND DOT ABOVE]
            case 0x1E66: // [LATIN CAPITAL LETTER S WITH CARON AND DOT ABOVE]
            case 0x1E68: // [LATIN CAPITAL LETTER S WITH DOT BELOW AND DOT ABOVE]
            case 0x24C8: // [CIRCLED LATIN CAPITAL LETTER S]
            case 0xA731: // [LATIN LETTER SMALL CAPITAL S]
            case 0xA785: // [LATIN SMALL LETTER INSULAR S]
            case 0xFF33: // [FULLWIDTH LATIN CAPITAL LETTER S]
                output[outputPos++] = L'S';
                break;
            case 0x015B: // [LATIN SMALL LETTER S WITH ACUTE]
            case 0x015D: // [LATIN SMALL LETTER S WITH CIRCUMFLEX]
            case 0x015F: // [LATIN SMALL LETTER S WITH CEDILLA]
            case 0x0161: // [LATIN SMALL LETTER S WITH CARON]
            case 0x017F: // [LATIN SMALL LETTER LONG S]
            case 0x0219: // [LATIN SMALL LETTER S WITH COMMA BELOW]
            case 0x023F: // [LATIN SMALL LETTER S WITH SWASH TAIL]
            case 0x0282: // [LATIN SMALL LETTER S WITH HOOK]
            case 0x1D74: // [LATIN SMALL LETTER S WITH MIDDLE TILDE]
            case 0x1D8A: // [LATIN SMALL LETTER S WITH PALATAL HOOK]
            case 0x1E61: // [LATIN SMALL LETTER S WITH DOT ABOVE]
            case 0x1E63: // [LATIN SMALL LETTER S WITH DOT BELOW]
            case 0x1E65: // [LATIN SMALL LETTER S WITH ACUTE AND DOT ABOVE]
            case 0x1E67: // [LATIN SMALL LETTER S WITH CARON AND DOT ABOVE]
            case 0x1E69: // [LATIN SMALL LETTER S WITH DOT BELOW AND DOT ABOVE]
            case 0x1E9C: // [LATIN SMALL LETTER LONG S WITH DIAGONAL STROKE]
            case 0x1E9D: // [LATIN SMALL LETTER LONG S WITH HIGH STROKE]
            case 0x24E2: // [CIRCLED LATIN SMALL LETTER S]
            case 0xA784: // [LATIN CAPITAL LETTER INSULAR S]
            case 0xFF53: // [FULLWIDTH LATIN SMALL LETTER S]
                output[outputPos++] = L's';
                break;
            case 0x1E9E: // [LATIN CAPITAL LETTER SHARP S]
                output[outputPos++] = L'S';
                output[outputPos++] = L'S';
                break;
            case 0x24AE: // [PARENTHESIZED LATIN SMALL LETTER S]
                output[outputPos++] = L'(';
                output[outputPos++] = L's';
                output[outputPos++] = L')';
                break;
            case 0x00DF: // [LATIN SMALL LETTER SHARP S]
                output[outputPos++] = L's';
                output[outputPos++] = L's';
                break;
            case 0xFB06: // [LATIN SMALL LIGATURE ST]
                output[outputPos++] = L's';
                output[outputPos++] = L't';
                break;
            case 0x0162: // [LATIN CAPITAL LETTER T WITH CEDILLA]
            case 0x0164: // [LATIN CAPITAL LETTER T WITH CARON]
            case 0x0166: // [LATIN CAPITAL LETTER T WITH STROKE]
            case 0x01AC: // [LATIN CAPITAL LETTER T WITH HOOK]
            case 0x01AE: // [LATIN CAPITAL LETTER T WITH RETROFLEX HOOK]
            case 0x021A: // [LATIN CAPITAL LETTER T WITH COMMA BELOW]
            case 0x023E: // [LATIN CAPITAL LETTER T WITH DIAGONAL STROKE]
            case 0x1D1B: // [LATIN LETTER SMALL CAPITAL T]
            case 0x1E6A: // [LATIN CAPITAL LETTER T WITH DOT ABOVE]
            case 0x1E6C: // [LATIN CAPITAL LETTER T WITH DOT BELOW]
            case 0x1E6E: // [LATIN CAPITAL LETTER T WITH LINE BELOW]
            case 0x1E70: // [LATIN CAPITAL LETTER T WITH CIRCUMFLEX BELOW]
            case 0x24C9: // [CIRCLED LATIN CAPITAL LETTER T]
            case 0xA786: // [LATIN CAPITAL LETTER INSULAR T]
            case 0xFF34: // [FULLWIDTH LATIN CAPITAL LETTER T]
                output[outputPos++] = L'T';
                break;
            case 0x0163: // [LATIN SMALL LETTER T WITH CEDILLA]
            case 0x0165: // [LATIN SMALL LETTER T WITH CARON]
            case 0x0167: // [LATIN SMALL LETTER T WITH STROKE]
            case 0x01AB: // [LATIN SMALL LETTER T WITH PALATAL HOOK]
            case 0x01AD: // [LATIN SMALL LETTER T WITH HOOK]
            case 0x021B: // [LATIN SMALL LETTER T WITH COMMA BELOW]
            case 0x0236: // [LATIN SMALL LETTER T WITH CURL]
            case 0x0287: // [LATIN SMALL LETTER TURNED T]
            case 0x0288: // [LATIN SMALL LETTER T WITH RETROFLEX HOOK]
            case 0x1D75: // [LATIN SMALL LETTER T WITH MIDDLE TILDE]
            case 0x1E6B: // [LATIN SMALL LETTER T WITH DOT ABOVE]
            case 0x1E6D: // [LATIN SMALL LETTER T WITH DOT BELOW]
            case 0x1E6F: // [LATIN SMALL LETTER T WITH LINE BELOW]
            case 0x1E71: // [LATIN SMALL LETTER T WITH CIRCUMFLEX BELOW]
            case 0x1E97: // [LATIN SMALL LETTER T WITH DIAERESIS]
            case 0x24E3: // [CIRCLED LATIN SMALL LETTER T]
            case 0x2C66: // [LATIN SMALL LETTER T WITH DIAGONAL STROKE]
            case 0xFF54: // [FULLWIDTH LATIN SMALL LETTER T]
                output[outputPos++] = L't';
                break;
            case 0x00DE: // [LATIN CAPITAL LETTER THORN]
            case 0xA766: // [LATIN CAPITAL LETTER THORN WITH STROKE THROUGH DESCENDER]
                output[outputPos++] = L'T';
                output[outputPos++] = L'H';
                break;
            case 0xA728: // [LATIN CAPITAL LETTER TZ]
                output[outputPos++] = L'T';
                output[outputPos++] = L'Z';
                break;
            case 0x24AF: // [PARENTHESIZED LATIN SMALL LETTER T]
                output[outputPos++] = L'(';
                output[outputPos++] = L't';
                output[outputPos++] = L')';
                break;
            case 0x02A8: // [LATIN SMALL LETTER TC DIGRAPH WITH CURL]
                output[outputPos++] = L't';
                output[outputPos++] = L'c';
                break;
            case 0x00FE: // [LATIN SMALL LETTER THORN]
            case 0x1D7A: // [LATIN SMALL LETTER TH WITH STRIKETHROUGH]
            case 0xA767: // [LATIN SMALL LETTER THORN WITH STROKE THROUGH DESCENDER]
                output[outputPos++] = L't';
                output[outputPos++] = L'h';
                break;
            case 0x02A6: // [LATIN SMALL LETTER TS DIGRAPH]
                output[outputPos++] = L't';
                output[outputPos++] = L's';
                break;
            case 0xA729: // [LATIN SMALL LETTER TZ]
                output[outputPos++] = L't';
                output[outputPos++] = L'z';
                break;
            case 0x00D9: // [LATIN CAPITAL LETTER U WITH GRAVE]
            case 0x00DA: // [LATIN CAPITAL LETTER U WITH ACUTE]
            case 0x00DB: // [LATIN CAPITAL LETTER U WITH CIRCUMFLEX]
            case 0x00DC: // [LATIN CAPITAL LETTER U WITH DIAERESIS]
            case 0x0168: // [LATIN CAPITAL LETTER U WITH TILDE]
            case 0x016A: // [LATIN CAPITAL LETTER U WITH MACRON]
            case 0x016C: // [LATIN CAPITAL LETTER U WITH BREVE]
            case 0x016E: // [LATIN CAPITAL LETTER U WITH RING ABOVE]
            case 0x0170: // [LATIN CAPITAL LETTER U WITH DOUBLE ACUTE]
            case 0x0172: // [LATIN CAPITAL LETTER U WITH OGONEK]
            case 0x01AF: // [LATIN CAPITAL LETTER U WITH HORN]
            case 0x01D3: // [LATIN CAPITAL LETTER U WITH CARON]
            case 0x01D5: // [LATIN CAPITAL LETTER U WITH DIAERESIS AND MACRON]
            case 0x01D7: // [LATIN CAPITAL LETTER U WITH DIAERESIS AND ACUTE]
            case 0x01D9: // [LATIN CAPITAL LETTER U WITH DIAERESIS AND CARON]
            case 0x01DB: // [LATIN CAPITAL LETTER U WITH DIAERESIS AND GRAVE]
            case 0x0214: // [LATIN CAPITAL LETTER U WITH DOUBLE GRAVE]
            case 0x0216: // [LATIN CAPITAL LETTER U WITH INVERTED BREVE]
            case 0x0244: // [LATIN CAPITAL LETTER U BAR]
            case 0x1D1C: // [LATIN LETTER SMALL CAPITAL U]
            case 0x1D7E: // [LATIN SMALL CAPITAL LETTER U WITH STROKE]
            case 0x1E72: // [LATIN CAPITAL LETTER U WITH DIAERESIS BELOW]
            case 0x1E74: // [LATIN CAPITAL LETTER U WITH TILDE BELOW]
            case 0x1E76: // [LATIN CAPITAL LETTER U WITH CIRCUMFLEX BELOW]
            case 0x1E78: // [LATIN CAPITAL LETTER U WITH TILDE AND ACUTE]
            case 0x1E7A: // [LATIN CAPITAL LETTER U WITH MACRON AND DIAERESIS]
            case 0x1EE4: // [LATIN CAPITAL LETTER U WITH DOT BELOW]
            case 0x1EE6: // [LATIN CAPITAL LETTER U WITH HOOK ABOVE]
            case 0x1EE8: // [LATIN CAPITAL LETTER U WITH HORN AND ACUTE]
            case 0x1EEA: // [LATIN CAPITAL LETTER U WITH HORN AND GRAVE]
            case 0x1EEC: // [LATIN CAPITAL LETTER U WITH HORN AND HOOK ABOVE]
            case 0x1EEE: // [LATIN CAPITAL LETTER U WITH HORN AND TILDE]
            case 0x1EF0: // [LATIN CAPITAL LETTER U WITH HORN AND DOT BELOW]
            case 0x24CA: // [CIRCLED LATIN CAPITAL LETTER U]
            case 0xFF35: // [FULLWIDTH LATIN CAPITAL LETTER U]
                output[outputPos++] = L'U';
                break;
            case 0x00F9: // [LATIN SMALL LETTER U WITH GRAVE]
            case 0x00FA: // [LATIN SMALL LETTER U WITH ACUTE]
            case 0x00FB: // [LATIN SMALL LETTER U WITH CIRCUMFLEX]
            case 0x00FC: // [LATIN SMALL LETTER U WITH DIAERESIS]
            case 0x0169: // [LATIN SMALL LETTER U WITH TILDE]
            case 0x016B: // [LATIN SMALL LETTER U WITH MACRON]
            case 0x016D: // [LATIN SMALL LETTER U WITH BREVE]
            case 0x016F: // [LATIN SMALL LETTER U WITH RING ABOVE]
            case 0x0171: // [LATIN SMALL LETTER U WITH DOUBLE ACUTE]
            case 0x0173: // [LATIN SMALL LETTER U WITH OGONEK]
            case 0x01B0: // [LATIN SMALL LETTER U WITH HORN]
            case 0x01D4: // [LATIN SMALL LETTER U WITH CARON]
            case 0x01D6: // [LATIN SMALL LETTER U WITH DIAERESIS AND MACRON]
            case 0x01D8: // [LATIN SMALL LETTER U WITH DIAERESIS AND ACUTE]
            case 0x01DA: // [LATIN SMALL LETTER U WITH DIAERESIS AND CARON]
            case 0x01DC: // [LATIN SMALL LETTER U WITH DIAERESIS AND GRAVE]
            case 0x0215: // [LATIN SMALL LETTER U WITH DOUBLE GRAVE]
            case 0x0217: // [LATIN SMALL LETTER U WITH INVERTED BREVE]
            case 0x0289: // [LATIN SMALL LETTER U BAR]
            case 0x1D64: // [LATIN SUBSCRIPT SMALL LETTER U]
            case 0x1D99: // [LATIN SMALL LETTER U WITH RETROFLEX HOOK]
            case 0x1E73: // [LATIN SMALL LETTER U WITH DIAERESIS BELOW]
            case 0x1E75: // [LATIN SMALL LETTER U WITH TILDE BELOW]
            case 0x1E77: // [LATIN SMALL LETTER U WITH CIRCUMFLEX BELOW]
            case 0x1E79: // [LATIN SMALL LETTER U WITH TILDE AND ACUTE]
            case 0x1E7B: // [LATIN SMALL LETTER U WITH MACRON AND DIAERESIS]
            case 0x1EE5: // [LATIN SMALL LETTER U WITH DOT BELOW]
            case 0x1EE7: // [LATIN SMALL LETTER U WITH HOOK ABOVE]
            case 0x1EE9: // [LATIN SMALL LETTER U WITH HORN AND ACUTE]
            case 0x1EEB: // [LATIN SMALL LETTER U WITH HORN AND GRAVE]
            case 0x1EED: // [LATIN SMALL LETTER U WITH HORN AND HOOK ABOVE]
            case 0x1EEF: // [LATIN SMALL LETTER U WITH HORN AND TILDE]
            case 0x1EF1: // [LATIN SMALL LETTER U WITH HORN AND DOT BELOW]
            case 0x24E4: // [CIRCLED LATIN SMALL LETTER U]
            case 0xFF55: // [FULLWIDTH LATIN SMALL LETTER U]
                output[outputPos++] = L'u';
                break;
            case 0x24B0: // [PARENTHESIZED LATIN SMALL LETTER U]
                output[outputPos++] = L'(';
                output[outputPos++] = L'u';
                output[outputPos++] = L')';
                break;
            case 0x1D6B: // [LATIN SMALL LETTER UE]
                output[outputPos++] = L'u';
                output[outputPos++] = L'e';
                break;
            case 0x01B2: // [LATIN CAPITAL LETTER V WITH HOOK]
            case 0x0245: // [LATIN CAPITAL LETTER TURNED V]
            case 0x1D20: // [LATIN LETTER SMALL CAPITAL V]
            case 0x1E7C: // [LATIN CAPITAL LETTER V WITH TILDE]
            case 0x1E7E: // [LATIN CAPITAL LETTER V WITH DOT BELOW]
            case 0x1EFC: // [LATIN CAPITAL LETTER MIDDLE-WELSH V]
            case 0x24CB: // [CIRCLED LATIN CAPITAL LETTER V]
            case 0xA75E: // [LATIN CAPITAL LETTER V WITH DIAGONAL STROKE]
            case 0xA768: // [LATIN CAPITAL LETTER VEND]
            case 0xFF36: // [FULLWIDTH LATIN CAPITAL LETTER V]
                output[outputPos++] = L'V';
                break;
            case 0x028B: // [LATIN SMALL LETTER V WITH HOOK]
            case 0x028C: // [LATIN SMALL LETTER TURNED V]
            case 0x1D65: // [LATIN SUBSCRIPT SMALL LETTER V]
            case 0x1D8C: // [LATIN SMALL LETTER V WITH PALATAL HOOK]
            case 0x1E7D: // [LATIN SMALL LETTER V WITH TILDE]
            case 0x1E7F: // [LATIN SMALL LETTER V WITH DOT BELOW]
            case 0x24E5: // [CIRCLED LATIN SMALL LETTER V]
            case 0x2C71: // [LATIN SMALL LETTER V WITH RIGHT HOOK]
            case 0x2C74: // [LATIN SMALL LETTER V WITH CURL]
            case 0xA75F: // [LATIN SMALL LETTER V WITH DIAGONAL STROKE]
            case 0xFF56: // [FULLWIDTH LATIN SMALL LETTER V]
                output[outputPos++] = L'v';
                break;
            case 0xA760: // [LATIN CAPITAL LETTER VY]
                output[outputPos++] = L'V';
                output[outputPos++] = L'Y';
                break;
            case 0x24B1: // [PARENTHESIZED LATIN SMALL LETTER V]
                output[outputPos++] = L'(';
                output[outputPos++] = L'v';
                output[outputPos++] = L')';
                break;
            case 0xA761: // [LATIN SMALL LETTER VY]
                output[outputPos++] = L'v';
                output[outputPos++] = L'y';
                break;
            case 0x0174: // [LATIN CAPITAL LETTER W WITH CIRCUMFLEX]
            case 0x01F7: // [LATIN CAPITAL LETTER WYNN]
            case 0x1D21: // [LATIN LETTER SMALL CAPITAL W]
            case 0x1E80: // [LATIN CAPITAL LETTER W WITH GRAVE]
            case 0x1E82: // [LATIN CAPITAL LETTER W WITH ACUTE]
            case 0x1E84: // [LATIN CAPITAL LETTER W WITH DIAERESIS]
            case 0x1E86: // [LATIN CAPITAL LETTER W WITH DOT ABOVE]
            case 0x1E88: // [LATIN CAPITAL LETTER W WITH DOT BELOW]
            case 0x24CC: // [CIRCLED LATIN CAPITAL LETTER W]
            case 0x2C72: // [LATIN CAPITAL LETTER W WITH HOOK]
            case 0xFF37: // [FULLWIDTH LATIN CAPITAL LETTER W]
                output[outputPos++] = L'W';
                break;
            case 0x0175: // [LATIN SMALL LETTER W WITH CIRCUMFLEX]
            case 0x01BF: // [LATIN LETTER WYNN]
            case 0x028D: // [LATIN SMALL LETTER TURNED W]
            case 0x1E81: // [LATIN SMALL LETTER W WITH GRAVE]
            case 0x1E83: // [LATIN SMALL LETTER W WITH ACUTE]
            case 0x1E85: // [LATIN SMALL LETTER W WITH DIAERESIS]
            case 0x1E87: // [LATIN SMALL LETTER W WITH DOT ABOVE]
            case 0x1E89: // [LATIN SMALL LETTER W WITH DOT BELOW]
            case 0x1E98: // [LATIN SMALL LETTER W WITH RING ABOVE]
            case 0x24E6: // [CIRCLED LATIN SMALL LETTER W]
            case 0x2C73: // [LATIN SMALL LETTER W WITH HOOK]
            case 0xFF57: // [FULLWIDTH LATIN SMALL LETTER W]
                output[outputPos++] = L'w';
                break;
            case 0x24B2: // [PARENTHESIZED LATIN SMALL LETTER W]
                output[outputPos++] = L'(';
                output[outputPos++] = L'w';
                output[outputPos++] = L')';
                break;
            case 0x1E8A: // [LATIN CAPITAL LETTER X WITH DOT ABOVE]
            case 0x1E8C: // [LATIN CAPITAL LETTER X WITH DIAERESIS]
            case 0x24CD: // [CIRCLED LATIN CAPITAL LETTER X]
            case 0xFF38: // [FULLWIDTH LATIN CAPITAL LETTER X]
                output[outputPos++] = L'X';
                break;
            case 0x1D8D: // [LATIN SMALL LETTER X WITH PALATAL HOOK]
            case 0x1E8B: // [LATIN SMALL LETTER X WITH DOT ABOVE]
            case 0x1E8D: // [LATIN SMALL LETTER X WITH DIAERESIS]
            case 0x2093: // [LATIN SUBSCRIPT SMALL LETTER X]
            case 0x24E7: // [CIRCLED LATIN SMALL LETTER X]
            case 0xFF58: // [FULLWIDTH LATIN SMALL LETTER X]
                output[outputPos++] = L'x';
                break;
            case 0x24B3: // [PARENTHESIZED LATIN SMALL LETTER X]
                output[outputPos++] = L'(';
                output[outputPos++] = L'x';
                output[outputPos++] = L')';
                break;
            case 0x00DD: // [LATIN CAPITAL LETTER Y WITH ACUTE]
            case 0x0176: // [LATIN CAPITAL LETTER Y WITH CIRCUMFLEX]
            case 0x0178: // [LATIN CAPITAL LETTER Y WITH DIAERESIS]
            case 0x01B3: // [LATIN CAPITAL LETTER Y WITH HOOK]
            case 0x0232: // [LATIN CAPITAL LETTER Y WITH MACRON]
            case 0x024E: // [LATIN CAPITAL LETTER Y WITH STROKE]
            case 0x028F: // [LATIN LETTER SMALL CAPITAL Y]
            case 0x1E8E: // [LATIN CAPITAL LETTER Y WITH DOT ABOVE]
            case 0x1EF2: // [LATIN CAPITAL LETTER Y WITH GRAVE]
            case 0x1EF4: // [LATIN CAPITAL LETTER Y WITH DOT BELOW]
            case 0x1EF6: // [LATIN CAPITAL LETTER Y WITH HOOK ABOVE]
            case 0x1EF8: // [LATIN CAPITAL LETTER Y WITH TILDE]
            case 0x1EFE: // [LATIN CAPITAL LETTER Y WITH LOOP]
            case 0x24CE: // [CIRCLED LATIN CAPITAL LETTER Y]
            case 0xFF39: // [FULLWIDTH LATIN CAPITAL LETTER Y]
                output[outputPos++] = L'Y';
                break;
            case 0x00FD: // [LATIN SMALL LETTER Y WITH ACUTE]
            case 0x00FF: // [LATIN SMALL LETTER Y WITH DIAERESIS]
            case 0x0177: // [LATIN SMALL LETTER Y WITH CIRCUMFLEX]
            case 0x01B4: // [LATIN SMALL LETTER Y WITH HOOK]
            case 0x0233: // [LATIN SMALL LETTER Y WITH MACRON]
            case 0x024F: // [LATIN SMALL LETTER Y WITH STROKE]
            case 0x028E: // [LATIN SMALL LETTER TURNED Y]
            case 0x1E8F: // [LATIN SMALL LETTER Y WITH DOT ABOVE]
            case 0x1E99: // [LATIN SMALL LETTER Y WITH RING ABOVE]
            case 0x1EF3: // [LATIN SMALL LETTER Y WITH GRAVE]
            case 0x1EF5: // [LATIN SMALL LETTER Y WITH DOT BELOW]
            case 0x1EF7: // [LATIN SMALL LETTER Y WITH HOOK ABOVE]
            case 0x1EF9: // [LATIN SMALL LETTER Y WITH TILDE]
            case 0x1EFF: // [LATIN SMALL LETTER Y WITH LOOP]
            case 0x24E8: // [CIRCLED LATIN SMALL LETTER Y]
            case 0xFF59: // [FULLWIDTH LATIN SMALL LETTER Y]
                output[outputPos++] = L'y';
                break;
            case 0x24B4: // [PARENTHESIZED LATIN SMALL LETTER Y]
                output[outputPos++] = L'(';
                output[outputPos++] = L'y';
                output[outputPos++] = L')';
                break;
            case 0x0179: // [LATIN CAPITAL LETTER Z WITH ACUTE]
            case 0x017B: // [LATIN CAPITAL LETTER Z WITH DOT ABOVE]
            case 0x017D: // [LATIN CAPITAL LETTER Z WITH CARON]
            case 0x01B5: // [LATIN CAPITAL LETTER Z WITH STROKE]
            case 0x021C: // [LATIN CAPITAL LETTER YOGH]
            case 0x0224: // [LATIN CAPITAL LETTER Z WITH HOOK]
            case 0x1D22: // [LATIN LETTER SMALL CAPITAL Z]
            case 0x1E90: // [LATIN CAPITAL LETTER Z WITH CIRCUMFLEX]
            case 0x1E92: // [LATIN CAPITAL LETTER Z WITH DOT BELOW]
            case 0x1E94: // [LATIN CAPITAL LETTER Z WITH LINE BELOW]
            case 0x24CF: // [CIRCLED LATIN CAPITAL LETTER Z]
            case 0x2C6B: // [LATIN CAPITAL LETTER Z WITH DESCENDER]
            case 0xA762: // [LATIN CAPITAL LETTER VISIGOTHIC Z]
            case 0xFF3A: // [FULLWIDTH LATIN CAPITAL LETTER Z]
                output[outputPos++] = L'Z';
                break;
            case 0x017A: // [LATIN SMALL LETTER Z WITH ACUTE]
            case 0x017C: // [LATIN SMALL LETTER Z WITH DOT ABOVE]
            case 0x017E: // [LATIN SMALL LETTER Z WITH CARON]
            case 0x01B6: // [LATIN SMALL LETTER Z WITH STROKE]
            case 0x021D: // [LATIN SMALL LETTER YOGH]
            case 0x0225: // [LATIN SMALL LETTER Z WITH HOOK]
            case 0x0240: // [LATIN SMALL LETTER Z WITH SWASH TAIL]
            case 0x0290: // [LATIN SMALL LETTER Z WITH RETROFLEX HOOK]
            case 0x0291: // [LATIN SMALL LETTER Z WITH CURL]
            case 0x1D76: // [LATIN SMALL LETTER Z WITH MIDDLE TILDE]
            case 0x1D8E: // [LATIN SMALL LETTER Z WITH PALATAL HOOK]
            case 0x1E91: // [LATIN SMALL LETTER Z WITH CIRCUMFLEX]
            case 0x1E93: // [LATIN SMALL LETTER Z WITH DOT BELOW]
            case 0x1E95: // [LATIN SMALL LETTER Z WITH LINE BELOW]
            case 0x24E9: // [CIRCLED LATIN SMALL LETTER Z]
            case 0x2C6C: // [LATIN SMALL LETTER Z WITH DESCENDER]
            case 0xA763: // [LATIN SMALL LETTER VISIGOTHIC Z]
            case 0xFF5A: // [FULLWIDTH LATIN SMALL LETTER Z]
                output[outputPos++] = L'z';
                break;
            case 0x24B5: // [PARENTHESIZED LATIN SMALL LETTER Z]
                output[outputPos++] = L'(';
                output[outputPos++] = L'z';
                output[outputPos++] = L')';
                break;
            case 0x2070: // [SUPERSCRIPT ZERO]
            case 0x2080: // [SUBSCRIPT ZERO]
            case 0x24EA: // [CIRCLED DIGIT ZERO]
            case 0x24FF: // [NEGATIVE CIRCLED DIGIT ZERO]
            case 0xFF10: // [FULLWIDTH DIGIT ZERO]
                output[outputPos++] = L'0';
                break;
            case 0x00B9: // [SUPERSCRIPT ONE]
            case 0x2081: // [SUBSCRIPT ONE]
            case 0x2460: // [CIRCLED DIGIT ONE]
            case 0x24F5: // [DOUBLE CIRCLED DIGIT ONE]
            case 0x2776: // [DINGBAT NEGATIVE CIRCLED DIGIT ONE]
            case 0x2780: // [DINGBAT CIRCLED SANS-SERIF DIGIT ONE]
            case 0x278A: // [DINGBAT NEGATIVE CIRCLED SANS-SERIF DIGIT ONE]
            case 0xFF11: // [FULLWIDTH DIGIT ONE]
                output[outputPos++] = L'1';
                break;
            case 0x2488: // [DIGIT ONE FULL STOP]
                output[outputPos++] = L'1';
                output[outputPos++] = L'.';
                break;
            case 0x2474: // [PARENTHESIZED DIGIT ONE]
                output[outputPos++] = L'(';
                output[outputPos++] = L'1';
                output[outputPos++] = L')';
                break;
            case 0x00B2: // [SUPERSCRIPT TWO]
            case 0x2082: // [SUBSCRIPT TWO]
            case 0x2461: // [CIRCLED DIGIT TWO]
            case 0x24F6: // [DOUBLE CIRCLED DIGIT TWO]
            case 0x2777: // [DINGBAT NEGATIVE CIRCLED DIGIT TWO]
            case 0x2781: // [DINGBAT CIRCLED SANS-SERIF DIGIT TWO]
            case 0x278B: // [DINGBAT NEGATIVE CIRCLED SANS-SERIF DIGIT TWO]
            case 0xFF12: // [FULLWIDTH DIGIT TWO]
                output[outputPos++] = L'2';
                break;
            case 0x2489: // [DIGIT TWO FULL STOP]
                output[outputPos++] = L'2';
                output[outputPos++] = L'.';
                break;
            case 0x2475: // [PARENTHESIZED DIGIT TWO]
                output[outputPos++] = L'(';
                output[outputPos++] = L'2';
                output[outputPos++] = L')';
                break;
            case 0x00B3: // [SUPERSCRIPT THREE]
            case 0x2083: // [SUBSCRIPT THREE]
            case 0x2462: // [CIRCLED DIGIT THREE]
            case 0x24F7: // [DOUBLE CIRCLED DIGIT THREE]
            case 0x2778: // [DINGBAT NEGATIVE CIRCLED DIGIT THREE]
            case 0x2782: // [DINGBAT CIRCLED SANS-SERIF DIGIT THREE]
            case 0x278C: // [DINGBAT NEGATIVE CIRCLED SANS-SERIF DIGIT THREE]
            case 0xFF13: // [FULLWIDTH DIGIT THREE]
                output[outputPos++] = L'3';
                break;
            case 0x248A: // [DIGIT THREE FULL STOP]
                output[outputPos++] = L'3';
                output[outputPos++] = L'.';
                break;
            case 0x2476: // [PARENTHESIZED DIGIT THREE]
                output[outputPos++] = L'(';
                output[outputPos++] = L'3';
                output[outputPos++] = L')';
                break;
            case 0x2074: // [SUPERSCRIPT FOUR]
            case 0x2084: // [SUBSCRIPT FOUR]
            case 0x2463: // [CIRCLED DIGIT FOUR]
            case 0x24F8: // [DOUBLE CIRCLED DIGIT FOUR]
            case 0x2779: // [DINGBAT NEGATIVE CIRCLED DIGIT FOUR]
            case 0x2783: // [DINGBAT CIRCLED SANS-SERIF DIGIT FOUR]
            case 0x278D: // [DINGBAT NEGATIVE CIRCLED SANS-SERIF DIGIT FOUR]
            case 0xFF14: // [FULLWIDTH DIGIT FOUR]
                output[outputPos++] = L'4';
                break;
            case 0x248B: // [DIGIT FOUR FULL STOP]
                output[outputPos++] = L'4';
                output[outputPos++] = L'.';
                break;
            case 0x2477: // [PARENTHESIZED DIGIT FOUR]
                output[outputPos++] = L'(';
                output[outputPos++] = L'4';
                output[outputPos++] = L')';
                break;
            case 0x2075: // [SUPERSCRIPT FIVE]
            case 0x2085: // [SUBSCRIPT FIVE]
            case 0x2464: // [CIRCLED DIGIT FIVE]
            case 0x24F9: // [DOUBLE CIRCLED DIGIT FIVE]
            case 0x277A: // [DINGBAT NEGATIVE CIRCLED DIGIT FIVE]
            case 0x2784: // [DINGBAT CIRCLED SANS-SERIF DIGIT FIVE]
            case 0x278E: // [DINGBAT NEGATIVE CIRCLED SANS-SERIF DIGIT FIVE]
            case 0xFF15: // [FULLWIDTH DIGIT FIVE]
                output[outputPos++] = L'5';
                break;
            case 0x248C: // [DIGIT FIVE FULL STOP]
                output[outputPos++] = L'5';
                output[outputPos++] = L'.';
                break;
            case 0x2478: // [PARENTHESIZED DIGIT FIVE]
                output[outputPos++] = L'(';
                output[outputPos++] = L'5';
                output[outputPos++] = L')';
                break;
            case 0x2076: // [SUPERSCRIPT SIX]
            case 0x2086: // [SUBSCRIPT SIX]
            case 0x2465: // [CIRCLED DIGIT SIX]
            case 0x24FA: // [DOUBLE CIRCLED DIGIT SIX]
            case 0x277B: // [DINGBAT NEGATIVE CIRCLED DIGIT SIX]
            case 0x2785: // [DINGBAT CIRCLED SANS-SERIF DIGIT SIX]
            case 0x278F: // [DINGBAT NEGATIVE CIRCLED SANS-SERIF DIGIT SIX]
            case 0xFF16: // [FULLWIDTH DIGIT SIX]
                output[outputPos++] = L'6';
                break;
            case 0x248D: // [DIGIT SIX FULL STOP]
                output[outputPos++] = L'6';
                output[outputPos++] = L'.';
                break;
            case 0x2479: // [PARENTHESIZED DIGIT SIX]
                output[outputPos++] = L'(';
                output[outputPos++] = L'6';
                output[outputPos++] = L')';
                break;
            case 0x2077: // [SUPERSCRIPT SEVEN]
            case 0x2087: // [SUBSCRIPT SEVEN]
            case 0x2466: // [CIRCLED DIGIT SEVEN]
            case 0x24FB: // [DOUBLE CIRCLED DIGIT SEVEN]
            case 0x277C: // [DINGBAT NEGATIVE CIRCLED DIGIT SEVEN]
            case 0x2786: // [DINGBAT CIRCLED SANS-SERIF DIGIT SEVEN]
            case 0x2790: // [DINGBAT NEGATIVE CIRCLED SANS-SERIF DIGIT SEVEN]
            case 0xFF17: // [FULLWIDTH DIGIT SEVEN]
                output[outputPos++] = L'7';
                break;
            case 0x248E: // [DIGIT SEVEN FULL STOP]
                output[outputPos++] = L'7';
                output[outputPos++] = L'.';
                break;
            case 0x247A: // [PARENTHESIZED DIGIT SEVEN]
                output[outputPos++] = L'(';
                output[outputPos++] = L'7';
                output[outputPos++] = L')';
                break;
            case 0x2078: // [SUPERSCRIPT EIGHT]
            case 0x2088: // [SUBSCRIPT EIGHT]
            case 0x2467: // [CIRCLED DIGIT EIGHT]
            case 0x24FC: // [DOUBLE CIRCLED DIGIT EIGHT]
            case 0x277D: // [DINGBAT NEGATIVE CIRCLED DIGIT EIGHT]
            case 0x2787: // [DINGBAT CIRCLED SANS-SERIF DIGIT EIGHT]
            case 0x2791: // [DINGBAT NEGATIVE CIRCLED SANS-SERIF DIGIT EIGHT]
            case 0xFF18: // [FULLWIDTH DIGIT EIGHT]
                output[outputPos++] = L'8';
                break;
            case 0x248F: // [DIGIT EIGHT FULL STOP]
                output[outputPos++] = L'8';
                output[outputPos++] = L'.';
                break;
            case 0x247B: // [PARENTHESIZED DIGIT EIGHT]
                output[outputPos++] = L'(';
                output[outputPos++] = L'8';
                output[outputPos++] = L')';
                break;
            case 0x2079: // [SUPERSCRIPT NINE]
            case 0x2089: // [SUBSCRIPT NINE]
            case 0x2468: // [CIRCLED DIGIT NINE]
            case 0x24FD: // [DOUBLE CIRCLED DIGIT NINE]
            case 0x277E: // [DINGBAT NEGATIVE CIRCLED DIGIT NINE]
            case 0x2788: // [DINGBAT CIRCLED SANS-SERIF DIGIT NINE]
            case 0x2792: // [DINGBAT NEGATIVE CIRCLED SANS-SERIF DIGIT NINE]
            case 0xFF19: // [FULLWIDTH DIGIT NINE]
                output[outputPos++] = L'9';
                break;
            case 0x2490: // [DIGIT NINE FULL STOP]
                output[outputPos++] = L'9';
                output[outputPos++] = L'.';
                break;
            case 0x247C: // [PARENTHESIZED DIGIT NINE]
                output[outputPos++] = L'(';
                output[outputPos++] = L'9';
                output[outputPos++] = L')';
                break;
            case 0x2469: // [CIRCLED NUMBER TEN]
            case 0x24FE: // [DOUBLE CIRCLED NUMBER TEN]
            case 0x277F: // [DINGBAT NEGATIVE CIRCLED NUMBER TEN]
            case 0x2789: // [DINGBAT CIRCLED SANS-SERIF NUMBER TEN]
            case 0x2793: // [DINGBAT NEGATIVE CIRCLED SANS-SERIF NUMBER TEN]
                output[outputPos++] = L'1';
                output[outputPos++] = L'0';
                break;
            case 0x2491: // [NUMBER TEN FULL STOP]
                output[outputPos++] = L'1';
                output[outputPos++] = L'0';
                output[outputPos++] = L'.';
                break;
            case 0x247D: // [PARENTHESIZED NUMBER TEN]
                output[outputPos++] = L'(';
                output[outputPos++] = L'1';
                output[outputPos++] = L'0';
                output[outputPos++] = L')';
                break;
            case 0x246A: // [CIRCLED NUMBER ELEVEN]
            case 0x24EB: // [NEGATIVE CIRCLED NUMBER ELEVEN]
                output[outputPos++] = L'1';
                output[outputPos++] = L'1';
                break;
            case 0x2492: // [NUMBER ELEVEN FULL STOP]
                output[outputPos++] = L'1';
                output[outputPos++] = L'1';
                output[outputPos++] = L'.';
                break;
            case 0x247E: // [PARENTHESIZED NUMBER ELEVEN]
                output[outputPos++] = L'(';
                output[outputPos++] = L'1';
                output[outputPos++] = L'1';
                output[outputPos++] = L')';
                break;
            case 0x246B: // [CIRCLED NUMBER TWELVE]
            case 0x24EC: // [NEGATIVE CIRCLED NUMBER TWELVE]
                output[outputPos++] = L'1';
                output[outputPos++] = L'2';
                break;
            case 0x2493: // [NUMBER TWELVE FULL STOP]
                output[outputPos++] = L'1';
                output[outputPos++] = L'2';
                output[outputPos++] = L'.';
                break;
            case 0x247F: // [PARENTHESIZED NUMBER TWELVE]
                output[outputPos++] = L'(';
                output[outputPos++] = L'1';
                output[outputPos++] = L'2';
                output[outputPos++] = L')';
                break;
            case 0x246C: // [CIRCLED NUMBER THIRTEEN]
            case 0x24ED: // [NEGATIVE CIRCLED NUMBER THIRTEEN]
                output[outputPos++] = L'1';
                output[outputPos++] = L'3';
                break;
            case 0x2494: // [NUMBER THIRTEEN FULL STOP]
                output[outputPos++] = L'1';
                output[outputPos++] = L'3';
                output[outputPos++] = L'.';
                break;
            case 0x2480: // [PARENTHESIZED NUMBER THIRTEEN]
                output[outputPos++] = L'(';
                output[outputPos++] = L'1';
                output[outputPos++] = L'3';
                output[outputPos++] = L')';
                break;
            case 0x246D: // [CIRCLED NUMBER FOURTEEN]
            case 0x24EE: // [NEGATIVE CIRCLED NUMBER FOURTEEN]
                output[outputPos++] = L'1';
                output[outputPos++] = L'4';
                break;
            case 0x2495: // [NUMBER FOURTEEN FULL STOP]
                output[outputPos++] = L'1';
                output[outputPos++] = L'4';
                output[outputPos++] = L'.';
                break;
            case 0x2481: // [PARENTHESIZED NUMBER FOURTEEN]
                output[outputPos++] = L'(';
                output[outputPos++] = L'1';
                output[outputPos++] = L'4';
                output[outputPos++] = L')';
                break;
            case 0x246E: // [CIRCLED NUMBER FIFTEEN]
            case 0x24EF: // [NEGATIVE CIRCLED NUMBER FIFTEEN]
                output[outputPos++] = L'1';
                output[outputPos++] = L'5';
                break;
            case 0x2496: // [NUMBER FIFTEEN FULL STOP]
                output[outputPos++] = L'1';
                output[outputPos++] = L'5';
                output[outputPos++] = L'.';
                break;
            case 0x2482: // [PARENTHESIZED NUMBER FIFTEEN]
                output[outputPos++] = L'(';
                output[outputPos++] = L'1';
                output[outputPos++] = L'5';
                output[outputPos++] = L')';
                break;
            case 0x246F: // [CIRCLED NUMBER SIXTEEN]
            case 0x24F0: // [NEGATIVE CIRCLED NUMBER SIXTEEN]
                output[outputPos++] = L'1';
                output[outputPos++] = L'6';
                break;
            case 0x2497: // [NUMBER SIXTEEN FULL STOP]
                output[outputPos++] = L'1';
                output[outputPos++] = L'6';
                output[outputPos++] = L'.';
                break;
            case 0x2483: // [PARENTHESIZED NUMBER SIXTEEN]
                output[outputPos++] = L'(';
                output[outputPos++] = L'1';
                output[outputPos++] = L'6';
                output[outputPos++] = L')';
                break;
            case 0x2470: // [CIRCLED NUMBER SEVENTEEN]
            case 0x24F1: // [NEGATIVE CIRCLED NUMBER SEVENTEEN]
                output[outputPos++] = L'1';
                output[outputPos++] = L'7';
                break;
            case 0x2498: // [NUMBER SEVENTEEN FULL STOP]
                output[outputPos++] = L'1';
                output[outputPos++] = L'7';
                output[outputPos++] = L'.';
                break;
            case 0x2484: // [PARENTHESIZED NUMBER SEVENTEEN]
                output[outputPos++] = L'(';
                output[outputPos++] = L'1';
                output[outputPos++] = L'7';
                output[outputPos++] = L')';
                break;
            case 0x2471: // [CIRCLED NUMBER EIGHTEEN]
            case 0x24F2: // [NEGATIVE CIRCLED NUMBER EIGHTEEN]
                output[outputPos++] = L'1';
                output[outputPos++] = L'8';
                break;
            case 0x2499: // [NUMBER EIGHTEEN FULL STOP]
                output[outputPos++] = L'1';
                output[outputPos++] = L'8';
                output[outputPos++] = L'.';
                break;
            case 0x2485: // [PARENTHESIZED NUMBER EIGHTEEN]
                output[outputPos++] = L'(';
                output[outputPos++] = L'1';
                output[outputPos++] = L'8';
                output[outputPos++] = L')';
                break;
            case 0x2472: // [CIRCLED NUMBER NINETEEN]
            case 0x24F3: // [NEGATIVE CIRCLED NUMBER NINETEEN]
                output[outputPos++] = L'1';
                output[outputPos++] = L'9';
                break;
            case 0x249A: // [NUMBER NINETEEN FULL STOP]
                output[outputPos++] = L'1';
                output[outputPos++] = L'9';
                output[outputPos++] = L'.';
                break;
            case 0x2486: // [PARENTHESIZED NUMBER NINETEEN]
                output[outputPos++] = L'(';
                output[outputPos++] = L'1';
                output[outputPos++] = L'9';
                output[outputPos++] = L')';
                break;
            case 0x2473: // [CIRCLED NUMBER TWENTY]
            case 0x24F4: // [NEGATIVE CIRCLED NUMBER TWENTY]
                output[outputPos++] = L'2';
                output[outputPos++] = L'0';
                break;
            case 0x249B: // [NUMBER TWENTY FULL STOP]
                output[outputPos++] = L'2';
                output[outputPos++] = L'0';
                output[outputPos++] = L'.';
                break;
            case 0x2487: // [PARENTHESIZED NUMBER TWENTY]
                output[outputPos++] = L'(';
                output[outputPos++] = L'2';
                output[outputPos++] = L'0';
                output[outputPos++] = L')';
                break;
            case 0x00AB: // [LEFT-POINTING DOUBLE ANGLE QUOTATION MARK]
            case 0x00BB: // [RIGHT-POINTING DOUBLE ANGLE QUOTATION MARK]
            case 0x201C: // [LEFT DOUBLE QUOTATION MARK]
            case 0x201D: // [RIGHT DOUBLE QUOTATION MARK]
            case 0x201E: // [DOUBLE LOW-9 QUOTATION MARK]
            case 0x2033: // [DOUBLE PRIME]
            case 0x2036: // [REVERSED DOUBLE PRIME]
            case 0x275D: // [HEAVY DOUBLE TURNED COMMA QUOTATION MARK ORNAMENT]
            case 0x275E: // [HEAVY DOUBLE COMMA QUOTATION MARK ORNAMENT]
            case 0x276E: // [HEAVY LEFT-POINTING ANGLE QUOTATION MARK ORNAMENT]
            case 0x276F: // [HEAVY RIGHT-POINTING ANGLE QUOTATION MARK ORNAMENT]
            case 0xFF02: // [FULLWIDTH QUOTATION MARK]
                output[outputPos++] = L'"';
                break;
            case 0x2018: // [LEFT SINGLE QUOTATION MARK]
            case 0x2019: // [RIGHT SINGLE QUOTATION MARK]
            case 0x201A: // [SINGLE LOW-9 QUOTATION MARK]
            case 0x201B: // [SINGLE HIGH-REVERSED-9 QUOTATION MARK]
            case 0x2032: // [PRIME]
            case 0x2035: // [REVERSED PRIME]
            case 0x2039: // [SINGLE LEFT-POINTING ANGLE QUOTATION MARK]
            case 0x203A: // [SINGLE RIGHT-POINTING ANGLE QUOTATION MARK]
            case 0x275B: // [HEAVY SINGLE TURNED COMMA QUOTATION MARK ORNAMENT]
            case 0x275C: // [HEAVY SINGLE COMMA QUOTATION MARK ORNAMENT]
            case 0xFF07: // [FULLWIDTH APOSTROPHE]
                output[outputPos++] = L'\'';
                break;
            case 0x2010: // [HYPHEN]
            case 0x2011: // [NON-BREAKING HYPHEN]
            case 0x2012: // [FIGURE DASH]
            case 0x2013: // [EN DASH]
            case 0x2014: // [EM DASH]
            case 0x207B: // [SUPERSCRIPT MINUS]
            case 0x208B: // [SUBSCRIPT MINUS]
            case 0xFF0D: // [FULLWIDTH HYPHEN-MINUS]
                output[outputPos++] = L'-';
                break;
            case 0x2045: // [LEFT SQUARE BRACKET WITH QUILL]
            case 0x2772: // [LIGHT LEFT TORTOISE SHELL BRACKET ORNAMENT]
            case 0xFF3B: // [FULLWIDTH LEFT SQUARE BRACKET]
                output[outputPos++] = L'[';
                break;
            case 0x2046: // [RIGHT SQUARE BRACKET WITH QUILL]
            case 0x2773: // [LIGHT RIGHT TORTOISE SHELL BRACKET ORNAMENT]
            case 0xFF3D: // [FULLWIDTH RIGHT SQUARE BRACKET]
                output[outputPos++] = L']';
                break;
            case 0x207D: // [SUPERSCRIPT LEFT PARENTHESIS]
            case 0x208D: // [SUBSCRIPT LEFT PARENTHESIS]
            case 0x2768: // [MEDIUM LEFT PARENTHESIS ORNAMENT]
            case 0x276A: // [MEDIUM FLATTENED LEFT PARENTHESIS ORNAMENT]
            case 0xFF08: // [FULLWIDTH LEFT PARENTHESIS]
                output[outputPos++] = L'(';
                break;
            case 0x2E28: // [LEFT DOUBLE PARENTHESIS]
                output[outputPos++] = L'(';
                output[outputPos++] = L'(';
                break;
            case 0x207E: // [SUPERSCRIPT RIGHT PARENTHESIS]
            case 0x208E: // [SUBSCRIPT RIGHT PARENTHESIS]
            case 0x2769: // [MEDIUM RIGHT PARENTHESIS ORNAMENT]
            case 0x276B: // [MEDIUM FLATTENED RIGHT PARENTHESIS ORNAMENT]
            case 0xFF09: // [FULLWIDTH RIGHT PARENTHESIS]
                output[outputPos++] = L')';
                break;
            case 0x2E29: // [RIGHT DOUBLE PARENTHESIS]
                output[outputPos++] = L')';
                output[outputPos++] = L')';
                break;
            case 0x276C: // [MEDIUM LEFT-POINTING ANGLE BRACKET ORNAMENT]
            case 0x2770: // [HEAVY LEFT-POINTING ANGLE BRACKET ORNAMENT]
            case 0xFF1C: // [FULLWIDTH LESS-THAN SIGN]
                output[outputPos++] = L'<';
                break;
            case 0x276D: // [MEDIUM RIGHT-POINTING ANGLE BRACKET ORNAMENT]
            case 0x2771: // [HEAVY RIGHT-POINTING ANGLE BRACKET ORNAMENT]
            case 0xFF1E: // [FULLWIDTH GREATER-THAN SIGN]
                output[outputPos++] = L'>';
                break;
            case 0x2774: // [MEDIUM LEFT CURLY BRACKET ORNAMENT]
            case 0xFF5B: // [FULLWIDTH LEFT CURLY BRACKET]
                output[outputPos++] = L'{';
                break;
            case 0x2775: // [MEDIUM RIGHT CURLY BRACKET ORNAMENT]
            case 0xFF5D: // [FULLWIDTH RIGHT CURLY BRACKET]
                output[outputPos++] = L'}';
                break;
            case 0x207A: // [SUPERSCRIPT PLUS SIGN]
            case 0x208A: // [SUBSCRIPT PLUS SIGN]
            case 0xFF0B: // [FULLWIDTH PLUS SIGN]
                output[outputPos++] = L'+';
                break;
            case 0x207C: // [SUPERSCRIPT EQUALS SIGN]
            case 0x208C: // [SUBSCRIPT EQUALS SIGN]
            case 0xFF1D: // [FULLWIDTH EQUALS SIGN]
                output[outputPos++] = L'=';
                break;
            case 0xFF01: // [FULLWIDTH EXCLAMATION MARK]
                output[outputPos++] = L'!';
                break;
            case 0x203C: // [DOUBLE EXCLAMATION MARK]
                output[outputPos++] = L'!';
                output[outputPos++] = L'!';
                break;
            case 0x2049: // [EXCLAMATION QUESTION MARK]
                output[outputPos++] = L'!';
                output[outputPos++] = L'?';
                break;
            case 0xFF03: // [FULLWIDTH NUMBER SIGN]
                output[outputPos++] = L'#';
                break;
            case 0xFF04: // [FULLWIDTH DOLLAR SIGN]
                output[outputPos++] = L'$';
                break;
            case 0x2052: // [COMMERCIAL MINUS SIGN]
            case 0xFF05: // [FULLWIDTH PERCENT SIGN]
                output[outputPos++] = L'%';
                break;
            case 0xFF06: // [FULLWIDTH AMPERSAND]
                output[outputPos++] = L'&';
                break;
            case 0x204E: // [LOW ASTERISK]
            case 0xFF0A: // [FULLWIDTH ASTERISK]
                output[outputPos++] = L'*';
                break;
            case 0xFF0C: // [FULLWIDTH COMMA]
                output[outputPos++] = L',';
                break;
            case 0xFF0E: // [FULLWIDTH FULL STOP]
                output[outputPos++] = L'.';
                break;
            case 0x2044: // [FRACTION SLASH]
            case 0xFF0F: // [FULLWIDTH SOLIDUS]
                output[outputPos++] = L'/';
                break;
            case 0xFF1A: // [FULLWIDTH COLON]
                output[outputPos++] = L':';
                break;
            case 0x204F: // [REVERSED SEMICOLON]
            case 0xFF1B: // [FULLWIDTH SEMICOLON]
                output[outputPos++] = L';';
                break;
            case 0xFF1F: // [FULLWIDTH QUESTION MARK]
                output[outputPos++] = L'?';
                break;
            case 0x2047: // [DOUBLE QUESTION MARK]
                output[outputPos++] = L'?';
                output[outputPos++] = L'?';
                break;
            case 0x2048: // [QUESTION EXCLAMATION MARK]
                output[outputPos++] = L'?';
                output[outputPos++] = L'!';
                break;
            case 0xFF20: // [FULLWIDTH COMMERCIAL AT]
                output[outputPos++] = L'@';
                break;
            case 0xFF3C: // [FULLWIDTH REVERSE SOLIDUS]
                output[outputPos++] = L'\\';
                break;
            case 0x2038: // [CARET]
            case 0xFF3E: // [FULLWIDTH CIRCUMFLEX ACCENT]
                output[outputPos++] = L'^';
                break;
            case 0xFF3F: // [FULLWIDTH LOW LINE]
                output[outputPos++] = L'_';
                break;
            case 0x2053: // [SWUNG DASH]
            case 0xFF5E: // [FULLWIDTH TILDE]
                output[outputPos++] = L'~';
                break;
            default:
                output[outputPos++] = c;
                break;
            }
        }
    }
}

}
