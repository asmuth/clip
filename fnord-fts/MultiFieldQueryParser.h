/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef MULTIFIELDQUERYPARSER_H
#define MULTIFIELDQUERYPARSER_H

#include "QueryParser.h"
#include "BooleanClause.h"

namespace Lucene {

/// A QueryParser which constructs queries to search multiple fields.
class MultiFieldQueryParser : public QueryParser {
public:
    /// Creates a MultiFieldQueryParser.  Allows passing of a map with term to Boost, and the boost to
    /// apply to each term.
    ///
    /// It will, when parse(String query) is called, construct a query like this (assuming the query
    /// consists of two terms and you specify the two fields title and body):
    /// <pre>
    /// (title:term1 body:term1) (title:term2 body:term2)
    /// </pre>
    ///
    /// When setDefaultOperator(AND_OPERATOR) is set, the result will be:
    /// <pre>
    /// +(title:term1 body:term1) +(title:term2 body:term2)
    /// </pre>
    ///
    /// When you pass a boost (title=>5 body=>10) you can get:
    /// <pre>
    /// +(title:term1^5.0 body:term1^10.0) +(title:term2^5.0 body:term2^10.0)
    /// </pre>
    ///
    /// In other words, all the query's terms must appear, but it doesn't matter in what fields they
    /// appear.
    MultiFieldQueryParser(LuceneVersion::Version matchVersion, Collection<String> fields, const AnalyzerPtr& analyzer, MapStringDouble boosts);

    /// Creates a MultiFieldQueryParser.  It will, when parse(String query) is called, construct a
    /// query like this (assuming the query consists of two terms and you specify the two fields
    /// title and body):
    /// <pre>
    /// (title:term1 body:term1) (title:term2 body:term2)
    /// </pre>
    ///
    /// When setDefaultOperator(AND_OPERATOR) is set, the result will be:
    /// <pre>
    /// +(title:term1 body:term1) +(title:term2 body:term2)
    /// </pre>
    ///
    /// In other words, all the query's terms must appear, but it doesn't matter in what fields they
    /// appear.
    MultiFieldQueryParser(LuceneVersion::Version matchVersion, Collection<String> fields, const AnalyzerPtr& analyzer);

    virtual ~MultiFieldQueryParser();

    LUCENE_CLASS(MultiFieldQueryParser);

protected:
    Collection<String> fields;
    MapStringDouble boosts;

public:
    using QueryParser::parse;

    /// Parses a query which searches on the fields specified.
    ///
    /// If x fields are specified, this effectively constructs:
    /// <pre>
    /// (field1:query1) (field2:query2) (field3:query3)...(fieldx:queryx)
    /// </pre>
    /// @param matchVersion Lucene version to match; this is passed through to QueryParser.
    /// @param queries Queries strings to parse
    /// @param fields Fields to search on
    /// @param analyzer Analyzer to use
    static QueryPtr parse(LuceneVersion::Version matchVersion, Collection<String> queries, Collection<String> fields, const AnalyzerPtr& analyzer);

    /// Parses a query, searching on the fields specified.  Use this if you need to specify certain fields as
    /// required, and others as prohibited.
    ///
    /// <pre>
    /// Usage:
    /// Collection<String> fields = newCollection<String>(L"filename", L"contents", L"description");
    /// Collection<BooleanClause::Occur> flags = newCollection<BooleanClause::Occur>(BooleanClause::SHOULD, BooleanClause::MUST, BooleanClause::MUST_NOT);
    /// MultiFieldQueryParser::parse(L"query", fields, flags, analyzer);
    /// </pre>
    ///
    /// The code above would construct a query:
    /// <pre>
    /// (filename:query) +(contents:query) -(description:query)
    /// </pre>
    ///
    /// @param matchVersion Lucene version to match; this is passed through to QueryParser.
    /// @param query Query string to parse
    /// @param fields Fields to search on
    /// @param flags Flags describing the fields
    /// @param analyzer Analyzer to use
    static QueryPtr parse(LuceneVersion::Version matchVersion, const String& query, Collection<String> fields, Collection<BooleanClause::Occur> flags, const AnalyzerPtr& analyzer);

    /// Parses a query, searching on the fields specified.  Use this if you need to specify certain fields as
    /// required, and others as prohibited.
    ///
    /// <pre>
    /// Usage:
    /// Collection<String> query = newCollection<String>(L"query1", L"query2", L"query3");
    /// Collection<String> fields = newCollection<String>(L"filename", L"contents", L"description");
    /// Collection<BooleanClause::Occur> flags = newCollection<BooleanClause::Occur>(BooleanClause::SHOULD, BooleanClause::MUST, BooleanClause::MUST_NOT);
    /// MultiFieldQueryParser::parse(query, fields, flags, analyzer);
    /// </pre>
    ///
    /// The code above would construct a query:
    /// <pre>
    /// (filename:query1) +(contents:query2) -(description:query3)
    /// </pre>
    ///
    /// @param matchVersion Lucene version to match; this is passed through to QueryParser.
    /// @param queries Queries string to parse
    /// @param fields Fields to search on
    /// @param flags Flags describing the fields
    /// @param analyzer Analyzer to use
    static QueryPtr parse(LuceneVersion::Version matchVersion, Collection<String> queries, Collection<String> fields, Collection<BooleanClause::Occur> flags, const AnalyzerPtr& analyzer);

protected:
    virtual QueryPtr getFieldQuery(const String& field, const String& queryText, int32_t slop);
    virtual QueryPtr getFieldQuery(const String& field, const String& queryText);
    void applySlop(const QueryPtr& query, int32_t slop);

    virtual QueryPtr getFuzzyQuery(const String& field, const String& termStr, double minSimilarity);
    virtual QueryPtr getPrefixQuery(const String& field, const String& termStr);
    virtual QueryPtr getWildcardQuery(const String& field, const String& termStr);
    virtual QueryPtr getRangeQuery(const String& field, const String& part1, const String& part2, bool inclusive);
};

}

#endif
