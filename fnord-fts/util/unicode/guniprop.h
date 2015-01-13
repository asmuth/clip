/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

/* Unicode character properties.
 *
 * Copyright (C) 1999 Tom Tromey
 * Copyright (C) 2000 Red Hat, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include "fnord-fts/Lucene.h"

typedef uint32_t gunichar;
typedef uint16_t gunichar2;
typedef uint32_t guint;
typedef uint8_t guchar;
typedef int32_t gint;
typedef char gchar;
typedef bool gboolean;
typedef size_t gsize;
typedef size_t gssize;

/* These are the possible character classifications.
 * See http://www.unicode.org/Public/UNIDATA/UCD.html#General_Category_Values
 */
typedef enum {
    G_UNICODE_CONTROL,
    G_UNICODE_FORMAT,
    G_UNICODE_UNASSIGNED,
    G_UNICODE_PRIVATE_USE,
    G_UNICODE_SURROGATE,
    G_UNICODE_LOWERCASE_LETTER,
    G_UNICODE_MODIFIER_LETTER,
    G_UNICODE_OTHER_LETTER,
    G_UNICODE_TITLECASE_LETTER,
    G_UNICODE_UPPERCASE_LETTER,
    G_UNICODE_COMBINING_MARK,
    G_UNICODE_ENCLOSING_MARK,
    G_UNICODE_NON_SPACING_MARK,
    G_UNICODE_DECIMAL_NUMBER,
    G_UNICODE_LETTER_NUMBER,
    G_UNICODE_OTHER_NUMBER,
    G_UNICODE_CONNECT_PUNCTUATION,
    G_UNICODE_DASH_PUNCTUATION,
    G_UNICODE_CLOSE_PUNCTUATION,
    G_UNICODE_FINAL_PUNCTUATION,
    G_UNICODE_INITIAL_PUNCTUATION,
    G_UNICODE_OTHER_PUNCTUATION,
    G_UNICODE_OPEN_PUNCTUATION,
    G_UNICODE_CURRENCY_SYMBOL,
    G_UNICODE_MODIFIER_SYMBOL,
    G_UNICODE_MATH_SYMBOL,
    G_UNICODE_OTHER_SYMBOL,
    G_UNICODE_LINE_SEPARATOR,
    G_UNICODE_PARAGRAPH_SEPARATOR,
    G_UNICODE_SPACE_SEPARATOR
} GUnicodeType;

/* These are the possible line break classifications.
 * Note that new types may be added in the future.
 * Implementations may regard unknown values like G_UNICODE_BREAK_UNKNOWN
 * See http://www.unicode.org/unicode/reports/tr14/
 */
typedef enum {
    G_UNICODE_BREAK_MANDATORY,
    G_UNICODE_BREAK_CARRIAGE_RETURN,
    G_UNICODE_BREAK_LINE_FEED,
    G_UNICODE_BREAK_COMBINING_MARK,
    G_UNICODE_BREAK_SURROGATE,
    G_UNICODE_BREAK_ZERO_WIDTH_SPACE,
    G_UNICODE_BREAK_INSEPARABLE,
    G_UNICODE_BREAK_NON_BREAKING_GLUE,
    G_UNICODE_BREAK_CONTINGENT,
    G_UNICODE_BREAK_SPACE,
    G_UNICODE_BREAK_AFTER,
    G_UNICODE_BREAK_BEFORE,
    G_UNICODE_BREAK_BEFORE_AND_AFTER,
    G_UNICODE_BREAK_HYPHEN,
    G_UNICODE_BREAK_NON_STARTER,
    G_UNICODE_BREAK_OPEN_PUNCTUATION,
    G_UNICODE_BREAK_CLOSE_PUNCTUATION,
    G_UNICODE_BREAK_QUOTATION,
    G_UNICODE_BREAK_EXCLAMATION,
    G_UNICODE_BREAK_IDEOGRAPHIC,
    G_UNICODE_BREAK_NUMERIC,
    G_UNICODE_BREAK_INFIX_SEPARATOR,
    G_UNICODE_BREAK_SYMBOL,
    G_UNICODE_BREAK_ALPHABETIC,
    G_UNICODE_BREAK_PREFIX,
    G_UNICODE_BREAK_POSTFIX,
    G_UNICODE_BREAK_COMPLEX_CONTEXT,
    G_UNICODE_BREAK_AMBIGUOUS,
    G_UNICODE_BREAK_UNKNOWN,
    G_UNICODE_BREAK_NEXT_LINE,
    G_UNICODE_BREAK_WORD_JOINER,
    G_UNICODE_BREAK_HANGUL_L_JAMO,
    G_UNICODE_BREAK_HANGUL_V_JAMO,
    G_UNICODE_BREAK_HANGUL_T_JAMO,
    G_UNICODE_BREAK_HANGUL_LV_SYLLABLE,
    G_UNICODE_BREAK_HANGUL_LVT_SYLLABLE
} GUnicodeBreakType;

