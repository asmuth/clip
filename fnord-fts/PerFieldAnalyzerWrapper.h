/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef PERFIELDANALYZERWRAPPER_H
#define PERFIELDANALYZERWRAPPER_H

#include "Analyzer.h"

namespace Lucene {

/// This analyzer is used to facilitate scenarios where different fields require different analysis techniques.
/// Use {@link #addAnalyzer} to add a non-default analyzer on a field name basis.
///
/// Example usage:
///
/// <pre>
/// PerFieldAnalyzerWrapperPtr aWrapper = newLucene<PerFieldAnalyzerWrapper>(newLucene<StandardAnalyzer>());
/// aWrapper->addAnalyzer(L"firstname", newLucene<KeywordAnalyzer>());
/// aWrapper->addAnalyzer(L"lastname", newLucene<KeywordAnalyzer>());
/// </pre>
///
/// In this example, StandardAnalyzer will be used for all fields except "firstname" and "lastname", for which
/// KeywordAnalyzer will be used.
///
/// A PerFieldAnalyzerWrapper can be used like any other analyzer, for both indexing and query parsing.
class PerFieldAnalyzerWrapper : public Analyzer {
public:
    /// Constructs with default analyzer.
    /// @param defaultAnalyzer Any fields not specifically defined to use a different analyzer will use the
    /// one provided here.
    PerFieldAnalyzerWrapper(const AnalyzerPtr& defaultAnalyzer);

    /// Constructs with default analyzer and a map of analyzers to use for specific fields.
    /// @param defaultAnalyzer Any fields not specifically defined to use a different analyzer will use the one provided here.
    /// @param fieldAnalyzers a Map (String field name to the Analyzer) to be used for those fields
    PerFieldAnalyzerWrapper(const AnalyzerPtr& defaultAnalyzer, MapStringAnalyzer fieldAnalyzers);

    virtual ~PerFieldAnalyzerWrapper();

    LUCENE_CLASS(PerFieldAnalyzerWrapper);

protected:
    AnalyzerPtr defaultAnalyzer;
    MapStringAnalyzer analyzerMap;

public:
    /// Defines an analyzer to use for the specified field.
    /// @param fieldName field name requiring a non-default analyzer
    /// @param analyzer non-default analyzer to use for field
    void addAnalyzer(const String& fieldName, const AnalyzerPtr& analyzer);

    virtual TokenStreamPtr tokenStream(const String& fieldName, const ReaderPtr& reader);
    virtual TokenStreamPtr reusableTokenStream(const String& fieldName, const ReaderPtr& reader);

    /// Return the positionIncrementGap from the analyzer assigned to fieldName.
    virtual int32_t getPositionIncrementGap(const String& fieldName);

    /// Return the offsetGap from the analyzer assigned to field
    virtual int32_t getOffsetGap(const FieldablePtr& field);

    virtual String toString();
};

}

#endif
