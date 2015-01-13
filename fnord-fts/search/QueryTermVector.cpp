/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "fnord-fts/LuceneInc.h"
#include "fnord-fts/search/QueryTermVector.h"
#include "fnord-fts/analysis/Analyzer.h"
#include "fnord-fts/analysis/TokenStream.h"
#include "fnord-fts/util/StringReader.h"
#include "fnord-fts/analysis/tokenattributes/TermAttribute.h"

namespace Lucene {

QueryTermVector::QueryTermVector(Collection<String> queryTerms) {
    terms = Collection<String>::newInstance();
    termFreqs = Collection<int32_t>::newInstance();
    processTerms(queryTerms);
}

QueryTermVector::QueryTermVector(const String& queryString, const AnalyzerPtr& analyzer) {
    terms = Collection<String>::newInstance();
    termFreqs = Collection<int32_t>::newInstance();
    if (analyzer) {
        TokenStreamPtr stream(analyzer->tokenStream(L"", newLucene<StringReader>(queryString)));
        if (stream) {
            Collection<String> terms = Collection<String>::newInstance();
            try {
                bool hasMoreTokens = false;

                stream->reset();
                TermAttributePtr termAtt(stream->addAttribute<TermAttribute>());

                hasMoreTokens = stream->incrementToken();
                while (hasMoreTokens) {
                    terms.add(termAtt->term());
                    hasMoreTokens = stream->incrementToken();
                }
                processTerms(terms);
            } catch (IOException&) {
            }
        }
    }
}

QueryTermVector::~QueryTermVector() {
}

void QueryTermVector::processTerms(Collection<String> queryTerms) {
    if (queryTerms) {
        std::sort(queryTerms.begin(), queryTerms.end());
        MapStringInt tmpSet(MapStringInt::newInstance());

        // filter out duplicates
        Collection<String> tmpList(Collection<String>::newInstance());
        Collection<int32_t> tmpFreqs(Collection<int32_t>::newInstance());
        int32_t j = 0;
        for (int32_t i = 0; i < queryTerms.size(); ++i) {
            String term(queryTerms[i]);
            MapStringInt::iterator position = tmpSet.find(term);
            if (position == tmpSet.end()) {
                tmpSet.put(term, j++);
                tmpList.add(term);
                tmpFreqs.add(1);
            } else {
                int32_t freq = tmpFreqs[position->second];
                tmpFreqs[position->second] = freq + 1;
            }
        }
        terms = tmpList;
        termFreqs = Collection<int32_t>::newInstance(tmpFreqs.size());
        int32_t i = 0;
        for (Collection<int32_t>::iterator freq = tmpFreqs.begin(); freq != tmpFreqs.end(); ++freq) {
            termFreqs[i++] = *freq;
        }
    }
}

String QueryTermVector::toString() {
    StringStream buffer;
    buffer << L"{";
    for (int32_t i = 0; i < terms.size(); ++i) {
        if (i > 0) {
            buffer << L", ";
        }
        buffer << terms[i] << L'/' << termFreqs[i];
    }
    buffer << L"}";
    return buffer.str();
}

int32_t QueryTermVector::size() {
    return terms.size();
}

Collection<String> QueryTermVector::getTerms() {
    return terms;
}

Collection<int32_t> QueryTermVector::getTermFrequencies() {
    return termFreqs;
}

int32_t QueryTermVector::indexOf(const String& term) {
    Collection<String>::iterator search = std::lower_bound(terms.begin(), terms.end(), term);
    return (search == terms.end() || term < *search) ? -1 : std::distance(terms.begin(), search);
}

Collection<int32_t> QueryTermVector::indexesOf(Collection<String> terms, int32_t start, int32_t length) {
    Collection<int32_t> res(Collection<int32_t>::newInstance(length));
    for (int32_t i = 0; i < length; ++i) {
        res[i] = indexOf(terms[i]);
    }
    return res;
}

}
