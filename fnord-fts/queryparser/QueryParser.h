/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef QUERYPARSER_H
#define QUERYPARSER_H

#include "fnord-fts/queryparser/QueryParserConstants.h"
#include "fnord-fts/document/DateTools.h"
#include "fnord-fts/search/BooleanClause.h"

namespace fnord {
namespace fts {

typedef HashMap<String, DateTools::Resolution> MapStringResolution;

/// The most important method is {@link #parse(const String&)}.
///
/// The syntax for query strings is as follows:
/// A Query is a series of clauses.
/// A clause may be prefixed by:
/// <ul>
/// <li> a plus (+) or a minus (-) sign, indicating that the clause is required or prohibited respectively; or
/// <li> a term followed by a colon, indicating the field to be searched.  This enables one to construct queries
/// which search multiple fields.
/// </ul>
///
/// A clause may be either:
/// <ul>
/// <li> a term, indicating all the documents that contain this term; or
/// <li> a nested query, enclosed in parentheses.  Note that this may be used with a +/- prefix to require any
/// of a set of terms.
/// </ul>
///
/// Thus, in BNF, the query grammar is:
/// <pre>
/// Query  ::= ( Clause )*
/// Clause ::= ["+", "-"] [<TERM> ":"] ( <TERM> | "(" Query ")" )
/// </pre>
///
/// Examples of appropriately formatted queries can be found in the query syntax documentation.
///
/// In {@link TermRangeQuery}s, QueryParser tries to detect date values, eg.
/// <tt>date:[6/1/2005 TO 6/4/2005]</tt> produces a range query that searches for "date" fields between
/// 2005-06-01 and 2005-06-04.  Note that the format of the accepted input depends on {@link #setLocale(Locale)
/// the locale}.
///
/// By default a date is converted into a search term using the deprecated {@link DateField} for compatibility
/// reasons.  To use the new {@link DateTools} to convert dates, a {@link Resolution} has to be set.
///
/// The date resolution that shall be used for RangeQueries can be set using {@link #setDateResolution(Resolution)}
/// or {@link #setDateResolution(const String&, Resolution)}.  The former sets the default date resolution for
/// all fields, whereas the latter can be used to set field specific date resolutions. Field specific date
/// resolutions take, if set, precedence over the default date resolution.
///
/// If you use neither {@link DateField} nor {@link DateTools} in your index, you can create your own query
/// parser that inherits QueryParser and overwrites {@link #getRangeQuery(const String&, const String&,
/// const String&, bool)} to use a different method for date conversion.
///
/// Note that QueryParser is not thread-safe.
///
/// NOTE: there is a new QueryParser in contrib, which matches the same syntax as this class, but is more modular,
/// enabling substantial customization to how a query is created.
///
/// NOTE: You must specify the required {@link Version} compatibility when creating QueryParser:
/// <ul>
/// <li> As of 2.9, {@link #setEnablePositionIncrements} is true by default.
/// </ul>
class QueryParser : public QueryParserConstants, public LuceneObject {
public:
    /// Constructs a query parser.
    /// @param matchVersion Lucene version to match.
    /// @param field The default field for query terms.
    /// @param analyzer Used to find terms in the query text.
    QueryParser(LuceneVersion::Version matchVersion, const String& field, const AnalyzerPtr& analyzer);

    /// Constructor with user supplied QueryParserCharStream.
    QueryParser(const QueryParserCharStreamPtr& stream);

    /// Constructor with generated Token Manager.
    QueryParser(const QueryParserTokenManagerPtr& tokenMgr);

    virtual ~QueryParser();

    LUCENE_CLASS(QueryParser);

    /// The default operator for parsing queries. Use {@link QueryParser#setDefaultOperator} to change it.
    enum Operator { OR_OPERATOR, AND_OPERATOR };

protected:
    static const int32_t CONJ_NONE;
    static const int32_t CONJ_AND;
    static const int32_t CONJ_OR;

    static const int32_t MOD_NONE;
    static const int32_t MOD_NOT;
    static const int32_t MOD_REQ;

    /// The actual operator that parser uses to combine query terms
    Operator _operator;

    /// Next token.
    int32_t _jj_ntk;
    QueryParserTokenPtr jj_scanpos;
    QueryParserTokenPtr jj_lastpos;

    int32_t jj_la;
    int32_t jj_gen;
    Collection<int32_t> jj_la1;

    static const int32_t jj_la1_0[];
    static const int32_t jj_la1_1[];

    struct JJCalls;
    typedef std::shared_ptr<JJCalls> JJCallsPtr;

    struct JJCalls {
        JJCalls() {
            gen = 0;
            arg = 0;
        }

        int32_t gen;
        QueryParserTokenPtr first;
        int32_t arg;
        JJCallsPtr next;
    };

