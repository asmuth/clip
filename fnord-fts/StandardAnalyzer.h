/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef STANDARDANALYZER_H
#define STANDARDANALYZER_H

#include "Analyzer.h"

namespace Lucene {

/// Filters {@link StandardTokenizer} with {@link StandardFilter}, {@link LowerCaseFilter} and {@link StopFilter}, using
/// a list of English stop words.
///
/// You must specify the required {@link Version} compatibility when creating StandardAnalyzer:
///
/// <ul>
/// <li> As of 2.9, StopFilter preserves position increments
/// <li> As of 2.4, Tokens incorrectly identified as acronyms are corrected
/// </ul>
class StandardAnalyzer : public Analyzer {
public:
    /// Builds an analyzer with the default stop words ({@link #STOP_WORDS_SET}).
    /// @param matchVersion Lucene version to match.
    StandardAnalyzer(LuceneVersion::Version matchVersion);

    /// Builds an analyzer with the given stop words.
    /// @param matchVersion Lucene version to match.
    /// @param stopWords stop words
    StandardAnalyzer(LuceneVersion::Version matchVersion, HashSet<String> stopWords);

    /// Builds an analyzer with the stop words from the given file.
    /// @see WordlistLoader#getWordSet(const String&, const String&)
    /// @param matchVersion Lucene version to match.
    /// @param stopwords File to read stop words from.
    StandardAnalyzer(LuceneVersion::Version matchVersion, const String& stopwords);

    /// Builds an analyzer with the stop words from the given reader.
    /// @see WordlistLoader#getWordSet(ReaderPtr, const String&)
    /// @param matchVersion Lucene version to match.
    /// @param stopwords Reader to read stop words from.
    StandardAnalyzer(LuceneVersion::Version matchVersion, const ReaderPtr& stopwords);

    virtual ~StandardAnalyzer();

    LUCENE_CLASS(StandardAnalyzer);

public:
    /// Default maximum allowed token length
    static const int32_t DEFAULT_MAX_TOKEN_LENGTH;

protected:
    HashSet<String> stopSet;

    /// Specifies whether deprecated acronyms should be replaced with HOST type.
    bool replaceInvalidAcronym;
    bool enableStopPositionIncrements;

    LuceneVersion::Version matchVersion;

    int32_t maxTokenLength;

protected:
    /// Construct an analyzer with the given stop words.
    void ConstructAnalyser(LuceneVersion::Version matchVersion, HashSet<String> stopWords);

public:
    /// Constructs a {@link StandardTokenizer} filtered by a {@link StandardFilter}, a {@link LowerCaseFilter}
    /// and a {@link StopFilter}.
    virtual TokenStreamPtr tokenStream(const String& fieldName, const ReaderPtr& reader);

    /// Set maximum allowed token length.  If a token is seen that exceeds this length then it is discarded.  This setting
    /// only takes effect the next time tokenStream or reusableTokenStream is called.
    void setMaxTokenLength(int32_t length);

    /// @see #setMaxTokenLength
    int32_t getMaxTokenLength();

    virtual TokenStreamPtr reusableTokenStream(const String& fieldName, const ReaderPtr& reader);
};

}

#endif
