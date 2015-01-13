/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "StandardAnalyzer.h"
#include "_StandardAnalyzer.h"
#include "StandardTokenizer.h"
#include "StandardFilter.h"
#include "LowerCaseFilter.h"
#include "StopAnalyzer.h"
#include "StopFilter.h"
#include "WordlistLoader.h"

namespace Lucene {

/// Construct an analyzer with the given stop words.
const int32_t StandardAnalyzer::DEFAULT_MAX_TOKEN_LENGTH = 255;

StandardAnalyzer::StandardAnalyzer(LuceneVersion::Version matchVersion) {
    ConstructAnalyser(matchVersion, StopAnalyzer::ENGLISH_STOP_WORDS_SET());
}

StandardAnalyzer::StandardAnalyzer(LuceneVersion::Version matchVersion, HashSet<String> stopWords) {
    ConstructAnalyser(matchVersion, stopWords);
}

StandardAnalyzer::StandardAnalyzer(LuceneVersion::Version matchVersion, const String& stopwords) {
    ConstructAnalyser(matchVersion, WordlistLoader::getWordSet(stopwords));
}

StandardAnalyzer::StandardAnalyzer(LuceneVersion::Version matchVersion, const ReaderPtr& stopwords) {
    ConstructAnalyser(matchVersion, WordlistLoader::getWordSet(stopwords));
}

StandardAnalyzer::~StandardAnalyzer() {
}

void StandardAnalyzer::ConstructAnalyser(LuceneVersion::Version matchVersion, HashSet<String> stopWords) {
    stopSet = stopWords;
    enableStopPositionIncrements = StopFilter::getEnablePositionIncrementsVersionDefault(matchVersion);
    replaceInvalidAcronym = LuceneVersion::onOrAfter(matchVersion, LuceneVersion::LUCENE_24);
    this->matchVersion = matchVersion;
    this->maxTokenLength = DEFAULT_MAX_TOKEN_LENGTH;
}

TokenStreamPtr StandardAnalyzer::tokenStream(const String& fieldName, const ReaderPtr& reader) {
    StandardTokenizerPtr tokenStream(newLucene<StandardTokenizer>(matchVersion, reader));
    tokenStream->setMaxTokenLength(maxTokenLength);
    TokenStreamPtr result(newLucene<StandardFilter>(tokenStream));
    result = newLucene<LowerCaseFilter>(result);
    result = newLucene<StopFilter>(enableStopPositionIncrements, result, stopSet);
    return result;
}

void StandardAnalyzer::setMaxTokenLength(int32_t length) {
    maxTokenLength = length;
}

int32_t StandardAnalyzer::getMaxTokenLength() {
    return maxTokenLength;
}

TokenStreamPtr StandardAnalyzer::reusableTokenStream(const String& fieldName, const ReaderPtr& reader) {
    StandardAnalyzerSavedStreamsPtr streams = boost::dynamic_pointer_cast<StandardAnalyzerSavedStreams>(getPreviousTokenStream());
    if (!streams) {
        streams = newLucene<StandardAnalyzerSavedStreams>();
        setPreviousTokenStream(streams);
        streams->tokenStream = newLucene<StandardTokenizer>(matchVersion, reader);
        streams->filteredTokenStream = newLucene<StandardFilter>(streams->tokenStream);
        streams->filteredTokenStream = newLucene<LowerCaseFilter>(streams->filteredTokenStream);
        streams->filteredTokenStream = newLucene<StopFilter>(enableStopPositionIncrements, streams->filteredTokenStream, stopSet);
    } else {
        streams->tokenStream->reset(reader);
    }
    streams->tokenStream->setMaxTokenLength(maxTokenLength);

    streams->tokenStream->setReplaceInvalidAcronym(replaceInvalidAcronym);

    return streams->filteredTokenStream;
}

StandardAnalyzerSavedStreams::~StandardAnalyzerSavedStreams() {
}

}
