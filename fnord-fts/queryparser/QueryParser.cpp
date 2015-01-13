/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include <iostream>
#include <boost/algorithm/string.hpp>
#include "QueryParser.h"
#include "QueryParserTokenManager.h"
#include "QueryParserToken.h"
#include "QueryParseError.h"
#include "MultiTermQuery.h"
#include "TermQuery.h"
#include "TermRangeQuery.h"
#include "FuzzyQuery.h"
#include "FastCharStream.h"
#include "StringReader.h"
#include "BooleanQuery.h"
#include "CachingTokenFilter.h"
#include "TermAttribute.h"
#include "Term.h"
#include "PositionIncrementAttribute.h"
#include "PhraseQuery.h"
#include "MultiPhraseQuery.h"
#include "PrefixQuery.h"
#include "WildcardQuery.h"
#include "MatchAllDocsQuery.h"
#include "SimpleAnalyzer.h"
#include "DateField.h"
#include "MiscUtils.h"
#include "StringUtils.h"

namespace Lucene {

const int32_t QueryParser::CONJ_NONE = 0;
const int32_t QueryParser::CONJ_AND = 1;
const int32_t QueryParser::CONJ_OR = 2;

const int32_t QueryParser::MOD_NONE = 0;
const int32_t QueryParser::MOD_NOT = 10;
const int32_t QueryParser::MOD_REQ = 11;

const int32_t QueryParser::jj_la1_0[] = {
    0x300, 0x300, 0x1c00, 0x1c00, 0x3ed3f00, 0x90000, 0x20000, 0x3ed2000, 0x2690000, 0x100000, 0x100000, 0x20000,
    0x30000000, 0x4000000, 0x30000000, 0x20000, 0x0, 0x40000000, 0x0, 0x20000, 0x100000, 0x20000, 0x3ed0000
};

const int32_t QueryParser::jj_la1_1[] = {
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x3, 0x0, 0x3, 0x0, 0x0, 0x0, 0x0
};

QueryParser::QueryParser(LuceneVersion::Version matchVersion, const String& field, const AnalyzerPtr& analyzer) {
    ConstructParser(newLucene<FastCharStream>(newLucene<StringReader>(L"")), QueryParserTokenManagerPtr());
    this->analyzer = analyzer;
    this->field = field;
    this->enablePositionIncrements = LuceneVersion::onOrAfter(matchVersion, LuceneVersion::LUCENE_29);
}

QueryParser::QueryParser(const QueryParserCharStreamPtr& stream) {
    ConstructParser(stream, QueryParserTokenManagerPtr());
}

QueryParser::QueryParser(const QueryParserTokenManagerPtr& tokenMgr) {
    ConstructParser(QueryParserCharStreamPtr(), tokenMgr);
}

QueryParser::~QueryParser() {
}

void QueryParser::ConstructParser(const QueryParserCharStreamPtr& stream, const QueryParserTokenManagerPtr& tokenMgr) {
    _operator = OR_OPERATOR;
    lowercaseExpandedTerms = true;
    multiTermRewriteMethod = MultiTermQuery::CONSTANT_SCORE_AUTO_REWRITE_DEFAULT();
    allowLeadingWildcard = false;
    enablePositionIncrements = true;
    phraseSlop = 0;
    fuzzyMinSim = FuzzyQuery::defaultMinSimilarity();
    fuzzyPrefixLength = FuzzyQuery::defaultPrefixLength;
    locale = std::locale();
    dateResolution = DateTools::RESOLUTION_NULL;

    token_source = tokenMgr ? tokenMgr : newLucene<QueryParserTokenManager>(stream);
    token = newLucene<QueryParserToken>();
    _jj_ntk = -1;
    jj_la = 0;
    jj_gen = 0;
    jj_rescan = false;
    jj_gc = 0;
    jj_la1 = Collection<int32_t>::newInstance(23);
    jj_2_rtns = Collection<JJCallsPtr>::newInstance(1);
    for (int32_t i = 0; i < 23; ++i) {
        jj_la1[i] = -1;
    }
    for (int32_t i = 0; i < jj_2_rtns.size(); ++i) {
        jj_2_rtns[i] = newInstance<JJCalls>();
    }
    jj_expentries = Collection< Collection<int32_t> >::newInstance();
    jj_kind = -1;
    jj_lasttokens = Collection<int32_t>::newInstance(100);
    jj_endpos = 0;
}

QueryPtr QueryParser::parse(const String& query) {
    ReInit(newLucene<FastCharStream>(newLucene<StringReader>(query)));
    try {
        // TopLevelQuery is a Query followed by the end-of-input (EOF)
        QueryPtr res(TopLevelQuery(field));
        return res ? res : newBooleanQuery(false);
    } catch (QueryParserError& e) {
        boost::throw_exception(QueryParserError(L"Cannot parse '" + query + L"': " + e.getError()));
    } catch (TooManyClausesException&) {
        boost::throw_exception(QueryParserError(L"Cannot parse '" + query + L"': too many boolean clauses"));
    }
    return QueryPtr();
}

AnalyzerPtr QueryParser::getAnalyzer() {
    return analyzer;
}

String QueryParser::getField() {
    return field;
}

double QueryParser::getFuzzyMinSim() {
    return fuzzyMinSim;
}

void QueryParser::setFuzzyMinSim(double fuzzyMinSim) {
    this->fuzzyMinSim = fuzzyMinSim;
}

int32_t QueryParser::getFuzzyPrefixLength() {
    return fuzzyPrefixLength;
}

void QueryParser::setFuzzyPrefixLength(int32_t fuzzyPrefixLength) {
    this->fuzzyPrefixLength = fuzzyPrefixLength;
}

void QueryParser::setPhraseSlop(int32_t phraseSlop) {
    this->phraseSlop = phraseSlop;
}

int32_t QueryParser::getPhraseSlop() {
    return phraseSlop;
}

void QueryParser::setAllowLeadingWildcard(bool allowLeadingWildcard) {
    this->allowLeadingWildcard = allowLeadingWildcard;
}

bool QueryParser::getAllowLeadingWildcard() {
    return allowLeadingWildcard;
}

void QueryParser::setEnablePositionIncrements(bool enable) {
    this->enablePositionIncrements = enable;
}

bool QueryParser::getEnablePositionIncrements() {
    return enablePositionIncrements;
}

void QueryParser::setDefaultOperator(Operator op) {
    this->_operator = op;
}

QueryParser::Operator QueryParser::getDefaultOperator() {
    return _operator;
}

void QueryParser::setLowercaseExpandedTerms(bool lowercaseExpandedTerms) {
    this->lowercaseExpandedTerms = lowercaseExpandedTerms;
}

bool QueryParser::getLowercaseExpandedTerms() {
    return lowercaseExpandedTerms;
}

void QueryParser::setMultiTermRewriteMethod(const RewriteMethodPtr& method) {
    multiTermRewriteMethod = method;
}

RewriteMethodPtr QueryParser::getMultiTermRewriteMethod() {
    return multiTermRewriteMethod;
}

void QueryParser::setLocale(std::locale locale) {
    this->locale = locale;
}

std::locale QueryParser::getLocale() {
    return locale;
}

void QueryParser::setDateResolution(DateTools::Resolution dateResolution) {
    this->dateResolution = dateResolution;
}

void QueryParser::setDateResolution(const String& fieldName, DateTools::Resolution dateResolution) {
    if (fieldName.empty()) {
        boost::throw_exception(IllegalArgumentException(L"Field cannot be empty."));
    }

    if (!fieldToDateResolution) {
        // lazily initialize Map
        fieldToDateResolution = MapStringResolution::newInstance();
    }

    fieldToDateResolution.put(fieldName, dateResolution);
}

DateTools::Resolution QueryParser::getDateResolution(const String& fieldName) {
    if (fieldName.empty()) {
        boost::throw_exception(IllegalArgumentException(L"Field cannot be empty."));
    }

    if (!fieldToDateResolution) {
        // no field specific date resolutions set; return default date resolution instead
        return this->dateResolution;
    }

    MapStringResolution::iterator resolution = fieldToDateResolution.find(fieldName);
    if (resolution == fieldToDateResolution.end()) {
        // no date resolutions set for the given field; return default date resolution instead
        return this->dateResolution;
    }

    return resolution->second;
}

void QueryParser::setRangeCollator(const CollatorPtr& rc) {
    rangeCollator = rc;
}

CollatorPtr QueryParser::getRangeCollator() {
    return rangeCollator;
}

void QueryParser::addClause(Collection<BooleanClausePtr> clauses, int32_t conj, int32_t mods, const QueryPtr& q) {
    bool required = false;
    bool prohibited = false;

    // If this term is introduced by AND, make the preceding term required, unless it's already prohibited
    if (!clauses.empty() && conj == CONJ_AND) {
        BooleanClausePtr c(clauses[clauses.size() - 1]);
        if (!c->isProhibited()) {
            c->setOccur(BooleanClause::MUST);
        }
    }

    if (!clauses.empty() && _operator == AND_OPERATOR && conj == CONJ_OR) {
        // If this term is introduced by OR, make the preceding term optional, unless it's prohibited (that
        // means we leave -a OR b but +a OR b-->a OR b) notice if the input is a OR b, first term is parsed
        // as required; without this modification a OR b would parsed as +a OR b
        BooleanClausePtr c(clauses[clauses.size() - 1]);
        if (!c->isProhibited()) {
            c->setOccur(BooleanClause::SHOULD);
        }
    }

    // We might have been passed a null query; the term might have been filtered away by the analyzer.
    if (!q) {
        return;
    }

    if (_operator == OR_OPERATOR) {
        // We set REQUIRED if we're introduced by AND or +; PROHIBITED if introduced by NOT or -; make
        // sure not to set both.
        prohibited = (mods == MOD_NOT);
        required = (mods == MOD_REQ);
        if (conj == CONJ_AND && !prohibited) {
            required = true;
        }
    } else {
        // We set PROHIBITED if we're introduced by NOT or -; We set REQUIRED if not PROHIBITED and not
        // introduced by OR
        prohibited = (mods == MOD_NOT);
        required = (!prohibited && conj != CONJ_OR);
    }
    if (required && !prohibited) {
        clauses.add(newBooleanClause(q, BooleanClause::MUST));
    } else if (!required && !prohibited) {
        clauses.add(newBooleanClause(q, BooleanClause::SHOULD));
    } else if (!required && prohibited) {
        clauses.add(newBooleanClause(q, BooleanClause::MUST_NOT));
    } else {
        boost::throw_exception(RuntimeException(L"Clause cannot be both required and prohibited"));
    }
}

QueryPtr QueryParser::getFieldQuery(const String& field, const String& queryText) {
    TokenStreamPtr source;
    try {
        source = analyzer->reusableTokenStream(field, newLucene<StringReader>(queryText));
        source->reset();
    } catch (IOException&) {
        source = analyzer->tokenStream(field, newLucene<StringReader>(queryText));
    }

    CachingTokenFilterPtr buffer(newLucene<CachingTokenFilter>(source));
    TermAttributePtr termAtt;
    PositionIncrementAttributePtr posIncrAtt;
    int32_t numTokens = 0;

    bool success = false;
    try {
        buffer->reset();
        success = true;
    } catch (IOException&) {
        // success == false if we hit an exception
    }
    if (success) {
        if (buffer->hasAttribute<TermAttribute>()) {
            termAtt = buffer->getAttribute<TermAttribute>();
        }
        if (buffer->hasAttribute<PositionIncrementAttribute>()) {
            posIncrAtt = buffer->getAttribute<PositionIncrementAttribute>();
        }
    }

    int32_t positionCount = 0;
    bool severalTokensAtSamePosition = false;

    bool hasMoreTokens = false;
    if (termAtt) {
        try {
            hasMoreTokens = buffer->incrementToken();
            while (hasMoreTokens) {
                ++numTokens;
                int32_t positionIncrement = posIncrAtt ? posIncrAtt->getPositionIncrement() : 1;
                if (positionIncrement != 0) {
                    positionCount += positionIncrement;
                } else {
                    severalTokensAtSamePosition = true;
                }
                hasMoreTokens = buffer->incrementToken();
            }
        } catch (IOException&) {
            // ignore
        }
    }
    try {
        // rewind the buffer stream
        buffer->reset();

        // close original stream - all tokens buffered
        source->close();
    } catch (IOException&) {
        // ignore
    }

    if (numTokens == 0) {
        return QueryPtr();
    } else if (numTokens == 1) {
        String term;
        try {
            bool hasNext = buffer->incrementToken();
            BOOST_ASSERT(hasNext);
            term = termAtt->term();
        } catch (IOException&) {
            // safe to ignore, because we know the number of tokens
        }
        return newTermQuery(newLucene<Term>(field, term));
    } else {
        if (severalTokensAtSamePosition) {
            if (positionCount == 1) {
                // no phrase query
                BooleanQueryPtr q(newBooleanQuery(true));
                for (int32_t i = 0; i < numTokens; ++i) {
                    String term;
                    try {
                        bool hasNext = buffer->incrementToken();
                        BOOST_ASSERT(hasNext);
                        term = termAtt->term();
                    } catch (IOException&) {
                        // safe to ignore, because we know the number of tokens
                    }

                    QueryPtr currentQuery(newTermQuery(newLucene<Term>(field, term)));
                    q->add(currentQuery, BooleanClause::SHOULD);
                }
                return q;
            } else {
                // phrase query
                MultiPhraseQueryPtr mpq(newMultiPhraseQuery());
                mpq->setSlop(phraseSlop);
                Collection<TermPtr> multiTerms(Collection<TermPtr>::newInstance());
                int32_t position = -1;
                for (int32_t i = 0; i < numTokens; ++i) {
                    String term;
                    int32_t positionIncrement = 1;
                    try {
                        bool hasNext = buffer->incrementToken();
                        BOOST_ASSERT(hasNext);
                        term = termAtt->term();
                        if (posIncrAtt) {
                            positionIncrement = posIncrAtt->getPositionIncrement();
                        }
                    } catch (IOException&) {
                        // safe to ignore, because we know the number of tokens
                    }

                    if (positionIncrement > 0 && !multiTerms.empty()) {
                        if (enablePositionIncrements) {
                            mpq->add(Collection<TermPtr>::newInstance(multiTerms.begin(), multiTerms.end()), position);
                        } else {
                            mpq->add(Collection<TermPtr>::newInstance(multiTerms.begin(), multiTerms.end()));
                        }
                        multiTerms.clear();
                    }
                    position += positionIncrement;
                    multiTerms.add(newLucene<Term>(field, term));
                }
                if (enablePositionIncrements) {
                    mpq->add(Collection<TermPtr>::newInstance(multiTerms.begin(), multiTerms.end()), position);
                } else {
                    mpq->add(Collection<TermPtr>::newInstance(multiTerms.begin(), multiTerms.end()));
                }
                return mpq;
            }
        } else {
            PhraseQueryPtr pq(newPhraseQuery());
            pq->setSlop(phraseSlop);
            int32_t position = -1;

            for (int32_t i = 0; i < numTokens; ++i) {
                String term;
                int32_t positionIncrement = 1;

                try {
                    bool hasNext = buffer->incrementToken();
                    BOOST_ASSERT(hasNext);
                    term = termAtt->term();
                    if (posIncrAtt) {
                        positionIncrement = posIncrAtt->getPositionIncrement();
                    }
                } catch (IOException&) {
                    // safe to ignore, because we know the number of tokens
                }

                if (enablePositionIncrements) {
                    position += positionIncrement;
                    pq->add(newLucene<Term>(field, term), position);
                } else {
                    pq->add(newLucene<Term>(field, term));
                }
            }
            return pq;
        }
    }
}

QueryPtr QueryParser::getFieldQuery(const String& field, const String& queryText, int32_t slop) {
    QueryPtr query(getFieldQuery(field, queryText));
    if (MiscUtils::typeOf<PhraseQuery>(query)) {
        boost::dynamic_pointer_cast<PhraseQuery>(query)->setSlop(slop);
    }
    if (MiscUtils::typeOf<MultiPhraseQuery>(query)) {
        boost::dynamic_pointer_cast<MultiPhraseQuery>(query)->setSlop(slop);
    }
    return query;
}

QueryPtr QueryParser::getRangeQuery(const String& field, const String& part1, const String& part2, bool inclusive) {
    String date1(part1);
    String date2(part2);
    if (lowercaseExpandedTerms) {
        StringUtils::toLower(date1);
        StringUtils::toLower(date2);
    }
    try {
        boost::posix_time::ptime d1(DateTools::parseDate(date1, locale));
        boost::posix_time::ptime d2;

        // The user can only specify the date, not the time, so make sure the time is set to
        // the latest possible time of that date to really include all documents
        if (inclusive) {
            d2 = boost::posix_time::ptime(DateTools::parseDate(date2, locale) +
                                          boost::posix_time::hours(23) +
                                          boost::posix_time::minutes(59) +
                                          boost::posix_time::seconds(59) +
                                          boost::posix_time::millisec(999));
        } else {
            d2 = boost::posix_time::ptime(DateTools::parseDate(date2, locale));
        }
        DateTools::Resolution resolution = getDateResolution(field);
        if (resolution == DateTools::RESOLUTION_NULL) {
            // no default or field specific date resolution has been set, use deprecated
            // DateField to maintain compatibility with pre-1.9 Lucene versions.
            date1 = DateField::dateToString(d1);
            date2 = DateField::dateToString(d2);
        } else {
            date1 = DateTools::dateToString(d1, resolution);
            date2 = DateTools::dateToString(d2, resolution);
        }
    } catch (...) {
    }
    return newRangeQuery(field, date1, date2, inclusive);
}

BooleanQueryPtr QueryParser::newBooleanQuery(bool disableCoord) {
    return newLucene<BooleanQuery>(disableCoord);
}

BooleanClausePtr QueryParser::newBooleanClause(const QueryPtr& q, BooleanClause::Occur occur) {
    return newLucene<BooleanClause>(q, occur);
}

QueryPtr QueryParser::newTermQuery(const TermPtr& term) {
    return newLucene<TermQuery>(term);
}

PhraseQueryPtr QueryParser::newPhraseQuery() {
    return newLucene<PhraseQuery>();
}

MultiPhraseQueryPtr QueryParser::newMultiPhraseQuery() {
    return newLucene<MultiPhraseQuery>();
}

QueryPtr QueryParser::newPrefixQuery(const TermPtr& prefix) {
    PrefixQueryPtr query(newLucene<PrefixQuery>(prefix));
    query->setRewriteMethod(multiTermRewriteMethod);
    return query;
}

QueryPtr QueryParser::newFuzzyQuery(const TermPtr& term, double minimumSimilarity, int32_t prefixLength) {
    // FuzzyQuery doesn't yet allow constant score rewrite
    return newLucene<FuzzyQuery>(term, minimumSimilarity, prefixLength);
}

QueryPtr QueryParser::newRangeQuery(const String& field, const String& part1, const String& part2, bool inclusive) {
    TermRangeQueryPtr query(newLucene<TermRangeQuery>(field, part1, part2, inclusive, inclusive, rangeCollator));
    query->setRewriteMethod(multiTermRewriteMethod);
    return query;
}

QueryPtr QueryParser::newMatchAllDocsQuery() {
    return newLucene<MatchAllDocsQuery>();
}

QueryPtr QueryParser::newWildcardQuery(const TermPtr& term) {
    WildcardQueryPtr query(newLucene<WildcardQuery>(term));
    query->setRewriteMethod(multiTermRewriteMethod);
    return query;
}

QueryPtr QueryParser::getBooleanQuery(Collection<BooleanClausePtr> clauses) {
    return getBooleanQuery(clauses, false);
}

QueryPtr QueryParser::getBooleanQuery(Collection<BooleanClausePtr> clauses, bool disableCoord) {
    if (clauses.empty()) {
        return QueryPtr();    // all clause words were filtered away by the analyzer.
    }
    BooleanQueryPtr query(newBooleanQuery(disableCoord));
    for (Collection<BooleanClausePtr>::iterator clause = clauses.begin(); clause != clauses.end(); ++clause) {
        query->add(*clause);
    }
    return query;
}

QueryPtr QueryParser::getWildcardQuery(const String& field, const String& termStr) {
    if (field == L"*" && termStr == L"*") {
        return newMatchAllDocsQuery();
    }
    if (!allowLeadingWildcard && (boost::starts_with(termStr, L"*") || boost::starts_with(termStr, L"?"))) {
        boost::throw_exception(QueryParserError(L"'*' or '?' not allowed as first character in WildcardQuery"));
    }
    String queryTerm(termStr);
    if (lowercaseExpandedTerms) {
        StringUtils::toLower(queryTerm);
    }
    TermPtr term(newLucene<Term>(field, queryTerm));
    return newWildcardQuery(term);
}

QueryPtr QueryParser::getPrefixQuery(const String& field, const String& termStr) {
    if (!allowLeadingWildcard && boost::starts_with(termStr, L"*")) {
        boost::throw_exception(QueryParserError(L"'*' not allowed as first character in PrefixQuery"));
    }
    String queryTerm(termStr);
    if (lowercaseExpandedTerms) {
        StringUtils::toLower(queryTerm);
    }
    TermPtr term(newLucene<Term>(field, queryTerm));
    return newPrefixQuery(term);
}

QueryPtr QueryParser::getFuzzyQuery(const String& field, const String& termStr, double minSimilarity) {
    String queryTerm(termStr);
    if (lowercaseExpandedTerms) {
        StringUtils::toLower(queryTerm);
    }
    TermPtr term(newLucene<Term>(field, queryTerm));
    return newFuzzyQuery(term, minSimilarity, fuzzyPrefixLength);
}

String QueryParser::discardEscapeChar(const String& input) {
    // Create char array to hold unescaped char sequence
    CharArray output(CharArray::newInstance(input.length()));

    // The length of the output can be less than the input due to discarded escape chars.
    // This variable holds the actual length of the output
    int32_t length = 0;

    // We remember whether the last processed character was an escape character
    bool lastCharWasEscapeChar = false;

    // The multiplier the current unicode digit must be multiplied with.  eg. the first digit must
    // be multiplied with 16^3, the second with 16^2
    int32_t codePointMultiplier = 0;

    // Used to calculate the codepoint of the escaped unicode character
    int32_t codePoint = 0;

    for (int32_t i = 0; i < (int32_t)input.length(); ++i) {
        wchar_t curChar = input[i];
        if (codePointMultiplier > 0) {
            codePoint += hexToInt(curChar) * codePointMultiplier;
            codePointMultiplier = MiscUtils::unsignedShift(codePointMultiplier, 4);
            if (codePointMultiplier == 0) {
                output[length++] = (wchar_t)codePoint;
                codePoint = 0;
            }
        } else if (lastCharWasEscapeChar) {
            if (curChar == L'u') {
                // found an escaped unicode character
                codePointMultiplier = 16 * 16 * 16;
            } else {
                // this character was escaped
                output[length++] = curChar;
            }
            lastCharWasEscapeChar = false;
        } else {
            if (curChar == L'\\') {
                lastCharWasEscapeChar = true;
            } else {
                output[length++] = curChar;
            }
        }
    }

    if (codePointMultiplier > 0) {
        boost::throw_exception(QueryParserError(L"Truncated unicode escape sequence."));
    }
    if (lastCharWasEscapeChar) {
        boost::throw_exception(QueryParserError(L"Term can not end with escape character."));
    }
    return String(output.get(), length);
}

int32_t QueryParser::hexToInt(wchar_t c) {
    if (L'0' <= c && c <= L'9') {
        return c - L'0';
    } else if (L'a' <= c && c <= L'f') {
        return c - L'a' + 10;
    } else if (L'A' <= c && c <= L'F') {
        return c - L'A' + 10;
    } else {
        boost::throw_exception(QueryParserError(L"None-hex character in unicode escape sequence: " + StringUtils::toString(c)));
        return 0;
    }
}

String QueryParser::escape(const String& s) {
    StringStream buffer;
    for (int32_t i = 0; i < (int32_t)s.length(); ++i) {
        wchar_t c = s[i];
        // These characters are part of the query syntax and must be escaped
        if (c == L'\\' || c == L'+' || c == L'-' || c == L'!' || c == L'(' || c == L')' || c == L':' ||
                c == L'^' || c == L'[' || c == L']' || c == L'\"' || c == L'{' || c == L'}' || c == L'~' ||
                c == L'*' || c == L'?' || c == L'|' || c == L'&') {
            buffer << L"\\";
        }
        buffer << c;
    }
    return buffer.str();
}

int QueryParser::main(Collection<String> args) {
    if (args.empty()) {
        std::wcout << L"Usage: QueryParser <input>";
        return 1;
    }
    QueryParserPtr qp(newLucene<QueryParser>(LuceneVersion::LUCENE_CURRENT, L"field", newLucene<SimpleAnalyzer>()));
    QueryPtr q(qp->parse(args[0]));
    std::wcout << q->toString(L"field");
    return 0;
}

int32_t QueryParser::Conjunction() {
    int32_t ret = CONJ_NONE;
    switch ((_jj_ntk == -1) ? jj_ntk() : _jj_ntk) {
    case AND:
    case OR:
        switch ((_jj_ntk == -1) ? jj_ntk() : _jj_ntk) {
        case AND:
            jj_consume_token(AND);
            ret = CONJ_AND;
            break;
        case OR:
            jj_consume_token(OR);
            ret = CONJ_OR;
            break;
        default:
            jj_la1[0] = jj_gen;
            jj_consume_token(-1);
            boost::throw_exception(QueryParserError());
        }
        break;
    default:
        jj_la1[1] = jj_gen;
    }
    return ret;
}

int32_t QueryParser::Modifiers() {
    int32_t ret = MOD_NONE;
    switch ((_jj_ntk == -1) ? jj_ntk() : _jj_ntk) {
    case NOT:
    case PLUS:
    case MINUS:
        switch ((_jj_ntk == -1) ? jj_ntk() : _jj_ntk) {
        case PLUS:
            jj_consume_token(PLUS);
            ret = MOD_REQ;
            break;
        case MINUS:
            jj_consume_token(MINUS);
            ret = MOD_NOT;
            break;
        case NOT:
            jj_consume_token(NOT);
            ret = MOD_NOT;
            break;
        default:
            jj_la1[2] = jj_gen;
            jj_consume_token(-1);
            boost::throw_exception(QueryParserError());
        }
        break;
    default:
        jj_la1[3] = jj_gen;
    }
    return ret;
}

QueryPtr QueryParser::TopLevelQuery(const String& field) {
    QueryPtr q(ParseQuery(field));
    jj_consume_token(0);
    return q;
}

QueryPtr QueryParser::ParseQuery(const String& field) {
    Collection<BooleanClausePtr> clauses(Collection<BooleanClausePtr>::newInstance());
    QueryPtr firstQuery;
    int32_t mods = Modifiers();
    QueryPtr q(ParseClause(field));
    addClause(clauses, CONJ_NONE, mods, q);
    if (mods == MOD_NONE) {
        firstQuery = q;
    }
    for (bool more = true; more; ) {
        switch ((_jj_ntk == -1) ? jj_ntk() : _jj_ntk) {
        case AND:
        case OR:
        case NOT:
        case PLUS:
        case MINUS:
        case LPAREN:
        case STAR:
        case QUOTED:
        case TERM:
        case PREFIXTERM:
        case WILDTERM:
        case RANGEIN_START:
        case RANGEEX_START:
        case NUMBER:
            break;
        default:
            jj_la1[4] = jj_gen;
            more = false;
            continue;
        }
        int32_t conj = Conjunction();
        mods = Modifiers();
        q = ParseClause(field);
        addClause(clauses, conj, mods, q);
    }
    if (clauses.size() == 1 && firstQuery) {
        return firstQuery;
    } else {
        return getBooleanQuery(clauses);
    }
}

QueryPtr QueryParser::ParseClause(const String& field) {
    QueryPtr q;
    QueryParserTokenPtr fieldToken;
    QueryParserTokenPtr boost;
    String fieldClause(field);
    if (jj_2_1(2)) {
        switch ((_jj_ntk == -1) ? jj_ntk() : _jj_ntk) {
        case TERM:
            fieldToken = jj_consume_token(TERM);
            jj_consume_token(COLON);
            fieldClause = discardEscapeChar(fieldToken->image);
            break;
        case STAR:
            jj_consume_token(STAR);
            jj_consume_token(COLON);
            fieldClause = L"*";
            break;
        default:
            jj_la1[5] = jj_gen;
            jj_consume_token(-1);
            boost::throw_exception(QueryParserError());
        }
    }
    switch ((_jj_ntk == -1) ? jj_ntk() : _jj_ntk) {
    case STAR:
    case QUOTED:
    case TERM:
    case PREFIXTERM:
    case WILDTERM:
    case RANGEIN_START:
    case RANGEEX_START:
    case NUMBER:
        q = ParseTerm(fieldClause);
        break;
    case LPAREN:
        jj_consume_token(LPAREN);
        q = ParseQuery(fieldClause);
        jj_consume_token(RPAREN);
        switch ((_jj_ntk == -1) ? jj_ntk() : _jj_ntk) {
        case CARAT:
            jj_consume_token(CARAT);
            boost = jj_consume_token(NUMBER);
            break;
        default:
            jj_la1[6] = jj_gen;
        }
        break;
    default:
        jj_la1[7] = jj_gen;
        jj_consume_token(-1);
        boost::throw_exception(QueryParserError());
    }
    if (boost) {
        double f = 1.0;
        try {
            if (q) {
                f = StringUtils::toDouble(boost->image);
                q->setBoost(f);
            }
        } catch (...) {
        }
    }
    return q;
}

QueryPtr QueryParser::ParseTerm(const String& field) {
    QueryParserTokenPtr term;
    QueryParserTokenPtr boost;
    QueryParserTokenPtr fuzzySlop;
    QueryParserTokenPtr goop1;
    QueryParserTokenPtr goop2;
    bool prefix = false;
    bool wildcard = false;
    bool fuzzy = false;
    QueryPtr q;
    switch ((_jj_ntk == -1) ? jj_ntk() : _jj_ntk) {
    case STAR:
    case TERM:
    case PREFIXTERM:
    case WILDTERM:
    case NUMBER:
        switch ((_jj_ntk == -1) ? jj_ntk() : _jj_ntk) {
        case TERM:
            term = jj_consume_token(TERM);
            break;
        case STAR:
            term = jj_consume_token(STAR);
            wildcard = true;
            break;
        case PREFIXTERM:
            term = jj_consume_token(PREFIXTERM);
            prefix = true;
            break;
        case WILDTERM:
            term = jj_consume_token(WILDTERM);
            wildcard = true;
            break;
        case NUMBER:
            term = jj_consume_token(NUMBER);
            break;
        default:
            jj_la1[8] = jj_gen;
            jj_consume_token(-1);
            boost::throw_exception(QueryParserError());
        }
        switch ((_jj_ntk == -1) ? jj_ntk() : _jj_ntk) {
        case FUZZY_SLOP:
            fuzzySlop = jj_consume_token(FUZZY_SLOP);
            fuzzy = true;
            break;
        default:
            jj_la1[9] = jj_gen;
        }
        switch ((_jj_ntk == -1) ? jj_ntk() : _jj_ntk) {
        case CARAT:
            jj_consume_token(CARAT);
            boost = jj_consume_token(NUMBER);
            switch ((_jj_ntk == -1) ? jj_ntk() : _jj_ntk) {
            case FUZZY_SLOP:
                fuzzySlop = jj_consume_token(FUZZY_SLOP);
                fuzzy = true;
                break;
            default:
                jj_la1[10] = jj_gen;
            }
            break;
        default:
            jj_la1[11] = jj_gen;
        }
        {
            String termImage(discardEscapeChar(term->image));
            if (wildcard) {
                q = getWildcardQuery(field, termImage);
            } else if (prefix) {
                q = getPrefixQuery(field, discardEscapeChar(term->image.substr(0, term->image.length() - 1)));
            } else if (fuzzy) {
                double fms = fuzzyMinSim;
                try {
                    fms = StringUtils::toDouble(fuzzySlop->image.substr(1));
                } catch (...) {
                }
                if (fms < 0.0 || fms > 1.0) {
                    boost::throw_exception(QueryParserError(L"Minimum similarity for a FuzzyQuery has to be between 0.0 and 1.0"));
                }
                q = getFuzzyQuery(field, termImage, fms);
            } else {
                q = getFieldQuery(field, termImage);
            }
        }
        break;
    case RANGEIN_START:
        jj_consume_token(RANGEIN_START);
        switch ((_jj_ntk == -1) ? jj_ntk() : _jj_ntk) {
        case RANGEIN_GOOP:
            goop1 = jj_consume_token(RANGEIN_GOOP);
            break;
        case RANGEIN_QUOTED:
            goop1 = jj_consume_token(RANGEIN_QUOTED);
            break;
        default:
            jj_la1[12] = jj_gen;
            jj_consume_token(-1);
            boost::throw_exception(QueryParserError());
        }
        switch ((_jj_ntk == -1) ? jj_ntk() : _jj_ntk) {
        case RANGEIN_TO:
            jj_consume_token(RANGEIN_TO);
            break;
        default:
            jj_la1[13] = jj_gen;
        }
        switch ((_jj_ntk == -1) ? jj_ntk() : _jj_ntk) {
        case RANGEIN_GOOP:
            goop2 = jj_consume_token(RANGEIN_GOOP);
            break;
        case RANGEIN_QUOTED:
            goop2 = jj_consume_token(RANGEIN_QUOTED);
            break;
        default:
            jj_la1[14] = jj_gen;
            jj_consume_token(-1);
            boost::throw_exception(QueryParserError());
        }
        jj_consume_token(RANGEIN_END);
        switch ((_jj_ntk == -1) ? jj_ntk() : _jj_ntk) {
        case CARAT:
            jj_consume_token(CARAT);
            boost = jj_consume_token(NUMBER);
            break;
        default:
            jj_la1[15] = jj_gen;
        }
        if (goop1->kind == RANGEIN_QUOTED) {
            goop1->image = goop1->image.substr(1, std::max((int32_t)0, (int32_t)goop1->image.length() - 2));
        }
        if (goop2->kind == RANGEIN_QUOTED) {
            goop2->image = goop2->image.substr(1, std::max((int32_t)0, (int32_t)goop2->image.length() - 2));
        }
        q = getRangeQuery(field, discardEscapeChar(goop1->image), discardEscapeChar(goop2->image), true);
        break;
    case RANGEEX_START:
        jj_consume_token(RANGEEX_START);
        switch ((_jj_ntk == -1) ? jj_ntk() : _jj_ntk) {
        case RANGEEX_GOOP:
            goop1 = jj_consume_token(RANGEEX_GOOP);
            break;
        case RANGEEX_QUOTED:
            goop1 = jj_consume_token(RANGEEX_QUOTED);
            break;
        default:
            jj_la1[16] = jj_gen;
            jj_consume_token(-1);
            boost::throw_exception(QueryParserError());
        }
        switch ((_jj_ntk == -1) ? jj_ntk() : _jj_ntk) {
        case RANGEEX_TO:
            jj_consume_token(RANGEEX_TO);
            break;
        default:
            jj_la1[17] = jj_gen;
        }
        switch ((_jj_ntk == -1) ? jj_ntk() : _jj_ntk) {
        case RANGEEX_GOOP:
            goop2 = jj_consume_token(RANGEEX_GOOP);
            break;
        case RANGEEX_QUOTED:
            goop2 = jj_consume_token(RANGEEX_QUOTED);
            break;
        default:
            jj_la1[18] = jj_gen;
            jj_consume_token(-1);
            boost::throw_exception(QueryParserError());
        }
        jj_consume_token(RANGEEX_END);
        switch ((_jj_ntk == -1) ? jj_ntk() : _jj_ntk) {
        case CARAT:
            jj_consume_token(CARAT);
            boost = jj_consume_token(NUMBER);
            break;
        default:
            jj_la1[19] = jj_gen;
        }
        if (goop1->kind == RANGEEX_QUOTED) {
            goop1->image = goop1->image.substr(1, std::max((int32_t)0, (int32_t)goop1->image.length() - 2));
        }
        if (goop2->kind == RANGEEX_QUOTED) {
            goop2->image = goop2->image.substr(1, std::max((int32_t)0, (int32_t)goop2->image.length() - 2));
        }
        q = getRangeQuery(field, discardEscapeChar(goop1->image), discardEscapeChar(goop2->image), false);
        break;
    case QUOTED:
        term = jj_consume_token(QUOTED);
        switch ((_jj_ntk == -1) ? jj_ntk() : _jj_ntk) {
        case FUZZY_SLOP:
            fuzzySlop = jj_consume_token(FUZZY_SLOP);
            break;
        default:
            jj_la1[20] = jj_gen;
        }
        switch ((_jj_ntk == -1) ? jj_ntk() : _jj_ntk) {
        case CARAT:
            jj_consume_token(CARAT);
            boost = jj_consume_token(NUMBER);
            break;
        default:
            jj_la1[21] = jj_gen;
        }
        {
            int32_t s = phraseSlop;
            if (fuzzySlop) {
                try {
                    s = StringUtils::toInt(fuzzySlop->image.substr(1));
                } catch (...) {
                }
            }
            q = getFieldQuery(field, discardEscapeChar(term->image.substr(1, std::max((int32_t)0, (int32_t)term->image.length() - 2))), s);
        }
        break;
    default:
        jj_la1[22] = jj_gen;
        jj_consume_token(-1);
        boost::throw_exception(QueryParserError());
    }
    if (boost) {
        double f = 1.0;
        try {
            f = StringUtils::toDouble(boost->image);
        } catch (...) {
        }

        // avoid boosting null queries, such as those caused by stop words
        if (q) {
            q->setBoost(f);
        }
    }
    return q;
}

bool QueryParser::jj_2_1(int32_t xla) {
    jj_la = xla;
    jj_scanpos = token;
    jj_lastpos = jj_scanpos;
    bool _jj_2_1 = false;
    LuceneException finally;
    try {
        _jj_2_1 = !jj_3_1();
    } catch (LookaheadSuccess&) {
        _jj_2_1 = true;
    } catch (LuceneException& e) {
        finally = e;
    }
    jj_save(0, xla);
    finally.throwException();
    return _jj_2_1;
}

bool QueryParser::jj_3R_2() {
    if (jj_scan_token(TERM)) {
        return true;
    }
    if (jj_scan_token(COLON)) {
        return true;
    }
    return false;
}

bool QueryParser::jj_3_1() {
    QueryParserTokenPtr xsp(jj_scanpos);
    if (jj_3R_2()) {
        jj_scanpos = xsp;
        if (jj_3R_3()) {
            return true;
        }
    }
    return false;
}

bool QueryParser::jj_3R_3() {
    if (jj_scan_token(STAR)) {
        return true;
    }
    if (jj_scan_token(COLON)) {
        return true;
    }
    return false;
}

void QueryParser::ReInit(const QueryParserCharStreamPtr& stream) {
    token_source->ReInit(stream);
    token = newLucene<QueryParserToken>();
    _jj_ntk = -1;
    jj_gen = 0;
    for (int32_t i = 0; i < 23; ++i) {
        jj_la1[i] = -1;
    }
    for (int32_t i = 0; i < jj_2_rtns.size(); ++i) {
        jj_2_rtns[i] = newInstance<JJCalls>();
    }
}

void QueryParser::ReInit(const QueryParserTokenManagerPtr& tokenMgr) {
    token_source = tokenMgr;
    token = newLucene<QueryParserToken>();
    _jj_ntk = -1;
    jj_gen = 0;
    for (int32_t i = 0; i < 23; ++i) {
        jj_la1[i] = -1;
    }
    for (int32_t i = 0; i < jj_2_rtns.size(); ++i) {
        jj_2_rtns[i] = newInstance<JJCalls>();
    }
}

QueryParserTokenPtr QueryParser::jj_consume_token(int32_t kind) {
    QueryParserTokenPtr oldToken(token);
    if (oldToken->next) {
        token = token->next;
    } else {
        token->next = token_source->getNextToken();
        token = token->next;
    }
    _jj_ntk = -1;
    if (token->kind == kind) {
        ++jj_gen;
        if (++jj_gc > 100) {
            jj_gc = 0;
            for (int32_t i = 0; i < jj_2_rtns.size(); ++i) {
                JJCallsPtr c(jj_2_rtns[i]);
                while (c) {
                    if (c->gen < jj_gen) {
                        c->first.reset();
                    }
                    c = c->next;
                }
            }
        }
        return token;
    }
    token = oldToken;
    jj_kind = kind;
    generateParseException();
    return QueryParserTokenPtr();
}

bool QueryParser::jj_scan_token(int32_t kind) {
    if (jj_scanpos == jj_lastpos) {
        --jj_la;
        if (!jj_scanpos->next) {
            jj_scanpos->next = token_source->getNextToken();
            jj_scanpos = jj_scanpos->next;
            jj_lastpos = jj_scanpos;
        } else {
            jj_scanpos = jj_scanpos->next;
            jj_lastpos = jj_scanpos;
        }
    } else {
        jj_scanpos = jj_scanpos->next;
    }
    if (jj_rescan) {
        int32_t i = 0;
        QueryParserTokenPtr tok(token);
        while (tok && tok != jj_scanpos) {
            ++i;
            tok = tok->next;
        }
        if (tok) {
            jj_add_error_token(kind, i);
        }
    }
    if (jj_scanpos->kind != kind) {
        return true;
    }
    if (jj_la == 0 && jj_scanpos == jj_lastpos) {
        boost::throw_exception(LookaheadSuccess());
    }
    return false;
}

QueryParserTokenPtr QueryParser::getNextToken() {
    if (token->next) {
        token = token->next;
    } else {
        token->next = token_source->getNextToken();
        token = token->next;
    }
    _jj_ntk = -1;
    ++jj_gen;
    return token;
}

QueryParserTokenPtr QueryParser::getToken(int32_t index) {
    QueryParserTokenPtr t(token);
    for (int32_t i = 0; i < index; ++i) {
        if (t->next) {
            t = t->next;
        } else {
            t->next = token_source->getNextToken();
            t = t->next;
        }
    }
    return t;
}

int32_t QueryParser::jj_ntk() {
    jj_nt = token->next;
    if (!jj_nt) {
        token->next = token_source->getNextToken();
        _jj_ntk = token->next->kind;
        return _jj_ntk;
    } else {
        _jj_ntk = jj_nt->kind;
        return _jj_ntk;
    }
}

void QueryParser::jj_add_error_token(int32_t kind, int32_t pos) {
    if (pos >= 100) {
        return;
    }
    if (pos == jj_endpos + 1) {
        jj_lasttokens[jj_endpos++] = kind;
    } else if (jj_endpos != 0) {
        jj_expentry = Collection<int32_t>::newInstance(jj_endpos);
        for (int32_t i = 0; i < jj_endpos; ++i) {
            jj_expentry[i] = jj_lasttokens[i];
        }
        for (Collection< Collection<int32_t> >::iterator oldentry = jj_expentries.begin(); oldentry != jj_expentries.end(); ++oldentry) {
            if (oldentry->size() == jj_expentry.size()) {
                bool jj_entries_loop = true;
                for (int32_t i = 0; i < jj_expentry.size(); ++i) {
                    if ((*oldentry)[i] != jj_expentry[i]) {
                        jj_entries_loop = false;
                        break;
                    }
                }
                if (!jj_entries_loop) {
                    continue;
                }
                jj_expentries.add(jj_expentry);
                break;
            }
        }
        if (pos != 0) {
            jj_endpos = pos;
            jj_lasttokens[jj_endpos - 1] = kind;
        }
    }
}

void QueryParser::generateParseException() {
    jj_expentries.clear();
    Collection<uint8_t> la1tokens(Collection<uint8_t>::newInstance(34));
    if (jj_kind >= 0) {
        la1tokens[jj_kind] = true;
        jj_kind = -1;
    }
    for (int32_t i = 0; i < 23; ++i) {
        if (jj_la1[i] == jj_gen) {
            for (int32_t j = 0; j < 32; ++j) {
                if ((jj_la1_0[i] & (1 << j)) != 0) {
                    la1tokens[j] = true;
                }
                if ((jj_la1_1[i] & (1 << j)) != 0) {
                    la1tokens[32 + j] = true;
                }
            }
        }
    }
    for (int32_t i = 0; i < 34; ++i) {
        if (la1tokens[i]) {
            jj_expentry = Collection<int32_t>::newInstance(1);
            jj_expentry[0] = i;
            jj_expentries.add(jj_expentry);
        }
    }
    jj_endpos = 0;
    jj_rescan_token();
    jj_add_error_token(0, 0);
    Collection< Collection<int32_t> > exptokseq(Collection< Collection<int32_t> >::newInstance(jj_expentries.size()));
    for (int32_t i = 0; i < jj_expentries.size(); ++i) {
        exptokseq[i] = jj_expentries[i];
    }
    boost::throw_exception(QueryParserError(QueryParseError::parseError(token, exptokseq, tokenImage)));
}

void QueryParser::enable_tracing() {
}

void QueryParser::disable_tracing() {
}

void QueryParser::jj_rescan_token() {
    jj_rescan = true;
    for (int32_t i = 0; i < 1; ++i) {
        try {
            JJCallsPtr p(jj_2_rtns[i]);
            do {
                if (p->gen > jj_gen) {
                    jj_la = p->arg;
                    jj_scanpos = p->first;
                    jj_lastpos = jj_scanpos;
                    jj_3_1();
                }
                p = p->next;
            } while (p);
        } catch (LookaheadSuccess&) {
        }
    }
    jj_rescan = false;
}

void QueryParser::jj_save(int32_t index, int32_t xla) {
    JJCallsPtr p(jj_2_rtns[index]);
    while (p->gen > jj_gen) {
        if (!p->next) {
            p->next = newInstance<JJCalls>();
            p = p->next;
            break;
        }
        p = p->next;
    }
    p->gen = jj_gen + xla - jj_la;
    p->first = token;
    p->arg = xla;
}

}
