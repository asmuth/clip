/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "StopFilter.h"
#include "CharArraySet.h"
#include "TermAttribute.h"
#include "PositionIncrementAttribute.h"

namespace Lucene {

StopFilter::StopFilter(bool enablePositionIncrements, const TokenStreamPtr& input, HashSet<String> stopWords, bool ignoreCase) : TokenFilter(input) {
    this->stopWords = newLucene<CharArraySet>(stopWords, ignoreCase);
    this->enablePositionIncrements = enablePositionIncrements;
    termAtt = addAttribute<TermAttribute>();
    posIncrAtt = addAttribute<PositionIncrementAttribute>();
}

StopFilter::StopFilter(bool enablePositionIncrements, const TokenStreamPtr& input, const CharArraySetPtr& stopWords, bool ignoreCase) : TokenFilter(input) {
    this->stopWords = stopWords;
    this->enablePositionIncrements = enablePositionIncrements;
    termAtt = addAttribute<TermAttribute>();
    posIncrAtt = addAttribute<PositionIncrementAttribute>();
}

StopFilter::~StopFilter() {
}

HashSet<String> StopFilter::makeStopSet(Collection<String> stopWords) {
    return HashSet<String>::newInstance(stopWords.begin(), stopWords.end());
}

bool StopFilter::incrementToken() {
    // return the first non-stop word found
    int32_t skippedPositions = 0;
    while (input->incrementToken()) {
        if (!stopWords->contains(termAtt->termBufferArray(), 0, termAtt->termLength())) {
            if (enablePositionIncrements) {
                posIncrAtt->setPositionIncrement(posIncrAtt->getPositionIncrement() + skippedPositions);
            }
            return true;
        }
        skippedPositions += posIncrAtt->getPositionIncrement();
    }
    // reached EOS -- return false
    return false;
}

bool StopFilter::getEnablePositionIncrementsVersionDefault(LuceneVersion::Version matchVersion) {
    return LuceneVersion::onOrAfter(matchVersion, LuceneVersion::LUCENE_29);
}

bool StopFilter::getEnablePositionIncrements() {
    return enablePositionIncrements;
}

void StopFilter::setEnablePositionIncrements(bool enable) {
    this->enablePositionIncrements = enable;
}

}
