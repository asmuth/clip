/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "StopAnalyzer.h"
#include "_StopAnalyzer.h"
#include "StopFilter.h"
#include "WordlistLoader.h"
#include "Reader.h"
#include "LowerCaseTokenizer.h"

namespace Lucene {

const wchar_t* StopAnalyzer::_ENGLISH_STOP_WORDS_SET[] = {
    L"a", L"an", L"and", L"are", L"as", L"at", L"be", L"but", L"by",
    L"for", L"if", L"in", L"into", L"is", L"it", L"no", L"not", L"of",
    L"on", L"or", L"such", L"that", L"the", L"their", L"then", L"there",
    L"these", L"they", L"this", L"to", L"was", L"will", L"with"
};

StopAnalyzer::StopAnalyzer(LuceneVersion::Version matchVersion) {
    stopWords = ENGLISH_STOP_WORDS_SET();
    enablePositionIncrements = StopFilter::getEnablePositionIncrementsVersionDefault(matchVersion);
}

StopAnalyzer::StopAnalyzer(LuceneVersion::Version matchVersion, HashSet<String> stopWords) {
    this->stopWords = stopWords;
    enablePositionIncrements = StopFilter::getEnablePositionIncrementsVersionDefault(matchVersion);
}

StopAnalyzer::StopAnalyzer(LuceneVersion::Version matchVersion, const String& stopwordsFile) {
    stopWords = WordlistLoader::getWordSet(stopwordsFile);
    enablePositionIncrements = StopFilter::getEnablePositionIncrementsVersionDefault(matchVersion);
}

StopAnalyzer::StopAnalyzer(LuceneVersion::Version matchVersion, const ReaderPtr& stopwords) {
    stopWords = WordlistLoader::getWordSet(stopwords);
    enablePositionIncrements = StopFilter::getEnablePositionIncrementsVersionDefault(matchVersion);
}

StopAnalyzer::~StopAnalyzer() {
}

const HashSet<String> StopAnalyzer::ENGLISH_STOP_WORDS_SET() {
    static HashSet<String> __ENGLISH_STOP_WORDS_SET;
    if (!__ENGLISH_STOP_WORDS_SET) {
        __ENGLISH_STOP_WORDS_SET = HashSet<String>::newInstance(_ENGLISH_STOP_WORDS_SET, _ENGLISH_STOP_WORDS_SET + SIZEOF_ARRAY(_ENGLISH_STOP_WORDS_SET));
    }
    return __ENGLISH_STOP_WORDS_SET;
}

TokenStreamPtr StopAnalyzer::tokenStream(const String& fieldName, const ReaderPtr& reader) {
    return newLucene<StopFilter>(enablePositionIncrements, newLucene<LowerCaseTokenizer>(reader), stopWords);
}

TokenStreamPtr StopAnalyzer::reusableTokenStream(const String& fieldName, const ReaderPtr& reader) {
    StopAnalyzerSavedStreamsPtr streams(boost::dynamic_pointer_cast<StopAnalyzerSavedStreams>(getPreviousTokenStream()));
    if (!streams) {
        streams = newLucene<StopAnalyzerSavedStreams>();
        streams->source = newLucene<LowerCaseTokenizer>(reader);
        streams->result = newLucene<StopFilter>(enablePositionIncrements, streams->source, stopWords);
        setPreviousTokenStream(streams);
    } else {
        streams->source->reset(reader);
    }
    return streams->result;
}

StopAnalyzerSavedStreams::~StopAnalyzerSavedStreams() {
}

}
