/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef UTF8STREAM_H
#define UTF8STREAM_H

#include "LuceneObject.h"

namespace Lucene {

class UTF8Base : public LuceneObject {
public:
    virtual ~UTF8Base();
    LUCENE_CLASS(UTF8Base);

public:
    static const uint16_t LEAD_SURROGATE_MIN;
    static const uint16_t LEAD_SURROGATE_MAX;
    static const uint16_t TRAIL_SURROGATE_MIN;
    static const uint16_t TRAIL_SURROGATE_MAX;
    static const uint16_t LEAD_OFFSET;
    static const uint32_t SURROGATE_OFFSET;
    static const uint32_t CODE_POINT_MAX;

    static const wchar_t UNICODE_REPLACEMENT_CHAR;
    static const wchar_t UNICODE_TERMINATOR;

protected:
    virtual uint32_t readNext() = 0;

    uint8_t mask8(uint32_t b);
    uint16_t mask16(uint32_t c);
    bool isTrail(uint32_t b);
    bool isSurrogate(uint32_t cp);
    bool isLeadSurrogate(uint32_t cp);
    bool isTrailSurrogate(uint32_t cp);
    bool isValidCodePoint(uint32_t cp);
    bool isOverlongSequence(uint32_t cp, int32_t length);
};

class UTF8Encoder : public UTF8Base {
public:
    UTF8Encoder(const wchar_t* unicodeBegin, const wchar_t* unicodeEnd);
    virtual ~UTF8Encoder();

    LUCENE_CLASS(UTF8Encoder);

protected:
    const wchar_t* unicodeBegin;
    const wchar_t* unicodeEnd;

public:
    int32_t encode(uint8_t* utf8, int32_t length);

    int32_t utf16to8(uint8_t* utf8, int32_t length);
    int32_t utf32to8(uint8_t* utf8, int32_t length);

protected:
    virtual uint32_t readNext();

    uint8_t* appendChar(uint8_t* utf8, uint32_t cp);
};

class UTF8EncoderStream : public UTF8Encoder {
public:
    UTF8EncoderStream(const ReaderPtr& reader);
    virtual ~UTF8EncoderStream();

    LUCENE_CLASS(UTF8EncoderStream);

protected:
    ReaderPtr reader;

protected:
    virtual uint32_t readNext();
};

class UTF8Decoder : public UTF8Base {
public:
    UTF8Decoder(const uint8_t* utf8Begin, const uint8_t* utf8End);
    virtual ~UTF8Decoder();

    LUCENE_CLASS(UTF8Decoder);

protected:
    const uint8_t* utf8Begin;
    const uint8_t* utf8End;

public:
    int32_t decode(wchar_t* unicode, int32_t length);

    int32_t utf8to16(wchar_t* unicode, int32_t length);
    int32_t utf8to32(wchar_t* unicode, int32_t length);

protected:
    virtual uint32_t readNext();

    int32_t sequenceLength(uint32_t cp);
    bool getSequence(uint32_t& cp, int32_t length);
    bool isValidNext(uint32_t& cp);
};

class UTF8DecoderStream : public UTF8Decoder {
public:
    UTF8DecoderStream(const ReaderPtr& reader);
    virtual ~UTF8DecoderStream();

    LUCENE_CLASS(UTF8DecoderStream);

protected:
    ReaderPtr reader;

protected:
    virtual uint32_t readNext();
};

class UTF16Decoder : public UTF8Base {
public:
    UTF16Decoder(const uint16_t* utf16Begin, const uint16_t* utf16End);
    virtual ~UTF16Decoder();

    LUCENE_CLASS(UTF16Decoder);

protected:
    const uint16_t* utf16Begin;
    const uint16_t* utf16End;

public:
    int32_t decode(wchar_t* unicode, int32_t length);

    int32_t utf16to16(wchar_t* unicode, int32_t length);
    int32_t utf16to32(wchar_t* unicode, int32_t length);

protected:
    virtual uint32_t readNext();
};

}

#endif