    Collection<JJCallsPtr> jj_2_rtns;
    bool jj_rescan;
    int32_t jj_gc;

    Collection< Collection<int32_t> > jj_expentries;
    Collection<int32_t> jj_expentry;
    int32_t jj_kind;
    Collection<int32_t> jj_lasttokens;
    int32_t jj_endpos;

public:
    bool lowercaseExpandedTerms;
    RewriteMethodPtr multiTermRewriteMethod;
    bool allowLeadingWildcard;
    bool enablePositionIncrements;

    AnalyzerPtr analyzer;
    String field;
    int32_t phraseSlop;
    double fuzzyMinSim;
    int32_t fuzzyPrefixLength;
    std::locale locale;

    // the default date resolution
    DateTools::Resolution dateResolution;

    // maps field names to date resolutions
    MapStringResolution fieldToDateResolution;

    // The collator to use when determining range inclusion, for use when constructing RangeQuerys
    CollatorPtr rangeCollator;

    /// Generated Token Manager.
    QueryParserTokenManagerPtr token_source;

    /// Current token.
    QueryParserTokenPtr token;

    /// Next token.
    QueryParserTokenPtr jj_nt;

public:
    /// Parses a query string, returning a {@link Query}.
    /// @param query The query string to be parsed.
    QueryPtr parse(const String& query);

    /// @return Returns the analyzer.
    AnalyzerPtr getAnalyzer();

    /// @return Returns the field.
    String getField();

    /// Get the minimal similarity for fuzzy queries.
    double getFuzzyMinSim();

    /// Set the minimum similarity for fuzzy queries.  Default is 0.5.
    void setFuzzyMinSim(double fuzzyMinSim);

    /// Get the prefix length for fuzzy queries.
    /// @return Returns the fuzzyPrefixLength.
    int32_t getFuzzyPrefixLength();

    /// Set the prefix length for fuzzy queries.  Default is 0.
    /// @param fuzzyPrefixLength The fuzzyPrefixLength to set.
    void setFuzzyPrefixLength(int32_t fuzzyPrefixLength);

    /// Sets the default slop for phrases.  If zero, then exact phrase matches are required.
    /// Default value is zero.
    void setPhraseSlop(int32_t phraseSlop);

    /// Gets the default slop for phrases.
    int32_t getPhraseSlop();

    /// Set to true to allow leading wildcard characters.
    ///
    /// When set, * or ? are allowed as the first character of a PrefixQuery and WildcardQuery.
    /// Note that this can produce very slow queries on big indexes.  Default: false.
    void setAllowLeadingWildcard(bool allowLeadingWildcard);

    /// @see #setAllowLeadingWildcard(bool)
    bool getAllowLeadingWildcard();

    /// Set to true to enable position increments in result query.
    ///
    /// When set, result phrase and multi-phrase queries will be aware of position increments.
    /// Useful when eg. a StopFilter increases the position increment of the token that follows an
    /// omitted token.  Default: false.
    void setEnablePositionIncrements(bool enable);

    /// @see #setEnablePositionIncrements(bool)
    bool getEnablePositionIncrements();

    /// Sets the boolean operator of the QueryParser.  In default mode (OR_OPERATOR) terms without
    /// any modifiers are considered optional: for example capital of Hungary is equal to capital
    /// OR of OR Hungary.
    /// In AND_OPERATOR mode terms are considered to be in conjunction: the above mentioned query is
    /// parsed as capital AND of AND Hungary
    void setDefaultOperator(Operator op);

    /// Gets implicit operator setting, which will be either AND_OPERATOR or OR_OPERATOR.
    Operator getDefaultOperator();

    /// Whether terms of wildcard, prefix, fuzzy and range queries are to be automatically lower-cased
    /// or not.  Default is true.
    void setLowercaseExpandedTerms(bool lowercaseExpandedTerms);

    /// @see #setLowercaseExpandedTerms(bool)
    bool getLowercaseExpandedTerms();

    /// By default QueryParser uses {@link MultiTermQuery#CONSTANT_SCORE_AUTO_REWRITE_DEFAULT} when
    /// creating a PrefixQuery, WildcardQuery or RangeQuery. This implementation is generally preferable
    /// because it a) Runs faster b) Does not have the scarcity of terms unduly influence score c) avoids
    /// any "TooManyClauses" exception.  However, if your application really needs to use the old-
    /// fashioned BooleanQuery expansion rewriting and the above points are not relevant then use this
    /// to change the rewrite method.
    void setMultiTermRewriteMethod(const RewriteMethodPtr& method);

    /// @see #setMultiTermRewriteMethod
    RewriteMethodPtr getMultiTermRewriteMethod();

    /// Set locale used by date range parsing.
    void setLocale(std::locale locale);

