/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "UTF8Stream.h"
#include "Reader.h"

namespace Lucene {

const uint16_t UTF8Base::LEAD_SURROGATE_MIN = 0xd800u;
const uint16_t UTF8Base::LEAD_SURROGATE_MAX = 0xdbffu;
const uint16_t UTF8Base::TRAIL_SURROGATE_MIN = 0xdc00u;
const uint16_t UTF8Base::TRAIL_SURROGATE_MAX = 0xdfffu;
const uint16_t UTF8Base::LEAD_OFFSET = LEAD_SURROGATE_MIN - (0x10000 >> 10);
const uint32_t UTF8Base::SURROGATE_OFFSET = 0x10000u - (LEAD_SURROGATE_MIN << 10) - TRAIL_SURROGATE_MIN;

// Maximum valid value for a Unicode code point
const uint32_t UTF8Base::CODE_POINT_MAX = 0x0010ffffu;

#ifdef LPP_UNICODE_CHAR_SIZE_2
const wchar_t UTF8Base::UNICODE_REPLACEMENT_CHAR = (wchar_t)0xfffd;
const wchar_t UTF8Base::UNICODE_TERMINATOR = (wchar_t)0xffff;
#else
const wchar_t UTF8Base::UNICODE_REPLACEMENT_CHAR = (wchar_t)0x0001fffd;
const wchar_t UTF8Base::UNICODE_TERMINATOR = (wchar_t)0x0001ffff;
#endif

UTF8Base::~UTF8Base() {
}

inline uint8_t UTF8Base::mask8(uint32_t b) {
    return static_cast<uint8_t>(0xff & b);
}

inline uint16_t UTF8Base::mask16(uint32_t c) {
    return static_cast<uint16_t>(0xffff & c);
}

inline bool UTF8Base::isTrail(uint32_t b) {
    return ((mask8(b) >> 6) == 0x2);
}

inline bool UTF8Base::isSurrogate(uint32_t cp) {
    return (cp >= LEAD_SURROGATE_MIN && cp <= TRAIL_SURROGATE_MAX);
}

inline bool UTF8Base::isLeadSurrogate(uint32_t cp) {
    return (cp >= LEAD_SURROGATE_MIN && cp <= LEAD_SURROGATE_MAX);
}

inline bool UTF8Base::isTrailSurrogate(uint32_t cp) {
    return (cp >= TRAIL_SURROGATE_MIN && cp <= TRAIL_SURROGATE_MAX);
}

inline bool UTF8Base::isValidCodePoint(uint32_t cp) {
    return (cp <= CODE_POINT_MAX && !isSurrogate(cp) && cp != 0xfffe && cp != 0xffff);
}

inline bool UTF8Base::isOverlongSequence(uint32_t cp, int32_t length) {
    if (cp < 0x80) {
        if (length != 1) {
            return true;
        }
    } else if (cp < 0x800) {
        if (length != 2) {
            return true;
        }
    } else if (cp < 0x10000) {
        if (length != 3) {
            return true;
        }
    }
    return false;
}

UTF8Encoder::UTF8Encoder(const wchar_t* unicodeBegin, const wchar_t* unicodeEnd) {
    this->unicodeBegin = unicodeBegin;
    this->unicodeEnd = unicodeEnd;
}

UTF8Encoder::~UTF8Encoder() {
}

uint32_t UTF8Encoder::readNext() {
    return unicodeBegin == unicodeEnd ? (uint32_t)UNICODE_TERMINATOR : (uint32_t)*unicodeBegin++;
}

inline uint8_t* UTF8Encoder::appendChar(uint8_t* utf8, uint32_t cp) {
    if (cp < 0x80) { // one octet
        *(utf8++) = static_cast<uint8_t>(cp);
    } else if (cp < 0x800) { // two octets
        *(utf8++) = static_cast<uint8_t>((cp >> 6) | 0xc0);
        *(utf8++) = static_cast<uint8_t>((cp & 0x3f) | 0x80);
    } else if (cp < 0x10000) { // three octets
        *(utf8++) = static_cast<uint8_t>((cp >> 12) | 0xe0);
        *(utf8++) = static_cast<uint8_t>(((cp >> 6) & 0x3f) | 0x80);
        *(utf8++) = static_cast<uint8_t>((cp & 0x3f) | 0x80);
    } else { // four octets
        *(utf8++) = static_cast<uint8_t>((cp >> 18) | 0xf0);
        *(utf8++) = static_cast<uint8_t>(((cp >> 12) & 0x3f) | 0x80);
        *(utf8++) = static_cast<uint8_t>(((cp >> 6) & 0x3f) | 0x80);
        *(utf8++) = static_cast<uint8_t>((cp & 0x3f) | 0x80);
    }
    return utf8;
}

int32_t UTF8Encoder::utf16to8(uint8_t* utf8, int32_t length) {
    uint8_t* start = utf8;
    uint32_t next = readNext();

    while (next != UNICODE_TERMINATOR) {
        uint32_t cp = mask16(next);
        if (isLeadSurrogate(cp)) {
            next = readNext();
            if (next == UNICODE_TERMINATOR) {
                return 0;
            }
            uint32_t trail_surrogate = mask16(next);
            if (!isTrailSurrogate(trail_surrogate)) {
                return 0;
            }
            cp = (cp << 10) + trail_surrogate + SURROGATE_OFFSET;
        } else if (isTrailSurrogate(cp)) {
            return 0;
        }
        if (!isValidCodePoint(cp)) {
            return 0;
        }
        utf8 = appendChar(utf8, cp);
        if ((utf8 - start) >= length) {
            break;
        }
        next = readNext();
    }

    return ((utf8 - start) == 0 && next == UNICODE_TERMINATOR) ? Reader::READER_EOF : (utf8 - start);
}

int32_t UTF8Encoder::utf32to8(uint8_t* utf8, int32_t length) {
    uint8_t* start = utf8;
    uint32_t next = readNext();

    while (next != UNICODE_TERMINATOR) {
        if (!isValidCodePoint(next)) {
            return 0;
        }
        utf8 = appendChar(utf8, next);
        if ((utf8 - start) >= length) {
            break;
        }
        next = readNext();
    }

    return ((utf8 - start) == 0 && next == UNICODE_TERMINATOR) ? Reader::READER_EOF : (utf8 - start);
}

int32_t UTF8Encoder::encode(uint8_t* utf8, int32_t length) {
#ifdef LPP_UNICODE_CHAR_SIZE_2
    return utf16to8(utf8, length);
#else
    return utf32to8(utf8, length);
#endif
}

UTF8EncoderStream::UTF8EncoderStream(const ReaderPtr& reader) : UTF8Encoder(NULL, NULL) {
    this->reader = reader;
}

UTF8EncoderStream::~UTF8EncoderStream() {
}

uint32_t UTF8EncoderStream::readNext() {
    int32_t next = reader->read();
    return next == Reader::READER_EOF ? UNICODE_TERMINATOR : (uint32_t)next;
}

UTF8Decoder::UTF8Decoder(const uint8_t* utf8Begin, const uint8_t* utf8End) {
    this->utf8Begin = utf8Begin;
    this->utf8End = utf8End;
}

UTF8Decoder::~UTF8Decoder() {
}

uint32_t UTF8Decoder::readNext() {
    return utf8Begin == utf8End ? (uint32_t)UNICODE_TERMINATOR : (uint32_t)*utf8Begin++;
}

inline int32_t UTF8Decoder::sequenceLength(uint32_t cp) {
    uint8_t lead = mask8(cp);
    if (lead < 0x80) {
        return 1;
    } else if ((lead >> 5) == 0x6) {
        return 2;
    } else if ((lead >> 4) == 0xe) {
        return 3;
    } else if ((lead >> 3) == 0x1e) {
        return 4;
    }
    return 0;
}

inline bool UTF8Decoder::getSequence(uint32_t& cp, int32_t length) {
    cp = mask8(cp);
    if (length == 1) {
        return true;
    }
    uint32_t next = readNext();
    if (next == UNICODE_TERMINATOR) {
        return false;
    }
    if (!isTrail(next)) {
        return false;
    }
    if (length == 2) {
        cp = ((cp << 6) & 0x7ff) + (next & 0x3f);
        return true;
    }
    if (length == 3) {
        cp = ((cp << 12) & 0xffff) + ((mask8(next) << 6) & 0xfff);
    } else {
        cp = ((cp << 18) & 0x1fffff) + ((mask8(next) << 12) & 0x3ffff);
    }
    next = readNext();
    if (next == UNICODE_TERMINATOR) {
        return false;
    }
    if (!isTrail(next)) {
        return false;
    }
    if (length == 3) {
        cp += next & 0x3f;
        return true;
    }
    cp += (mask8(next) << 6) & 0xfff;
    next = readNext();
    if (next == UNICODE_TERMINATOR) {
        return false;
    }
    if (!isTrail(next)) {
        return false;
    }
    cp += next & 0x3f;
    return true;
}

inline bool UTF8Decoder::isValidNext(uint32_t& cp) {
    // Determine the sequence length based on the lead octet
    int32_t length = sequenceLength(cp);
    if (length < 1 || length > 4) {
        return false;
    }

    // Now that we have a valid sequence length, get trail octets and calculate the code point
    if (!getSequence(cp, length)) {
        return false;
    }

    // Decoding succeeded, now security checks
    return (isValidCodePoint(cp) && !isOverlongSequence(cp, length));
}

int32_t UTF8Decoder::utf8to16(wchar_t* unicode, int32_t length) {
    int32_t position = 0;
    uint32_t next = readNext();

    while (next != UNICODE_TERMINATOR) {
        if (!isValidNext(next)) {
            return 0;
        }
        if (next > 0xffff) { // make a surrogate pair
            unicode[position++] = static_cast<uint16_t>((next >> 10) + LEAD_OFFSET);
            unicode[position++] = static_cast<uint16_t>((next & 0x3ff) + TRAIL_SURROGATE_MIN);
        } else {
            unicode[position++] = static_cast<uint16_t>(next);
        }
        if (position >= length) {
            break;
        }
        next = readNext();
    }

    return (position == 0 && next == UNICODE_TERMINATOR) ? Reader::READER_EOF : position;
}

int32_t UTF8Decoder::utf8to32(wchar_t* unicode, int32_t length) {
    int32_t position = 0;
    uint32_t next = readNext();

    while (next != UNICODE_TERMINATOR) {
        if (!isValidNext(next)) {
            return 0;
        }
        unicode[position++] = static_cast<wchar_t>(next);
        if (position >= length) {
            break;
        }
        next = readNext();
    }

    return (position == 0 && next == UNICODE_TERMINATOR) ? Reader::READER_EOF : position;
}

int32_t UTF8Decoder::decode(wchar_t* unicode, int32_t length) {
#ifdef LPP_UNICODE_CHAR_SIZE_2
    return utf8to16(unicode, length);
#else
    return utf8to32(unicode, length);
#endif
}

UTF8DecoderStream::UTF8DecoderStream(const ReaderPtr& reader)  : UTF8Decoder(NULL, NULL) {
    this->reader = reader;
}

UTF8DecoderStream::~UTF8DecoderStream() {
}

uint32_t UTF8DecoderStream::readNext() {
    int32_t next = reader->read();
    return next == Reader::READER_EOF ? UNICODE_TERMINATOR : (uint32_t)next;
}

UTF16Decoder::UTF16Decoder(const uint16_t* utf16Begin, const uint16_t* utf16End) {
    this->utf16Begin = utf16Begin;
    this->utf16End = utf16End;
}

UTF16Decoder::~UTF16Decoder() {
}

uint32_t UTF16Decoder::readNext() {
    return utf16Begin == utf16End ? (uint32_t)UNICODE_TERMINATOR : (uint32_t)*utf16Begin++;
}

int32_t UTF16Decoder::utf16to32(wchar_t* unicode, int32_t length) {
    int32_t position = 0;
    uint32_t next = readNext();

    while (next != UNICODE_TERMINATOR) {
        uint32_t cp = mask16(next);
        if (isLeadSurrogate(cp)) {
            next = readNext();
            if (next == UNICODE_TERMINATOR) {
                return 0;
            }
            uint32_t trail_surrogate = mask16(next);
            if (!isTrailSurrogate(trail_surrogate)) {
                return 0;
            }
            unicode[position++] = static_cast<wchar_t>(((cp - LEAD_SURROGATE_MIN) << 10) + (trail_surrogate - TRAIL_SURROGATE_MIN) + 0x0010000);
        } else if (isTrailSurrogate(cp)) {
            return 0;
        } else {
            unicode[position++] = static_cast<wchar_t>(cp);
        }
        if (position >= length) {
            break;
        }
        next = readNext();
    }

    return (position == 0 && next == UNICODE_TERMINATOR) ? Reader::READER_EOF : position;
}

int32_t UTF16Decoder::utf16to16(wchar_t* unicode, int32_t length) {
    int32_t position = 0;
    uint32_t next = readNext();

    while (next != UNICODE_TERMINATOR) {
        unicode[position++] = static_cast<wchar_t>(next);
        if (position >= length) {
            break;
        }
        next = readNext();
    }

    return (position == 0 && next == UNICODE_TERMINATOR) ? Reader::READER_EOF : position;
}

int32_t UTF16Decoder::decode(wchar_t* unicode, int32_t length) {
#ifdef LPP_UNICODE_CHAR_SIZE_2
    return utf16to16(unicode, length);
#else
    return utf16to32(unicode, length);
#endif
}

}

