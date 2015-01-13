/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef PORTERSTEMMER_H
#define PORTERSTEMMER_H

#include "LuceneObject.h"

namespace Lucene {

/// This is the Porter stemming algorithm, coded up as thread-safe ANSI C by the author.
///
/// It may be be regarded as canonical, in that it follows the algorithm presented in Porter, 1980, An algorithm
/// for suffix stripping, Program, Vol. 14, no. 3, pp 130-137, only differing from it at the points marked DEPARTURE.
///
/// See also http://www.tartarus.org/~martin/PorterStemmer
///
/// The algorithm as described in the paper could be exactly replicated by adjusting the points of DEPARTURE, but
/// this is barely necessary, because (a) the points of DEPARTURE are definitely improvements, and (b) no encoding
/// of the Porter stemmer I have seen is anything like as exact as this version, even with the points of DEPARTURE!
///
/// Release 2 (the more old-fashioned, non-thread-safe version may be regarded as release 1.)
class PorterStemmer : public LuceneObject {
public:
    PorterStemmer();
    virtual ~PorterStemmer();

    LUCENE_CLASS(PorterStemmer);

protected:
    wchar_t* b; // buffer for word to be stemmed
    int32_t k; // offset to the end of the string
    int32_t j; // a general offset into the string
    int32_t i; // initial length of word
    bool dirty;

public:
    bool stem(CharArray word);

    /// In stem(b, k), b is a char pointer, and the string to be stemmed is from b[0] to b[k] inclusive.
    /// Possibly b[k+1] == '\0', but it is not important. The stemmer adjusts the characters b[0] ... b[k] and
    /// stores the new end-point of the string, k'. Stemming never increases word length, so 0 <= k' <= k.
    bool stem(wchar_t* b, int32_t k);

    wchar_t* getResultBuffer();
    int32_t getResultLength();

protected:
    /// Returns true if b[i] is a consonant. ('b' means 'z->b', but here and below we drop 'z->' in comments.
    bool cons(int32_t i);

    /// Measures the number of consonant sequences between 0 and j.  If c is a consonant sequence and v a vowel
    /// sequence, and <..> indicates arbitrary presence,
    ///
    /// <c><v>       gives 0
    /// <c>vc<v>     gives 1
    /// <c>vcvc<v>   gives 2
    /// <c>vcvcvc<v> gives 3
    /// ...
    int32_t m();

    /// Return true if 0,...j contains a vowel
    bool vowelinstem();

    /// Return true if j,(j-1) contain a double consonant.
    bool doublec(int32_t j);

    /// Return true if i-2,i-1,i has the form consonant - vowel - consonant and also if the second c is not w,x or y.
    /// This is used when trying to restore an e at the end of a short word.
    ///
    /// eg. cav(e), lov(e), hop(e), crim(e), but
    /// snow, box, tray.
    bool cvc(int32_t i);

    /// Returns true if 0,...k ends with the string s.
    bool ends(const wchar_t* s);

    /// Sets (j+1),...k to the characters in the string s, readjusting k.
    void setto(const wchar_t* s);

    void r(const wchar_t* s);

    /// step1ab() gets rid of plurals and -ed or -ing. eg.
    ///
    /// caresses  ->  caress
    /// ponies    ->  poni
    /// ties      ->  ti
    /// caress    ->  caress
    /// cats      ->  cat
    ///
    /// feed      ->  feed
    /// agreed    ->  agree
    /// disabled  ->  disable
    ///
    /// matting   ->  mat
    /// mating    ->  mate
    /// meeting   ->  meet
    /// milling   ->  mill
    /// messing   ->  mess
    ///
    /// meetings  ->  meet
    void step1ab();

    /// Turns terminal y to i when there is another vowel in the stem.
    void step1c();

    /// Maps double suffices to single ones. so -ization ( = -ize plus -ation) maps to -ize etc. note that the
    /// string before the suffix must give m() > 0.
    void step2();

    /// Deals with -ic-, -full, -ness etc. similar strategy to step2.
    void step3();

    /// Takes off -ant, -ence etc., in context <c>vcvc<v>.
    void step4();

    /// Removes a final -e if m() > 1, and changes -ll to -l if m() > 1.
    void step5();
};

}

#endif
