/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "PerFieldAnalyzerWrapper.h"
#include "Fieldable.h"

namespace Lucene {

PerFieldAnalyzerWrapper::PerFieldAnalyzerWrapper(const AnalyzerPtr& defaultAnalyzer) {
    this->defaultAnalyzer = defaultAnalyzer;
    this->analyzerMap = MapStringAnalyzer::newInstance();
}

PerFieldAnalyzerWrapper::PerFieldAnalyzerWrapper(const AnalyzerPtr& defaultAnalyzer, MapStringAnalyzer fieldAnalyzers) {
    this->defaultAnalyzer = defaultAnalyzer;
    this->analyzerMap = MapStringAnalyzer::newInstance();
    if (fieldAnalyzers) {
        analyzerMap.putAll(fieldAnalyzers.begin(), fieldAnalyzers.end());
    }
}

PerFieldAnalyzerWrapper::~PerFieldAnalyzerWrapper() {
}

void PerFieldAnalyzerWrapper::addAnalyzer(const String& fieldName, const AnalyzerPtr& analyzer) {
    analyzerMap.put(fieldName, analyzer);
}

TokenStreamPtr PerFieldAnalyzerWrapper::tokenStream(const String& fieldName, const ReaderPtr& reader) {
    AnalyzerPtr analyzer(analyzerMap.get(fieldName));
    if (!analyzer) {
        analyzer = defaultAnalyzer;
    }
    return analyzer->tokenStream(fieldName, reader);
}

TokenStreamPtr PerFieldAnalyzerWrapper::reusableTokenStream(const String& fieldName, const ReaderPtr& reader) {
    AnalyzerPtr analyzer(analyzerMap.get(fieldName));
    if (!analyzer) {
        analyzer = defaultAnalyzer;
    }
    return analyzer->reusableTokenStream(fieldName, reader);
}

int32_t PerFieldAnalyzerWrapper::getPositionIncrementGap(const String& fieldName) {
    AnalyzerPtr analyzer(analyzerMap.get(fieldName));
    if (!analyzer) {
        analyzer = defaultAnalyzer;
    }
    return analyzer->getPositionIncrementGap(fieldName);
}

int32_t PerFieldAnalyzerWrapper::getOffsetGap(const FieldablePtr& field) {
    AnalyzerPtr analyzer(analyzerMap.get(field->name()));
    if (!analyzer) {
        analyzer = defaultAnalyzer;
    }
    return analyzer->getOffsetGap(field);
}

String PerFieldAnalyzerWrapper::toString() {
    return L"PerFieldAnalyzerWrapper(default=" + defaultAnalyzer->toString() + L")";
}

}
