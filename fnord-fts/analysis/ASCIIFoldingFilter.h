/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef ASCIIFOLDINGFILTER_H
#define ASCIIFOLDINGFILTER_H

#include "fnord-fts/analysis/TokenFilter.h"

namespace Lucene {

/// This class converts alphabetic, numeric, and symbolic Unicode characters which are not in the first 127 ASCII
/// characters (the "Basic Latin" Unicode block) into their ASCII equivalents, if one exists.
///
/// Characters from the following Unicode blocks are converted; however, only those characters with reasonable ASCII
/// alternatives are converted:
///
///    C1 Controls and Latin-1 Supplement: <a href="http://www.unicode.org/charts/PDF/U0080.pdf">http://www.unicode.org/charts/PDF/U0080.pdf</a>
///    Latin Extended-A: <a href="http://www.unicode.org/charts/PDF/U0100.pdf">http://www.unicode.org/charts/PDF/U0100.pdf</a>
///    Latin Extended-B: <a href="http://www.unicode.org/charts/PDF/U0180.pdf">http://www.unicode.org/charts/PDF/U0180.pdf</a>
///    Latin Extended Additional: <a href="http://www.unicode.org/charts/PDF/U1E00.pdf">http://www.unicode.org/charts/PDF/U1E00.pdf</a>
///    Latin Extended-C: <a href="http://www.unicode.org/charts/PDF/U2C60.pdf">http://www.unicode.org/charts/PDF/U2C60.pdf</a>
///    Latin Extended-D: <a href="http://www.unicode.org/charts/PDF/UA720.pdf">http://www.unicode.org/charts/PDF/UA720.pdf</a>
///    IPA Extensions: <a href="http://www.unicode.org/charts/PDF/U0250.pdf">http://www.unicode.org/charts/PDF/U0250.pdf</a>
///    Phonetic Extensions: <a href="http://www.unicode.org/charts/PDF/U1D00.pdf">http://www.unicode.org/charts/PDF/U1D00.pdf</a>
///    Phonetic Extensions Supplement: <a href="http://www.unicode.org/charts/PDF/U1D80.pdf">http://www.unicode.org/charts/PDF/U1D80.pdf</a>
///    General Punctuation: <a href="http://www.unicode.org/charts/PDF/U2000.pdf">http://www.unicode.org/charts/PDF/U2000.pdf</a>
///    Superscripts and Subscripts: <a href="http://www.unicode.org/charts/PDF/U2070.pdf">http://www.unicode.org/charts/PDF/U2070.pdf</a>
///    Enclosed Alphanumerics: <a href="http://www.unicode.org/charts/PDF/U2460.pdf">http://www.unicode.org/charts/PDF/U2460.pdf</a>
///    Dingbats: <a href="http://www.unicode.org/charts/PDF/U2700.pdf">http://www.unicode.org/charts/PDF/U2700.pdf</a>
///    Supplemental Punctuation: <a href="http://www.unicode.org/charts/PDF/U2E00.pdf">http://www.unicode.org/charts/PDF/U2E00.pdf</a>
///    Alphabetic Presentation Forms: <a href="http://www.unicode.org/charts/PDF/UFB00.pdf">http://www.unicode.org/charts/PDF/UFB00.pdf</a>
///    Halfwidth and Fullwidth Forms: <a href="http://www.unicode.org/charts/PDF/UFF00.pdf">http://www.unicode.org/charts/PDF/UFF00.pdf</a>
///
/// See: <a href="http://en.wikipedia.org/wiki/Latin_characters_in_Unicode">http://en.wikipedia.org/wiki/Latin_characters_in_Unicode</a>
///
/// The set of character conversions supported by this class is a superset of those supported by Lucene's {@link
/// ISOLatin1AccentFilter} which strips accents from Latin1 characters.  For example, '&agrave;' will be replaced by 'a'.
///
class ASCIIFoldingFilter : public TokenFilter {
public:
    ASCIIFoldingFilter(const TokenStreamPtr& input);
    virtual ~ASCIIFoldingFilter();

    LUCENE_CLASS(ASCIIFoldingFilter);

protected:
    CharArray output;
    int32_t outputPos;
    TermAttributePtr termAtt;

public:
    virtual bool incrementToken();

    /// Converts characters above ASCII to their ASCII equivalents.  For example, accents are removed from
    /// accented characters.
    /// @param input The string to fold
    /// @param length The number of characters in the input string
    void foldToASCII(const wchar_t* input, int32_t length);
};

}

#endif
