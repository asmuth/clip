/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef STOPFILTER_H
#define STOPFILTER_H

#include "fnord-fts/analysis/TokenFilter.h"

namespace Lucene {

/// Removes stop words from a token stream.
class StopFilter : public TokenFilter {
public:
    /// Construct a token stream filtering the given input.  If stopWords is an instance of {@link CharArraySet}
    /// (true if makeStopSet() was used to construct the set) it will be directly used and ignoreCase will be
    /// ignored since CharArraySet directly controls case sensitivity.
    ///
    /// If stopWords is not an instance of {@link CharArraySet}, a new CharArraySet will be constructed and
    /// ignoreCase will be used to specify the case sensitivity of that set.
    ///
    /// @param enablePositionIncrements true if token positions should record the removed stop words
    /// @param input Input TokenStream
    /// @param stopWords A Set of Strings or char[] or any other toString()-able set representing the stopwords
    /// @param ignoreCase if true, all words are lower cased first
    StopFilter(bool enablePositionIncrements, const TokenStreamPtr& input, HashSet<String> stopWords, bool ignoreCase = false);
    StopFilter(bool enablePositionIncrements, const TokenStreamPtr& input, const CharArraySetPtr& stopWords, bool ignoreCase = false);

    virtual ~StopFilter();

    LUCENE_CLASS(StopFilter);

protected:
    CharArraySetPtr stopWords;
    bool enablePositionIncrements;

    TermAttributePtr termAtt;
    PositionIncrementAttributePtr posIncrAtt;

public:
    /// Builds a Set from an array of stop words, appropriate for passing into the StopFilter constructor.
    static HashSet<String> makeStopSet(Collection<String> stopWords);

    /// Returns the next input Token whose term() is not a stop word.
    virtual bool incrementToken();

    /// Returns version-dependent default for enablePositionIncrements.  Analyzers that embed StopFilter use this
    /// method when creating the StopFilter.  Prior to 2.9, this returns false.  On 2.9 or later, it returns true.
    static bool getEnablePositionIncrementsVersionDefault(LuceneVersion::Version matchVersion);

    /// @see #setEnablePositionIncrements(bool).
    bool getEnablePositionIncrements();

    /// If true, this StopFilter will preserve positions of the incoming tokens (ie, accumulate and set position
    /// increments of the removed stop tokens).  Generally, true is best as it does not lose information (positions
    /// of the original tokens) during indexing.
    ///
    /// When set, when a token is stopped (omitted), the position increment of the following token is incremented.
    ///
    /// NOTE: be sure to also set {@link QueryParser#setEnablePositionIncrements} if you use QueryParser to create queries.
    void setEnablePositionIncrements(bool enable);
};

}

#endif
