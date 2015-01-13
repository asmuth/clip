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

#include "fnord-fts/LuceneInc.h"
#include "fnord-fts/util/unicode/guniprop.h"

#define ATTR_TABLE(Page) (((Page) <= G_UNICODE_LAST_PAGE_PART1) \
                          ? attr_table_part1[Page] \
                          : attr_table_part2[(Page) - 0xe00])

#define ATTTABLE(Page, Char) \
  ((ATTR_TABLE(Page) == G_UNICODE_MAX_TABLE_INDEX) ? 0 : (attr_data[ATTR_TABLE(Page)][Char]))

#define TTYPE_PART1(Page, Char) \
  ((type_table_part1[Page] >= G_UNICODE_MAX_TABLE_INDEX) \
   ? (type_table_part1[Page] - G_UNICODE_MAX_TABLE_INDEX) \
   : (type_data[type_table_part1[Page]][Char]))

#define TTYPE_PART2(Page, Char) \
  ((type_table_part2[Page] >= G_UNICODE_MAX_TABLE_INDEX) \
   ? (type_table_part2[Page] - G_UNICODE_MAX_TABLE_INDEX) \
   : (type_data[type_table_part2[Page]][Char]))

#define TYPE(Char) \
  (((Char) <= G_UNICODE_LAST_CHAR_PART1) \
   ? TTYPE_PART1 ((Char) >> 8, (Char) & 0xff) \
   : (((Char) >= 0xe0000 && (Char) <= G_UNICODE_LAST_CHAR) \
      ? TTYPE_PART2 (((Char) - 0xe0000) >> 8, (Char) & 0xff) \
      : G_UNICODE_UNASSIGNED))


#define IS(Type, Class) (((guint)1 << (Type)) & (Class))
#define OR(Type, Rest)  (((guint)1 << (Type)) | (Rest))

/* Count the number of elements in an array. The array must be defined
 * as such; using this with a dynamically allocated array will give
 * incorrect results.
 */
#define G_N_ELEMENTS(arr) (sizeof (arr) / sizeof ((arr)[0]))

#define ISALPHA(Type)   IS ((Type),             \
                OR (G_UNICODE_LOWERCASE_LETTER, \
                OR (G_UNICODE_UPPERCASE_LETTER, \
                OR (G_UNICODE_TITLECASE_LETTER, \
                OR (G_UNICODE_MODIFIER_LETTER,  \
                OR (G_UNICODE_OTHER_LETTER,     0))))))

#define ISALDIGIT(Type) IS ((Type),             \
                OR (G_UNICODE_DECIMAL_NUMBER,   \
                OR (G_UNICODE_LETTER_NUMBER,    \
                OR (G_UNICODE_OTHER_NUMBER,     \
                OR (G_UNICODE_LOWERCASE_LETTER, \
                OR (G_UNICODE_UPPERCASE_LETTER, \
                OR (G_UNICODE_TITLECASE_LETTER, \
                OR (G_UNICODE_MODIFIER_LETTER,  \
                OR (G_UNICODE_OTHER_LETTER,     0)))))))))

#define ISMARK(Type)    IS ((Type),             \
                OR (G_UNICODE_NON_SPACING_MARK, \
                OR (G_UNICODE_COMBINING_MARK,   \
                OR (G_UNICODE_ENCLOSING_MARK,   0))))

#define ISZEROWIDTHTYPE(Type)   IS ((Type),     \
                OR (G_UNICODE_NON_SPACING_MARK, \
                OR (G_UNICODE_ENCLOSING_MARK,   \
                OR (G_UNICODE_FORMAT,       0))))

#define UTF8_COMPUTE(Char, Mask, Len)           \
  if (Char < 128)                               \
    {                                           \
      Len = 1;                                  \
      Mask = 0x7f;                              \
    }                                           \
  else if ((Char & 0xe0) == 0xc0)               \
    {                                           \
      Len = 2;                                  \
      Mask = 0x1f;                              \
    }                                           \
  else if ((Char & 0xf0) == 0xe0)               \
    {                                           \
      Len = 3;                                  \
      Mask = 0x0f;                              \
    }                                           \
  else if ((Char & 0xf8) == 0xf0)               \
    {                                           \
      Len = 4;                                  \
      Mask = 0x07;                              \
    }                                           \
  else if ((Char & 0xfc) == 0xf8)               \
    {                                           \
      Len = 5;                                  \
      Mask = 0x03;                              \
    }                                           \
  else if ((Char & 0xfe) == 0xfc)               \
    {                                           \
      Len = 6;                                  \
      Mask = 0x01;                              \
    }                                           \
  else                                          \
    Len = -1;

