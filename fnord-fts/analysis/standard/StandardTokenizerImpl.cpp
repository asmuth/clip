/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "fnord-fts/LuceneInc.h"
#include "StandardTokenizerImpl.h"
#include "StandardTokenizer.h"
#include "Reader.h"
#include "fnord-fts/analysis/Token.h"
#include "TermAttribute.h"
#include "fnord-fts/util/MiscUtils.h"

#include <boost/thread/once.hpp>

namespace Lucene {

/// Initial size of the lookahead buffer
const int32_t StandardTokenizerImpl::ZZ_BUFFERSIZE = 16384;

/// Translates characters to character classes
CharArray StandardTokenizerImpl::_ZZ_CMAP;
const wchar_t StandardTokenizerImpl::ZZ_CMAP_PACKED[] = {
    L"\11\0\1\0\1\15\1\0\1\0\1\14\22\0\1\0\5\0\1\5"
    L"\1\3\4\0\1\11\1\7\1\4\1\11\12\2\6\0\1\6\32\12"
    L"\4\0\1\10\1\0\32\12\57\0\1\12\12\0\1\12\4\0\1\12"
    L"\5\0\27\12\1\0\37\12\1\0\u0128\12\2\0\22\12\34\0\136\12"
    L"\2\0\11\12\2\0\7\12\16\0\2\12\16\0\5\12\11\0\1\12"
    L"\213\0\1\12\13\0\1\12\1\0\3\12\1\0\1\12\1\0\24\12"
    L"\1\0\54\12\1\0\10\12\2\0\32\12\14\0\202\12\12\0\71\12"
    L"\2\0\2\12\2\0\2\12\3\0\46\12\2\0\2\12\67\0\46\12"
    L"\2\0\1\12\7\0\47\12\110\0\33\12\5\0\3\12\56\0\32\12"
    L"\5\0\13\12\25\0\12\2\7\0\143\12\1\0\1\12\17\0\2\12"
    L"\11\0\12\2\3\12\23\0\1\12\1\0\33\12\123\0\46\12\u015f\0"
    L"\65\12\3\0\1\12\22\0\1\12\7\0\12\12\4\0\12\2\25\0"
    L"\10\12\2\0\2\12\2\0\26\12\1\0\7\12\1\0\1\12\3\0"
    L"\4\12\42\0\2\12\1\0\3\12\4\0\12\2\2\12\23\0\6\12"
    L"\4\0\2\12\2\0\26\12\1\0\7\12\1\0\2\12\1\0\2\12"
    L"\1\0\2\12\37\0\4\12\1\0\1\12\7\0\12\2\2\0\3\12"
    L"\20\0\7\12\1\0\1\12\1\0\3\12\1\0\26\12\1\0\7\12"
    L"\1\0\2\12\1\0\5\12\3\0\1\12\22\0\1\12\17\0\1\12"
    L"\5\0\12\2\25\0\10\12\2\0\2\12\2\0\26\12\1\0\7\12"
    L"\1\0\2\12\2\0\4\12\3\0\1\12\36\0\2\12\1\0\3\12"
    L"\4\0\12\2\25\0\6\12\3\0\3\12\1\0\4\12\3\0\2\12"
    L"\1\0\1\12\1\0\2\12\3\0\2\12\3\0\3\12\3\0\10\12"
    L"\1\0\3\12\55\0\11\2\25\0\10\12\1\0\3\12\1\0\27\12"
    L"\1\0\12\12\1\0\5\12\46\0\2\12\4\0\12\2\25\0\10\12"
    L"\1\0\3\12\1\0\27\12\1\0\12\12\1\0\5\12\44\0\1\12"
    L"\1\0\2\12\4\0\12\2\25\0\10\12\1\0\3\12\1\0\27\12"
    L"\1\0\20\12\46\0\2\12\4\0\12\2\25\0\22\12\3\0\30\12"
    L"\1\0\11\12\1\0\1\12\2\0\7\12\71\0\1\1\60\12\1\1"
    L"\2\12\14\1\7\12\11\1\12\2\47\0\2\12\1\0\1\12\2\0"
    L"\2\12\1\0\1\12\2\0\1\12\6\0\4\12\1\0\7\12\1\0"
    L"\3\12\1\0\1\12\1\0\1\12\2\0\2\12\1\0\4\12\1\0"
    L"\2\12\11\0\1\12\2\0\5\12\1\0\1\12\11\0\12\2\2\0"
    L"\2\12\42\0\1\12\37\0\12\2\26\0\10\12\1\0\42\12\35\0"
    L"\4\12\164\0\42\12\1\0\5\12\1\0\2\12\25\0\12\2\6\0"
    L"\6\12\112\0\46\12\12\0\47\12\11\0\132\12\5\0\104\12\5\0"
    L"\122\12\6\0\7\12\1\0\77\12\1\0\1\12\1\0\4\12\2\0"
    L"\7\12\1\0\1\12\1\0\4\12\2\0\47\12\1\0\1\12\1\0"
    L"\4\12\2\0\37\12\1\0\1\12\1\0\4\12\2\0\7\12\1\0"
    L"\1\12\1\0\4\12\2\0\7\12\1\0\7\12\1\0\27\12\1\0"
    L"\37\12\1\0\1\12\1\0\4\12\2\0\7\12\1\0\47\12\1\0"
    L"\23\12\16\0\11\2\56\0\125\12\14\0\u026c\12\2\0\10\12\12\0"
    L"\32\12\5\0\113\12\225\0\64\12\54\0\12\2\46\0\12\2\6\0"
    L"\130\12\10\0\51\12\u0557\0\234\12\4\0\132\12\6\0\26\12\2\0"
    L"\6\12\2\0\46\12\2\0\6\12\2\0\10\12\1\0\1\12\1\0"
    L"\1\12\1\0\1\12\1\0\37\12\2\0\65\12\1\0\7\12\1\0"
    L"\1\12\3\0\3\12\1\0\7\12\3\0\4\12\2\0\6\12\4\0"
    L"\15\12\5\0\3\12\1\0\7\12\202\0\1\12\202\0\1\12\4\0"
    L"\1\12\2\0\12\12\1\0\1\12\3\0\5\12\6\0\1\12\1\0"
    L"\1\12\1\0\1\12\1\0\4\12\1\0\3\12\1\0\7\12\u0ecb\0"
    L"\2\12\52\0\5\12\12\0\1\13\124\13\10\13\2\13\2\13\132\13"
    L"\1\13\3\13\6\13\50\13\3\13\1\0\136\12\21\0\30\12\70\0"
    L"\20\13\u0100\0\200\13\200\0\u19b6\13\12\13\100\0\u51a6\13\132\13\u048d\12"
    L"\u0773\0\u2ba4\12\u215c\0\u012e\13\322\13\7\12\14\0\5\12\5\0\1\12"
    L"\1\0\12\12\1\0\15\12\1\0\5\12\1\0\1\12\1\0\2\12"
    L"\1\0\2\12\1\0\154\12\41\0\u016b\12\22\0\100\12\2\0\66\12"
    L"\50\0\14\12\164\0\3\12\1\0\1\12\1\0\207\12\23\0\12\2"
    L"\7\0\32\12\6\0\32\12\12\0\1\13\72\13\37\12\3\0\6\12"
    L"\2\0\6\12\2\0\6\12\2\0\3\12\43\0"
};

const int32_t StandardTokenizerImpl::ZZ_CMAP_LENGTH = 65536;
const int32_t StandardTokenizerImpl::ZZ_CMAP_PACKED_LENGTH = 1154;

IntArray StandardTokenizerImpl::_ZZ_ACTION;
const wchar_t StandardTokenizerImpl::ZZ_ACTION_PACKED_0[] = {
    L"\1\0\1\1\3\2\1\3\1\1\13\0\1\2\3\4"
    L"\2\0\1\5\1\0\1\5\3\4\6\5\1\6\1\4"
    L"\2\7\1\10\1\0\1\10\3\0\2\10\1\11\1\12"
    L"\1\4"
};

const int32_t StandardTokenizerImpl::ZZ_ACTION_LENGTH = 51;
const int32_t StandardTokenizerImpl::ZZ_ACTION_PACKED_LENGTH = 50;

IntArray StandardTokenizerImpl::_ZZ_ROWMAP;
const wchar_t StandardTokenizerImpl::ZZ_ROWMAP_PACKED_0[] = {
    L"\0\0\0\16\0\34\0\52\0\70\0\16\0\106\0\124"
    L"\0\142\0\160\0\176\0\214\0\232\0\250\0\266\0\304"
    L"\0\322\0\340\0\356\0\374\0\u010a\0\u0118\0\u0126\0\u0134"
    L"\0\u0142\0\u0150\0\u015e\0\u016c\0\u017a\0\u0188\0\u0196\0\u01a4"
    L"\0\u01b2\0\u01c0\0\u01ce\0\u01dc\0\u01ea\0\u01f8\0\322\0\u0206"
    L"\0\u0214\0\u0222\0\u0230\0\u023e\0\u024c\0\u025a\0\124\0\214"
    L"\0\u0268\0\u0276\0\u0284"
};

const int32_t StandardTokenizerImpl::ZZ_ROWMAP_LENGTH = 51;
const int32_t StandardTokenizerImpl::ZZ_ROWMAP_PACKED_LENGTH = 102;

IntArray StandardTokenizerImpl::_ZZ_TRANS;
const wchar_t StandardTokenizerImpl::ZZ_TRANS_PACKED_0[] = {
    L"\1\2\1\3\1\4\7\2\1\5\1\6\1\7\1\2"
    L"\17\0\2\3\1\0\1\10\1\0\1\11\2\12\1\13"
    L"\1\3\4\0\1\3\1\4\1\0\1\14\1\0\1\11"
    L"\2\15\1\16\1\4\4\0\1\3\1\4\1\17\1\20"
    L"\1\21\1\22\2\12\1\13\1\23\20\0\1\2\1\0"
    L"\1\24\1\25\7\0\1\26\4\0\2\27\7\0\1\27"
    L"\4\0\1\30\1\31\7\0\1\32\5\0\1\33\7\0"
    L"\1\13\4\0\1\34\1\35\7\0\1\36\4\0\1\37"
    L"\1\40\7\0\1\41\4\0\1\42\1\43\7\0\1\44"
    L"\15\0\1\45\4\0\1\24\1\25\7\0\1\46\15\0"
    L"\1\47\4\0\2\27\7\0\1\50\4\0\1\3\1\4"
    L"\1\17\1\10\1\21\1\22\2\12\1\13\1\23\4\0"
    L"\2\24\1\0\1\51\1\0\1\11\2\52\1\0\1\24"
    L"\4\0\1\24\1\25\1\0\1\53\1\0\1\11\2\54"
    L"\1\55\1\25\4\0\1\24\1\25\1\0\1\51\1\0"
    L"\1\11\2\52\1\0\1\26\4\0\2\27\1\0\1\56"
    L"\2\0\1\56\2\0\1\27\4\0\2\30\1\0\1\52"
    L"\1\0\1\11\2\52\1\0\1\30\4\0\1\30\1\31"
    L"\1\0\1\54\1\0\1\11\2\54\1\55\1\31\4\0"
    L"\1\30\1\31\1\0\1\52\1\0\1\11\2\52\1\0"
    L"\1\32\5\0\1\33\1\0\1\55\2\0\3\55\1\33"
    L"\4\0\2\34\1\0\1\57\1\0\1\11\2\12\1\13"
    L"\1\34\4\0\1\34\1\35\1\0\1\60\1\0\1\11"
    L"\2\15\1\16\1\35\4\0\1\34\1\35\1\0\1\57"
    L"\1\0\1\11\2\12\1\13\1\36\4\0\2\37\1\0"
    L"\1\12\1\0\1\11\2\12\1\13\1\37\4\0\1\37"
    L"\1\40\1\0\1\15\1\0\1\11\2\15\1\16\1\40"
    L"\4\0\1\37\1\40\1\0\1\12\1\0\1\11\2\12"
    L"\1\13\1\41\4\0\2\42\1\0\1\13\2\0\3\13"
    L"\1\42\4\0\1\42\1\43\1\0\1\16\2\0\3\16"
    L"\1\43\4\0\1\42\1\43\1\0\1\13\2\0\3\13"
    L"\1\44\6\0\1\17\6\0\1\45\4\0\1\24\1\25"
    L"\1\0\1\61\1\0\1\11\2\52\1\0\1\26\4\0"
    L"\2\27\1\0\1\56\2\0\1\56\2\0\1\50\4\0"
    L"\2\24\7\0\1\24\4\0\2\30\7\0\1\30\4\0"
    L"\2\34\7\0\1\34\4\0\2\37\7\0\1\37\4\0"
    L"\2\42\7\0\1\42\4\0\2\62\7\0\1\62\4\0"
    L"\2\24\7\0\1\63\4\0\2\62\1\0\1\56\2\0"
    L"\1\56\2\0\1\62\4\0\2\24\1\0\1\61\1\0"
    L"\1\11\2\52\1\0\1\24\3\0"
};

const int32_t StandardTokenizerImpl::ZZ_TRANS_LENGTH = 658;
const int32_t StandardTokenizerImpl::ZZ_TRANS_PACKED_LENGTH = 634;

const int32_t StandardTokenizerImpl::ZZ_UNKNOWN_ERROR = 0;
const int32_t StandardTokenizerImpl::ZZ_NO_MATCH = 1;
const int32_t StandardTokenizerImpl::ZZ_PUSHBACK_2BIG = 2;

const wchar_t* StandardTokenizerImpl::ZZ_ERROR_MSG[] = {
    L"Unknown internal scanner error",
    L"Error: could not match input",
    L"Error: pushback value was too large"
};

IntArray StandardTokenizerImpl::_ZZ_ATTRIBUTE;
const wchar_t StandardTokenizerImpl::ZZ_ATTRIBUTE_PACKED_0[] = {
    L"\1\0\1\11\3\1\1\11\1\1\13\0\4\1\2\0"
    L"\1\1\1\0\17\1\1\0\1\1\3\0\5\1"
};

const int32_t StandardTokenizerImpl::ZZ_ATTRIBUTE_LENGTH = 51;
const int32_t StandardTokenizerImpl::ZZ_ATTRIBUTE_PACKED_LENGTH = 30;

/// This character denotes the end of file
const int32_t StandardTokenizerImpl::YYEOF = -1;

/// Lexical states
const int32_t StandardTokenizerImpl::YYINITIAL = 0;

StandardTokenizerImpl::StandardTokenizerImpl(const ReaderPtr& in) {
    this->zzState = 0;
    this->zzLexicalState = YYINITIAL;
    this->zzBuffer = CharArray::newInstance(ZZ_BUFFERSIZE);
    this->zzMarkedPos = 0;
    this->zzPushbackPos = 0;
    this->zzCurrentPos = 0;
    this->zzStartRead = 0;
    this->zzEndRead = 0;
    this->yyline = 0;
    this->_yychar = 0;
    this->yycolumn = 0;
    this->zzAtBOL = true;
    this->zzAtEOF = false;
    this->zzReader = in;
}

StandardTokenizerImpl::~StandardTokenizerImpl() {
}

void StandardTokenizerImpl::ZZ_CMAP_INIT() {
    _ZZ_CMAP = CharArray::newInstance(ZZ_CMAP_LENGTH);
    wchar_t* result = _ZZ_CMAP.get();

    int32_t i = 0; // index in packed string
    int32_t j = 0; // index in unpacked array
    while (i < ZZ_CMAP_PACKED_LENGTH) {
        int32_t count = ZZ_CMAP_PACKED[i++];
        wchar_t value = ZZ_CMAP_PACKED[i++];
        do {
            result[j++] = value;
        } while (--count > 0);
    }
}

const wchar_t* StandardTokenizerImpl::ZZ_CMAP() {
    static boost::once_flag once = BOOST_ONCE_INIT;
    boost::call_once(once, ZZ_CMAP_INIT);
    return _ZZ_CMAP.get();
}

void StandardTokenizerImpl::ZZ_ACTION_INIT() {
    _ZZ_ACTION = IntArray::newInstance(ZZ_ACTION_LENGTH);
    int32_t* result = _ZZ_ACTION.get();

    int32_t i = 0; // index in packed string
    int32_t j = 0; // index in unpacked array
    while (i < ZZ_ACTION_PACKED_LENGTH) {
        int32_t count = ZZ_ACTION_PACKED_0[i++];
        int32_t value = ZZ_ACTION_PACKED_0[i++];
        do {
            result[j++] = value;
        } while (--count > 0);
    }
}

const int32_t* StandardTokenizerImpl::ZZ_ACTION() {
    static boost::once_flag once = BOOST_ONCE_INIT;
    boost::call_once(once, ZZ_ACTION_INIT);
    return _ZZ_ACTION.get();
}

void StandardTokenizerImpl::ZZ_ROWMAP_INIT() {
    _ZZ_ROWMAP = IntArray::newInstance(ZZ_ROWMAP_LENGTH);
    int32_t* result = _ZZ_ROWMAP.get();

    int32_t i = 0; // index in packed string
    int32_t j = 0; // index in unpacked array
    while (i < ZZ_ROWMAP_PACKED_LENGTH) {
        int32_t high = ZZ_ROWMAP_PACKED_0[i++] << 16;
        result[j++] = high | ZZ_ROWMAP_PACKED_0[i++];
    }
}

const int32_t* StandardTokenizerImpl::ZZ_ROWMAP() {
    static boost::once_flag once = BOOST_ONCE_INIT;
    boost::call_once(once, ZZ_ROWMAP_INIT);
    return _ZZ_ROWMAP.get();
}

void StandardTokenizerImpl::ZZ_TRANS_INIT() {
    _ZZ_TRANS = IntArray::newInstance(ZZ_TRANS_LENGTH);
    int32_t* result = _ZZ_TRANS.get();

    int32_t i = 0; // index in packed string
    int32_t j = 0; // index in unpacked array
    while (i < ZZ_TRANS_PACKED_LENGTH) {
        int32_t count = ZZ_TRANS_PACKED_0[i++];
        int32_t value = ZZ_TRANS_PACKED_0[i++];
        --value;
        do {
            result[j++] = value;
        } while (--count > 0);
    }
}

const int32_t* StandardTokenizerImpl::ZZ_TRANS() {
    static boost::once_flag once = BOOST_ONCE_INIT;
    boost::call_once(once, ZZ_TRANS_INIT);
    return _ZZ_TRANS.get();
}

void StandardTokenizerImpl::ZZ_ATTRIBUTE_INIT() {
    _ZZ_ATTRIBUTE = IntArray::newInstance(ZZ_ATTRIBUTE_LENGTH);
    int32_t* result = _ZZ_ATTRIBUTE.get();

    int32_t i = 0; // index in packed string
    int32_t j = 0; // index in unpacked array
    while (i < ZZ_ATTRIBUTE_PACKED_LENGTH) {
        int32_t count = ZZ_ATTRIBUTE_PACKED_0[i++];
        int32_t value = ZZ_ATTRIBUTE_PACKED_0[i++];
        do {
            result[j++] = value;
        } while (--count > 0);
    }
}

const int32_t* StandardTokenizerImpl::ZZ_ATTRIBUTE() {
    static boost::once_flag once = BOOST_ONCE_INIT;
    boost::call_once(once, ZZ_ATTRIBUTE_INIT);
    return _ZZ_ATTRIBUTE.get();
}

int32_t StandardTokenizerImpl::yychar() {
    return _yychar;
}

void StandardTokenizerImpl::reset(const ReaderPtr& r) {
    // reset to default buffer size, if buffer has grown
    if (zzBuffer.size() > ZZ_BUFFERSIZE) {
        zzBuffer.resize(ZZ_BUFFERSIZE);
    }
    yyreset(r);
}

void StandardTokenizerImpl::getText(const TokenPtr& t) {
    t->setTermBuffer(zzBuffer.get(), zzStartRead, zzMarkedPos - zzStartRead);
}

void StandardTokenizerImpl::getText(const TermAttributePtr& t) {
    t->setTermBuffer(zzBuffer.get(), zzStartRead, zzMarkedPos - zzStartRead);
}

bool StandardTokenizerImpl::zzRefill() {
    // first: make room (if you can)
    if (zzStartRead > 0) {
        MiscUtils::arrayCopy(zzBuffer.get(), zzStartRead, zzBuffer.get(), 0, zzEndRead - zzStartRead);

        // translate stored positions
        zzEndRead -= zzStartRead;
        zzCurrentPos -= zzStartRead;
        zzMarkedPos -= zzStartRead;
        zzPushbackPos -= zzStartRead;
        zzStartRead = 0;
    }

    // is the buffer big enough?
    if (zzCurrentPos >= zzBuffer.size()) {
        zzBuffer.resize(zzCurrentPos * 2);
    }

    // finally: fill the buffer with new input
    int32_t numRead = zzReader->read(zzBuffer.get(), zzEndRead, zzBuffer.size() - zzEndRead);

    if (numRead < 0) {
        return true;
    } else {
        zzEndRead += numRead;
        return false;
    }
}

void StandardTokenizerImpl::yyclose() {
    zzAtEOF = true; // indicate end of file
    zzEndRead = zzStartRead; // invalidate buffer

    if (zzReader) {
        zzReader->close();
    }
}

void StandardTokenizerImpl::yyreset(const ReaderPtr& reader) {
    zzReader = reader;
    zzAtBOL = true;
    zzAtEOF = false;
    zzEndRead = 0;
    zzStartRead = 0;
    zzCurrentPos = 0;
    zzMarkedPos = 0;
    zzPushbackPos = 0;
    yyline = 0;
    _yychar = 0;
    yycolumn = 0;
    zzLexicalState = YYINITIAL;
}

int32_t StandardTokenizerImpl::yystate() {
    return zzLexicalState;
}

void StandardTokenizerImpl::yybegin(int32_t newState) {
    zzLexicalState = newState;
}

String StandardTokenizerImpl::yytext() {
    return String(zzBuffer.get() + zzStartRead, zzMarkedPos - zzStartRead);
}

wchar_t StandardTokenizerImpl::yycharat(int32_t pos) {
    return zzBuffer[zzStartRead + pos];
}

int32_t StandardTokenizerImpl::yylength() {
    return zzMarkedPos - zzStartRead;
}

void StandardTokenizerImpl::zzScanError(int32_t errorCode) {
    boost::throw_exception(ParseException(ZZ_ERROR_MSG[errorCode]));
}

void StandardTokenizerImpl::yypushback(int32_t number) {
    if (number > yylength()) {
        zzScanError(ZZ_PUSHBACK_2BIG);
    }
    zzMarkedPos -= number;
}

int32_t StandardTokenizerImpl::getNextToken() {
    int32_t zzInput;
    int32_t zzAction;

    // cached fields
    int32_t zzCurrentPosL;
    int32_t zzMarkedPosL;
    int32_t zzEndReadL = zzEndRead;
    wchar_t* zzBufferL = zzBuffer.get();
    const wchar_t* zzCMapL = ZZ_CMAP();

    const int32_t* zzTransL = ZZ_TRANS();
    const int32_t* zzRowMapL = ZZ_ROWMAP();
    const int32_t* zzAttrL = ZZ_ATTRIBUTE();
    const int32_t* zzActionL = ZZ_ACTION();

    while (true) {
        zzMarkedPosL = zzMarkedPos;
        _yychar += zzMarkedPosL - zzStartRead;
        zzAction = -1;
        zzCurrentPosL = zzMarkedPosL;
        zzCurrentPos = zzMarkedPosL;
        zzStartRead = zzMarkedPosL;
        zzState = zzLexicalState;

        while (true) {
            if (zzCurrentPosL < zzEndReadL) {
                zzInput = zzBufferL[zzCurrentPosL++];
            } else if (zzAtEOF) {
                zzInput = YYEOF;
                break;
            } else {
                // store back cached positions
                zzCurrentPos = zzCurrentPosL;
                zzMarkedPos = zzMarkedPosL;
                bool eof = zzRefill();
                // get translated positions and possibly new buffer
                zzCurrentPosL = zzCurrentPos;
                zzMarkedPosL = zzMarkedPos;
                zzBufferL = zzBuffer.get();
                zzEndReadL = zzEndRead;
                if (eof) {
                    zzInput = YYEOF;
                    break;
                } else {
                    zzInput = zzBufferL[zzCurrentPosL++];
                }
            }

            int32_t zzNext = zzTransL[zzRowMapL[zzState] + zzCMapL[zzInput]];
            if (zzNext == -1) {
                break;
            }
            zzState = zzNext;

            int32_t zzAttributes = zzAttrL[zzState];
            if ((zzAttributes & 1) == 1) {
                zzAction = zzState;
                zzMarkedPosL = zzCurrentPosL;
                if ((zzAttributes & 8) == 8) {
                    break;
                }
            }
        }

        // store back cached position
        zzMarkedPos = zzMarkedPosL;

        switch (zzAction < 0 ? zzAction : zzActionL[zzAction]) {
        case 4:
            return StandardTokenizer::HOST;
        case 11:
            break;
        case 9:
            return StandardTokenizer::ACRONYM;
        case 12:
            break;
        case 8:
            return StandardTokenizer::ACRONYM_DEP;
        case 13:
            break;
        case 1: // ignore
        case 14:
            break;
        case 5:
            return StandardTokenizer::NUM;
        case 15:
            break;
        case 3:
            return StandardTokenizer::CJ;
        case 16:
            break;
        case 2:
            return StandardTokenizer::ALPHANUM;
        case 17:
            break;
        case 7:
            return StandardTokenizer::COMPANY;
        case 18:
            break;
        case 6:
            return StandardTokenizer::APOSTROPHE;
        case 19:
            break;
        case 10:
            return StandardTokenizer::EMAIL;
        case 20:
            break;
        default:
            if (zzInput == YYEOF && zzStartRead == zzCurrentPos) {
                zzAtEOF = true;
                return YYEOF;
            } else {
                zzScanError(ZZ_NO_MATCH);
            }
        }
    }

    return YYINITIAL;
}

}