    /// Returns current locale, allowing access by subclasses.
    std::locale getLocale();

    /// Sets the default date resolution used by RangeQueries for fields for which no specific date
    /// resolutions has been set. Field specific resolutions can be set with {@link
    /// #setDateResolution(const String&, DateTools::Resolution)}.
    /// @param dateResolution The default date resolution to set
    void setDateResolution(DateTools::Resolution dateResolution);

    /// Sets the date resolution used by RangeQueries for a specific field.
    /// @param fieldName Field for which the date resolution is to be set
    /// @param dateResolution Date resolution to set
    void setDateResolution(const String& fieldName, DateTools::Resolution dateResolution);

    /// Returns the date resolution that is used by RangeQueries for the given field.  Returns null, if
    /// no default or field specific date resolution has been set for the given field.
    DateTools::Resolution getDateResolution(const String& fieldName);

    /// Sets the collator used to determine index term inclusion in ranges for RangeQuerys.
    ///
    /// WARNING: Setting the rangeCollator to a non-null collator using this method will cause every
    /// single index Term in the Field referenced by lowerTerm and/or upperTerm to be examined.  Depending
    /// on the number of index Terms in this Field, the operation could be very slow.
    /// @param rc The collator to use when constructing RangeQuerys
    void setRangeCollator(const CollatorPtr& rc);

    /// @return the collator used to determine index term inclusion in ranges for RangeQuerys.
    CollatorPtr getRangeCollator();

    /// Command line tool to test QueryParser, using {@link SimpleAnalyzer}.
    static int main(Collection<String> args);

    /// Query  ::= ( Clause )*
    /// Clause ::= ["+", "-"] [<TERM> ":"] ( <TERM> | "(" Query ")" )
    int32_t Conjunction();
    int32_t Modifiers();

    /// This makes sure that there is no garbage after the query string
    virtual QueryPtr TopLevelQuery(const String& field);

    virtual QueryPtr ParseQuery(const String& field);
    virtual QueryPtr ParseClause(const String& field);
    virtual QueryPtr ParseTerm(const String& field);

    /// Reinitialise.
    virtual void ReInit(const QueryParserCharStreamPtr& stream);

    /// Reinitialise.
    virtual void ReInit(const QueryParserTokenManagerPtr& tokenMgr);

    /// Get the next Token.
    virtual QueryParserTokenPtr getNextToken();

    /// Get the specific Token.
    virtual QueryParserTokenPtr getToken(int32_t index);

    /// Generate QueryParserError exception.
    virtual void generateParseException();

    /// Enable tracing.
    virtual void enable_tracing();

    /// Disable tracing.
    virtual void disable_tracing();

protected:
    /// Construct query parser with supplied QueryParserCharStream or TokenManager
    void ConstructParser(const QueryParserCharStreamPtr& stream, const QueryParserTokenManagerPtr& tokenMgr);

    virtual void addClause(Collection<BooleanClausePtr> clauses, int32_t conj, int32_t mods, const QueryPtr& q);

    /// Use the analyzer to get all the tokens, and then build a TermQuery, PhraseQuery, or nothing
    /// based on the term count.
    virtual QueryPtr getFieldQuery(const String& field, const String& queryText);

    /// Base implementation delegates to {@link #getFieldQuery(const String&, const String&)}.
    /// This method may be overridden, for example, to return a SpanNearQuery instead of a PhraseQuery.
    virtual QueryPtr getFieldQuery(const String& field, const String& queryText, int32_t slop);

    /// Builds a new TermRangeQuery instance for given min/max parts
    virtual QueryPtr getRangeQuery(const String& field, const String& part1, const String& part2, bool inclusive);

    /// Builds a new BooleanQuery instance
    /// @param disableCoord disable coord
    BooleanQueryPtr newBooleanQuery(bool disableCoord);

    /// Builds a new BooleanClause instance
    /// @param q sub query
    /// @param occur how this clause should occur when matching documents
    /// @return new BooleanClause instance
    BooleanClausePtr newBooleanClause(const QueryPtr& q, BooleanClause::Occur occur);

    /// Builds a new TermQuery instance
    /// @param term term
    /// @return new TermQuery instance
    QueryPtr newTermQuery(const TermPtr& term);

    /// Builds a new PhraseQuery instance
    /// @return new PhraseQuery instance
    PhraseQueryPtr newPhraseQuery();

    /// Builds a new MultiPhraseQuery instance
    /// @return new MultiPhraseQuery instance
    MultiPhraseQueryPtr newMultiPhraseQuery();

    /// Builds a new PrefixQuery instance
    /// @param prefix Prefix term
    /// @return new PrefixQuery instance
    QueryPtr newPrefixQuery(const TermPtr& prefix);