#define UTF8_GET(Result, Chars, Count, Mask, Len) \
  (Result) = (Chars)[0] & (Mask);               \
  for ((Count) = 1; (Count) < (Len); ++(Count)) \
    {                                           \
      if (((Chars)[(Count)] & 0xc0) != 0x80)    \
    {                                           \
      (Result) = -1;                            \
      break;                                    \
    }                                           \
      (Result) <<= 6;                           \
      (Result) |= ((Chars)[(Count)] & 0x3f);    \
    }

/**
 * g_utf8_get_char:
 * @p: a pointer to Unicode character encoded as UTF-8
 *
 * Converts a sequence of bytes encoded as UTF-8 to a Unicode character.
 * If @p does not point to a valid UTF-8 encoded character, results are
 * undefined. If you are not sure that the bytes are complete
 * valid Unicode characters, you should use g_utf8_get_char_validated()
 * instead.
 *
 * Return value: the resulting character
 **/
gunichar
g_utf8_get_char (const gchar* p) {
    int i, mask = 0, len;
    gunichar result;
    unsigned char c = (unsigned char) *p;

    UTF8_COMPUTE (c, mask, len);
    if (len == -1) {
        return (gunichar)-1;
    }
    UTF8_GET (result, p, i, mask, len);

    return result;
}

/**
 * g_unichar_isalnum:
 * @c: a Unicode character
 *
 * Determines whether a character is alphanumeric.
 * Given some UTF-8 text, obtain a character value
 * with g_utf8_get_char().
 *
 * Return value: %TRUE if @c is an alphanumeric character
 **/
gboolean
g_unichar_isalnum (gunichar c) {
    return ISALDIGIT (TYPE (c)) ? true : false;
}

/**
 * g_unichar_isalpha:
 * @c: a Unicode character
 *
 * Determines whether a character is alphabetic (i.e. a letter).
 * Given some UTF-8 text, obtain a character value with
 * g_utf8_get_char().
 *
 * Return value: %TRUE if @c is an alphabetic character
 **/
gboolean
g_unichar_isalpha (gunichar c) {
    return ISALPHA (TYPE (c)) ? true : false;
}


/**
 * g_unichar_iscntrl:
 * @c: a Unicode character
 *
 * Determines whether a character is a control character.
 * Given some UTF-8 text, obtain a character value with
 * g_utf8_get_char().
 *
 * Return value: %TRUE if @c is a control character
 **/
gboolean
g_unichar_iscntrl (gunichar c) {
    return TYPE (c) == G_UNICODE_CONTROL;
}

/**
 * g_unichar_isdigit:
 * @c: a Unicode character
 *
 * Determines whether a character is numeric (i.e. a digit).  This
 * covers ASCII 0-9 and also digits in other languages/scripts.  Given
 * some UTF-8 text, obtain a character value with g_utf8_get_char().
 *
 * Return value: %TRUE if @c is a digit
 **/
gboolean
g_unichar_isdigit (gunichar c) {
    return TYPE (c) == G_UNICODE_DECIMAL_NUMBER;
}


/**
 * g_unichar_isgraph:
 * @c: a Unicode character
 *
 * Determines whether a character is printable and not a space
 * (returns %FALSE for control characters, format characters, and
 * spaces). g_unichar_isprint() is similar, but returns %TRUE for
 * spaces. Given some UTF-8 text, obtain a character value with
 * g_utf8_get_char().
 *
 * Return value: %TRUE if @c is printable unless it's a space
 **/
gboolean
g_unichar_isgraph (gunichar c) {
    return !IS (TYPE(c),
                OR (G_UNICODE_CONTROL,
                    OR (G_UNICODE_FORMAT,
                        OR (G_UNICODE_UNASSIGNED,
                            OR (G_UNICODE_SURROGATE,
                                OR (G_UNICODE_SPACE_SEPARATOR,
                                    0))))));
}

/**
 * g_unichar_islower:
 * @c: a Unicode character
 *
 * Determines whether a character is a lowercase letter.
 * Given some UTF-8 text, obtain a character value with
 * g_utf8_get_char().
 *
 * Return value: %TRUE if @c is a lowercase letter
 **/
gboolean
g_unichar_islower (gunichar c) {
    return TYPE (c) == G_UNICODE_LOWERCASE_LETTER;
}


/**
 * g_unichar_isprint:
 * @c: a Unicode character
 *
 * Determines whether a character is printable.
 * Unlike g_unichar_isgraph(), returns %TRUE for spaces.
 * Given some UTF-8 text, obtain a character value with
 * g_utf8_get_char().
 *
 * Return value: %TRUE if @c is printable
 **/
gboolean
g_unichar_isprint (gunichar c) {
    return !IS (TYPE(c),
                OR (G_UNICODE_CONTROL,
                    OR (G_UNICODE_FORMAT,
                        OR (G_UNICODE_UNASSIGNED,
                            OR (G_UNICODE_SURROGATE,
                                0)))));
}

