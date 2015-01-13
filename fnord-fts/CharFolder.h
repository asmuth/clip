/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef CHARFOLDER_H
#define CHARFOLDER_H

#include "LuceneObject.h"

namespace Lucene {

/// Utility class for folding character case.
class CharFolder : public LuceneObject {
public:
    virtual ~CharFolder();
    LUCENE_CLASS(CharFolder);

protected:
    static bool lowerCache;
    static bool upperCache;
    static wchar_t lowerChars[CHAR_MAX - CHAR_MIN + 1];
    static wchar_t upperChars[CHAR_MAX - CHAR_MIN + 1];

public:
    static wchar_t toLower(wchar_t ch);
    static wchar_t toUpper(wchar_t ch);

    template <class ITER>
    static void toLower(ITER first, ITER last) {
        for (; first != last; ++first) {
            *first = toLower(*first);
        }
    }

    template <class ITER>
    static void toUpper(ITER first, ITER last) {
        for (; first != last; ++first) {
            *first = toUpper(*first);
        }
    }

protected:
    static bool fillLower();
    static bool fillUpper();
};

}

#endif
