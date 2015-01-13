/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef _FIELDCACHE_H
#define _FIELDCACHE_H

#include "LuceneObject.h"

namespace Lucene {

/// @see FieldCache#DEFAULT_BYTE_PARSER()
class DefaultByteParser : public ByteParser {
public:
    virtual ~DefaultByteParser();
    LUCENE_CLASS(DefaultByteParser);

public:
    virtual uint8_t parseByte(const String& string);
    virtual String toString();
};

/// @see FieldCache#DEFAULT_INT_PARSER()
class DefaultIntParser : public IntParser {
public:
    virtual ~DefaultIntParser();
    LUCENE_CLASS(DefaultIntParser);

public:
    virtual int32_t parseInt(const String& string);
    virtual String toString();
};

/// @see FieldCache#NUMERIC_UTILS_INT_PARSER()
class NumericUtilsIntParser : public IntParser {
public:
    virtual ~NumericUtilsIntParser();
    LUCENE_CLASS(NumericUtilsIntParser);

public:
    virtual int32_t parseInt(const String& string);
    virtual String toString();
};

/// @see FieldCache#DEFAULT_LONG_PARSER()
class DefaultLongParser : public LongParser {
public:
    virtual ~DefaultLongParser();
    LUCENE_CLASS(DefaultLongParser);

public:
    virtual int64_t parseLong(const String& string);
    virtual String toString();
};

/// @see FieldCache#NUMERIC_UTILS_LONG_PARSER()
class NumericUtilsLongParser : public LongParser {
public:
    virtual ~NumericUtilsLongParser();
    LUCENE_CLASS(NumericUtilsLongParser);

public:
    virtual int64_t parseLong(const String& string);
    virtual String toString();
};

/// @see FieldCache#DEFAULT_DOUBLE_PARSER()
class DefaultDoubleParser : public DoubleParser {
public:
    virtual ~DefaultDoubleParser();
    LUCENE_CLASS(DefaultDoubleParser);

public:
    virtual double parseDouble(const String& string);
    virtual String toString();
};

/// @see FieldCache#NUMERIC_UTILS_DOUBLE_PARSER()
class NumericUtilsDoubleParser : public DoubleParser {
public:
    virtual ~NumericUtilsDoubleParser();
    LUCENE_CLASS(NumericUtilsDoubleParser);

public:
    virtual double parseDouble(const String& string);
    virtual String toString();
};

}

#endif