/**
 * g_unichar_ispunct:
 * @c: a Unicode character
 *
 * Determines whether a character is punctuation or a symbol.
 * Given some UTF-8 text, obtain a character value with
 * g_utf8_get_char().
 *
 * Return value: %TRUE if @c is a punctuation or symbol character
 **/
gboolean
g_unichar_ispunct (gunichar c) {
    return IS (TYPE(c),
               OR (G_UNICODE_CONNECT_PUNCTUATION,
                   OR (G_UNICODE_DASH_PUNCTUATION,
                       OR (G_UNICODE_CLOSE_PUNCTUATION,
                           OR (G_UNICODE_FINAL_PUNCTUATION,
                               OR (G_UNICODE_INITIAL_PUNCTUATION,
                                   OR (G_UNICODE_OTHER_PUNCTUATION,
                                       OR (G_UNICODE_OPEN_PUNCTUATION,
                                           OR (G_UNICODE_CURRENCY_SYMBOL,
                                                   OR (G_UNICODE_MODIFIER_SYMBOL,
                                                           OR (G_UNICODE_MATH_SYMBOL,
                                                                   OR (G_UNICODE_OTHER_SYMBOL,
                                                                           0)))))))))))) ? true : false;
}

/**
 * g_unichar_isspace:
 * @c: a Unicode character
 *
 * Determines whether a character is a space, tab, or line separator
 * (newline, carriage return, etc.).  Given some UTF-8 text, obtain a
 * character value with g_utf8_get_char().
 *
 * (Note: don't use this to do word breaking; you have to use
 * Pango or equivalent to get word breaking right, the algorithm
 * is fairly complex.)
 *
 * Return value: %TRUE if @c is a space character
 **/
gboolean
g_unichar_isspace (gunichar c) {
    switch (c) {
    /* special-case these since Unicode thinks they are not spaces */
    case '\t':
    case '\n':
    case '\r':
    case '\f':
        return true;
        break;

    default: {
        return IS (TYPE(c),
                   OR (G_UNICODE_SPACE_SEPARATOR,
                       OR (G_UNICODE_LINE_SEPARATOR,
                           OR (G_UNICODE_PARAGRAPH_SEPARATOR,
                               0)))) ? true : false;
    }
    break;
    }
}

/**
 * g_unichar_ismark:
 * @c: a Unicode character
 *
 * Determines whether a character is a mark (non-spacing mark,
 * combining mark, or enclosing mark in Unicode speak).
 * Given some UTF-8 text, obtain a character value
 * with g_utf8_get_char().
 *
 * Note: in most cases where isalpha characters are allowed,
 * ismark characters should be allowed to as they are essential
 * for writing most European languages as well as many non-Latin
 * scripts.
 *
 * Return value: %TRUE if @c is a mark character
 *
 * Since: 2.14
 **/
gboolean
g_unichar_ismark (gunichar c) {
    return ISMARK (TYPE (c)) ? true : false;
}

/**
 * g_unichar_isupper:
 * @c: a Unicode character
 *
 * Determines if a character is uppercase.
 *
 * Return value: %TRUE if @c is an uppercase character
 **/
gboolean
g_unichar_isupper (gunichar c) {
    return TYPE (c) == G_UNICODE_UPPERCASE_LETTER;
}

/**
 * g_unichar_istitle:
 * @c: a Unicode character
 *
 * Determines if a character is titlecase. Some characters in
 * Unicode which are composites, such as the DZ digraph
 * have three case variants instead of just two. The titlecase
 * form is used at the beginning of a word where only the
 * first letter is capitalized. The titlecase form of the DZ
 * digraph is U+01F2 LATIN CAPITAL LETTTER D WITH SMALL LETTER Z.
 *
 * Return value: %TRUE if the character is titlecase
 **/
gboolean
g_unichar_istitle (gunichar c) {
    unsigned int i;
    for (i = 0; i < G_N_ELEMENTS (title_table); ++i)
        if (title_table[i][0] == c) {
            return true;
        }
    return false;
}

/**
 * g_unichar_isxdigit:
 * @c: a Unicode character.
 *
 * Determines if a character is a hexidecimal digit.
 *
 * Return value: %TRUE if the character is a hexadecimal digit
 **/
gboolean
g_unichar_isxdigit (gunichar c) {
    return ((c >= 'a' && c <= 'f')
            || (c >= 'A' && c <= 'F')
            || (TYPE (c) == G_UNICODE_DECIMAL_NUMBER));
}

/**
 * g_unichar_isdefined:
 * @c: a Unicode character
 *
 * Determines if a given character is assigned in the Unicode
 * standard.
 *
 * Return value: %TRUE if the character has an assigned value
 **/