    /// Builds a new FuzzyQuery instance
    /// @param term Term
    /// @param minimumSimilarity minimum similarity
    /// @param prefixLength prefix length
    /// @return new FuzzyQuery Instance
    QueryPtr newFuzzyQuery(const TermPtr& term, double minimumSimilarity, int32_t prefixLength);

    /// Builds a new TermRangeQuery instance
    /// @param field Field
    /// @param part1 min
    /// @param part2 max
    /// @param inclusive true if range is inclusive
    /// @return new TermRangeQuery instance
    QueryPtr newRangeQuery(const String& field, const String& part1, const String& part2, bool inclusive);

    /// Builds a new MatchAllDocsQuery instance
    /// @return new MatchAllDocsQuery instance
    QueryPtr newMatchAllDocsQuery();

    /// Builds a new WildcardQuery instance
    /// @param t wildcard term
    /// @return new WildcardQuery instance
    QueryPtr newWildcardQuery(const TermPtr& term);

    /// Factory method for generating query, given a set of clauses.  By default creates a boolean query
    /// composed of clauses passed in.
    ///
    /// Can be overridden by extending classes, to modify query being returned.
    ///
    /// @param clauses List that contains {@link BooleanClause} instances to join.
    /// @return Resulting {@link Query} object.
    virtual QueryPtr getBooleanQuery(Collection<BooleanClausePtr> clauses);

    /// Factory method for generating query, given a set of clauses.  By default creates a boolean query
    /// composed of clauses passed in.
    ///
    /// Can be overridden by extending classes, to modify query being returned.
    ///
    /// @param clauses List that contains {@link BooleanClause} instances to join.
    /// @param disableCoord true if coord scoring should be disabled.
    /// @return Resulting {@link Query} object.
    virtual QueryPtr getBooleanQuery(Collection<BooleanClausePtr> clauses, bool disableCoord);

    /// Factory method for generating a query.  Called when parser parses an input term token that contains
    /// one or more wildcard characters (? and *), but is not a prefix term token (one that has just a
    /// single * character at the end)
    ///
    /// Depending on settings, prefix term may be lower-cased automatically. It will not go through the
    /// default Analyzer, however, since normal Analyzers are unlikely to work properly with wildcard
    /// templates.
    ///
    /// Can be overridden by extending classes, to provide custom handling for wildcard queries, which may
    /// be necessary due to missing analyzer calls.
    ///
    /// @param field Name of the field query will use.
    /// @param termStr Term token that contains one or more wild card characters (? or *), but is not simple
    /// prefix term
    /// @return Resulting {@link Query} built for the term
    virtual QueryPtr getWildcardQuery(const String& field, const String& termStr);

    /// Factory method for generating a query (similar to {@link #getWildcardQuery}).  Called when parser
    /// parses an input term token that uses prefix notation; that is, contains a single '*' wildcard
    /// character as its last character. Since this is a special case of generic wildcard term, and such
    /// a query can be optimized easily, this usually results in a different query object.
    ///
    /// Depending on settings, a prefix term may be lower-cased automatically. It will not go through the
    /// default Analyzer, however, since normal Analyzers are unlikely to work properly with wildcard templates.
    ///
    /// Can be overridden by extending classes, to provide custom handling for wild card queries, which may be
    /// necessary due to missing analyzer calls.
    ///
    /// @param field Name of the field query will use.
    /// @param termStr Term token to use for building term for the query (without trailing '*' character)
    /// @return Resulting {@link Query} built for the term
    virtual QueryPtr getPrefixQuery(const String& field, const String& termStr);

    /// Factory method for generating a query (similar to {@link #getWildcardQuery}). Called when parser
    /// parses an input term token that has the fuzzy suffix (~) appended.
    ///
    /// @param field Name of the field query will use.
    /// @param termStr Term token to use for building term for the query
    /// @return Resulting {@link Query} built for the term
    virtual QueryPtr getFuzzyQuery(const String& field, const String& termStr, double minSimilarity);

    /// Returns a String where the escape char has been removed, or kept only once if there was a double
    /// escape.  Supports escaped unicode characters, eg. translates \\u0041 to A.
    String discardEscapeChar(const String& input);

    /// Returns the numeric value of the hexadecimal character
    static int32_t hexToInt(wchar_t c);

    /// Returns a String where those characters that QueryParser expects to be escaped are escaped by
    /// a preceding \.
    static String escape(const String& s);

    bool jj_2_1(int32_t xla);
    bool jj_3R_2();
    bool jj_3_1();
    bool jj_3R_3();

    QueryParserTokenPtr jj_consume_token(int32_t kind);
    bool jj_scan_token(int32_t kind);
    int32_t jj_ntk();
    void jj_add_error_token(int32_t kind, int32_t pos);
    void jj_rescan_token();
    void jj_save(int32_t index, int32_t xla);
};

}

}
#endif