typedef enum {
    /* ISO 15924 code */
    G_UNICODE_SCRIPT_INVALID_CODE = -1,
    G_UNICODE_SCRIPT_COMMON       = 0,   /* Zyyy */
    G_UNICODE_SCRIPT_INHERITED,          /* Qaai */
    G_UNICODE_SCRIPT_ARABIC,             /* Arab */
    G_UNICODE_SCRIPT_ARMENIAN,           /* Armn */
    G_UNICODE_SCRIPT_BENGALI,            /* Beng */
    G_UNICODE_SCRIPT_BOPOMOFO,           /* Bopo */
    G_UNICODE_SCRIPT_CHEROKEE,           /* Cher */
    G_UNICODE_SCRIPT_COPTIC,             /* Qaac */
    G_UNICODE_SCRIPT_CYRILLIC,           /* Cyrl (Cyrs) */
    G_UNICODE_SCRIPT_DESERET,            /* Dsrt */
    G_UNICODE_SCRIPT_DEVANAGARI,         /* Deva */
    G_UNICODE_SCRIPT_ETHIOPIC,           /* Ethi */
    G_UNICODE_SCRIPT_GEORGIAN,           /* Geor (Geon, Geoa) */
    G_UNICODE_SCRIPT_GOTHIC,             /* Goth */
    G_UNICODE_SCRIPT_GREEK,              /* Grek */
    G_UNICODE_SCRIPT_GUJARATI,           /* Gujr */
    G_UNICODE_SCRIPT_GURMUKHI,           /* Guru */
    G_UNICODE_SCRIPT_HAN,                /* Hani */
    G_UNICODE_SCRIPT_HANGUL,             /* Hang */
    G_UNICODE_SCRIPT_HEBREW,             /* Hebr */
    G_UNICODE_SCRIPT_HIRAGANA,           /* Hira */
    G_UNICODE_SCRIPT_KANNADA,            /* Knda */
    G_UNICODE_SCRIPT_KATAKANA,           /* Kana */
    G_UNICODE_SCRIPT_KHMER,              /* Khmr */
    G_UNICODE_SCRIPT_LAO,                /* Laoo */
    G_UNICODE_SCRIPT_LATIN,              /* Latn (Latf, Latg) */
    G_UNICODE_SCRIPT_MALAYALAM,          /* Mlym */
    G_UNICODE_SCRIPT_MONGOLIAN,          /* Mong */
    G_UNICODE_SCRIPT_MYANMAR,            /* Mymr */
    G_UNICODE_SCRIPT_OGHAM,              /* Ogam */
    G_UNICODE_SCRIPT_OLD_ITALIC,         /* Ital */
    G_UNICODE_SCRIPT_ORIYA,              /* Orya */
    G_UNICODE_SCRIPT_RUNIC,              /* Runr */
    G_UNICODE_SCRIPT_SINHALA,            /* Sinh */
    G_UNICODE_SCRIPT_SYRIAC,             /* Syrc (Syrj, Syrn, Syre) */
    G_UNICODE_SCRIPT_TAMIL,              /* Taml */
    G_UNICODE_SCRIPT_TELUGU,             /* Telu */
    G_UNICODE_SCRIPT_THAANA,             /* Thaa */
    G_UNICODE_SCRIPT_THAI,               /* Thai */
    G_UNICODE_SCRIPT_TIBETAN,            /* Tibt */
    G_UNICODE_SCRIPT_CANADIAN_ABORIGINAL, /* Cans */
    G_UNICODE_SCRIPT_YI,                 /* Yiii */
    G_UNICODE_SCRIPT_TAGALOG,            /* Tglg */
    G_UNICODE_SCRIPT_HANUNOO,            /* Hano */
    G_UNICODE_SCRIPT_BUHID,              /* Buhd */
    G_UNICODE_SCRIPT_TAGBANWA,           /* Tagb */

    /* Unicode-4.0 additions */
    G_UNICODE_SCRIPT_BRAILLE,            /* Brai */
    G_UNICODE_SCRIPT_CYPRIOT,            /* Cprt */
    G_UNICODE_SCRIPT_LIMBU,              /* Limb */
    G_UNICODE_SCRIPT_OSMANYA,            /* Osma */
    G_UNICODE_SCRIPT_SHAVIAN,            /* Shaw */
    G_UNICODE_SCRIPT_LINEAR_B,           /* Linb */
    G_UNICODE_SCRIPT_TAI_LE,             /* Tale */
    G_UNICODE_SCRIPT_UGARITIC,           /* Ugar */

    /* Unicode-4.1 additions */
    G_UNICODE_SCRIPT_NEW_TAI_LUE,        /* Talu */
    G_UNICODE_SCRIPT_BUGINESE,           /* Bugi */
    G_UNICODE_SCRIPT_GLAGOLITIC,         /* Glag */
    G_UNICODE_SCRIPT_TIFINAGH,           /* Tfng */
    G_UNICODE_SCRIPT_SYLOTI_NAGRI,       /* Sylo */
    G_UNICODE_SCRIPT_OLD_PERSIAN,        /* Xpeo */
    G_UNICODE_SCRIPT_KHAROSHTHI,         /* Khar */

    /* Unicode-5.0 additions */
    G_UNICODE_SCRIPT_UNKNOWN,            /* Zzzz */
    G_UNICODE_SCRIPT_BALINESE,           /* Bali */
    G_UNICODE_SCRIPT_CUNEIFORM,          /* Xsux */
    G_UNICODE_SCRIPT_PHOENICIAN,         /* Phnx */
    G_UNICODE_SCRIPT_PHAGS_PA,           /* Phag */
    G_UNICODE_SCRIPT_NKO,                /* Nkoo */

    /* Unicode-5.1 additions */
    G_UNICODE_SCRIPT_KAYAH_LI,           /* Kali */
    G_UNICODE_SCRIPT_LEPCHA,             /* Lepc */
    G_UNICODE_SCRIPT_REJANG,             /* Rjng */
    G_UNICODE_SCRIPT_SUNDANESE,          /* Sund */
    G_UNICODE_SCRIPT_SAURASHTRA,         /* Saur */
    G_UNICODE_SCRIPT_CHAM,               /* Cham */
    G_UNICODE_SCRIPT_OL_CHIKI,           /* Olck */
    G_UNICODE_SCRIPT_VAI,                /* Vaii */
    G_UNICODE_SCRIPT_CARIAN,             /* Cari */
    G_UNICODE_SCRIPT_LYCIAN,             /* Lyci */
    G_UNICODE_SCRIPT_LYDIAN              /* Lydi */
} GUnicodeScript;

#include "gunichartables.h"

gboolean g_unichar_isalnum (gunichar c);
gboolean g_unichar_isalpha (gunichar c);
gboolean g_unichar_iscntrl (gunichar c);
gboolean g_unichar_isdigit (gunichar c);
gboolean g_unichar_isgraph (gunichar c);
gboolean g_unichar_islower (gunichar c);
gboolean g_unichar_isprint (gunichar c);
gboolean g_unichar_ispunct (gunichar c);
gboolean g_unichar_isspace (gunichar c);
gboolean g_unichar_ismark (gunichar c);
gboolean g_unichar_isupper (gunichar c);
gboolean g_unichar_istitle (gunichar c);
gboolean g_unichar_isxdigit (gunichar c);
gboolean g_unichar_isdefined (gunichar c);
gunichar g_unichar_toupper (gunichar c);
gunichar g_unichar_tolower (gunichar c);
gunichar g_unichar_totitle (gunichar c);
int g_unichar_digit_value (gunichar c);
int g_unichar_xdigit_value (gunichar c);
GUnicodeType g_unichar_type (gunichar c);