gboolean
g_unichar_isdefined (gunichar c) {
    return !IS (TYPE(c),
                OR (G_UNICODE_UNASSIGNED,
                    OR (G_UNICODE_SURROGATE,
                        0)));
}

/**
 * g_unichar_toupper:
 * @c: a Unicode character
 *
 * Converts a character to uppercase.
 *
 * Return value: the result of converting @c to uppercase.
 *               If @c is not an lowercase or titlecase character,
 *               or has no upper case equivalent @c is returned unchanged.
 **/
gunichar
g_unichar_toupper (gunichar c) {
    int t = TYPE (c);
    if (t == G_UNICODE_LOWERCASE_LETTER) {
        gunichar val = ATTTABLE (c >> 8, c & 0xff);
        if (val >= 0x1000000) {
            const gchar* p = special_case_table + val - 0x1000000;
            val = g_utf8_get_char (p);
        }
        /* Some lowercase letters, e.g., U+000AA, FEMININE ORDINAL INDICATOR,
         * do not have an uppercase equivalent, in which case val will be
         * zero.
         */
        return val ? val : c;
    } else if (t == G_UNICODE_TITLECASE_LETTER) {
        unsigned int i;
        for (i = 0; i < G_N_ELEMENTS (title_table); ++i) {
            if (title_table[i][0] == c) {
                return title_table[i][1];
            }
        }
    }
    return c;
}

/**
 * g_unichar_tolower:
 * @c: a Unicode character.
 *
 * Converts a character to lower case.
 *
 * Return value: the result of converting @c to lower case.
 *               If @c is not an upperlower or titlecase character,
 *               or has no lowercase equivalent @c is returned unchanged.
 **/
gunichar
g_unichar_tolower (gunichar c) {
    int t = TYPE (c);
    if (t == G_UNICODE_UPPERCASE_LETTER) {
        gunichar val = ATTTABLE (c >> 8, c & 0xff);
        if (val >= 0x1000000) {
            const gchar* p = special_case_table + val - 0x1000000;
            return g_utf8_get_char (p);
        } else {
            /* Not all uppercase letters are guaranteed to have a lowercase
             * equivalent.  If this is the case, val will be zero. */
            return val ? val : c;
        }
    } else if (t == G_UNICODE_TITLECASE_LETTER) {
        unsigned int i;
        for (i = 0; i < G_N_ELEMENTS (title_table); ++i) {
            if (title_table[i][0] == c) {
                return title_table[i][2];
            }
        }
    }
    return c;
}

/**
 * g_unichar_totitle:
 * @c: a Unicode character
 *
 * Converts a character to the titlecase.
 *
 * Return value: the result of converting @c to titlecase.
 *               If @c is not an uppercase or lowercase character,
 *               @c is returned unchanged.
 **/
gunichar
g_unichar_totitle (gunichar c) {
    unsigned int i;
    for (i = 0; i < G_N_ELEMENTS (title_table); ++i) {
        if (title_table[i][0] == c || title_table[i][1] == c
                || title_table[i][2] == c) {
            return title_table[i][0];
        }
    }

    if (TYPE (c) == G_UNICODE_LOWERCASE_LETTER) {
        return g_unichar_toupper (c);
    }

    return c;
}

/**
 * g_unichar_digit_value:
 * @c: a Unicode character
 *
 * Determines the numeric value of a character as a decimal
 * digit.
 *
 * Return value: If @c is a decimal digit (according to
 * g_unichar_isdigit()), its numeric value. Otherwise, -1.
 **/
int
g_unichar_digit_value (gunichar c) {
    if (TYPE (c) == G_UNICODE_DECIMAL_NUMBER) {
        return ATTTABLE (c >> 8, c & 0xff);
    }
    return -1;
}

/**
 * g_unichar_xdigit_value:
 * @c: a Unicode character
 *
 * Determines the numeric value of a character as a hexadecimal
 * digit.
 *
 * Return value: If @c is a hex digit (according to
 * g_unichar_isxdigit()), its numeric value. Otherwise, -1.
 **/
int
g_unichar_xdigit_value (gunichar c) {
    if (c >= 'A' && c <= 'F') {
        return c - 'A' + 10;
    }
    if (c >= 'a' && c <= 'f') {
        return c - 'a' + 10;
    }
    if (TYPE (c) == G_UNICODE_DECIMAL_NUMBER) {
        return ATTTABLE (c >> 8, c & 0xff);
    }
    return -1;
}

/**
 * g_unichar_type:
 * @c: a Unicode character
 *
 * Classifies a Unicode character by type.
 *
 * Return value: the type of the character.
 **/
GUnicodeType
g_unichar_type (gunichar c) {
    return (GUnicodeType)TYPE (c);
}
