/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "MultiFieldQueryParser.h"
#include "BooleanQuery.h"
#include "BooleanClause.h"
#include "PhraseQuery.h"
#include "MultiPhraseQuery.h"
#include "MiscUtils.h"

namespace Lucene {

MultiFieldQueryParser::MultiFieldQueryParser(LuceneVersion::Version matchVersion, Collection<String> fields, const AnalyzerPtr& analyzer, MapStringDouble boosts) : QueryParser(matchVersion, L"", analyzer) {
    this->boosts = boosts;
    this->fields = fields;
}

MultiFieldQueryParser::MultiFieldQueryParser(LuceneVersion::Version matchVersion, Collection<String> fields, const AnalyzerPtr& analyzer) : QueryParser(matchVersion, L"", analyzer) {
    this->fields = fields;
}

MultiFieldQueryParser::~MultiFieldQueryParser() {
}

QueryPtr MultiFieldQueryParser::getFieldQuery(const String& field, const String& queryText, int32_t slop) {
    if (field.empty()) {
        Collection<BooleanClausePtr> clauses(Collection<BooleanClausePtr>::newInstance());
        for (Collection<String>::iterator field = fields.begin(); field != fields.end(); ++field) {
            QueryPtr query(QueryParser::getFieldQuery(*field, queryText));
            if (query) {
                // If the user passes a map of boosts
                if (boosts) {
                    // Get the boost from the map and apply them
                    MapStringDouble::iterator boost = boosts.find(*field);
                    if (boost != boosts.end()) {
                        query->setBoost(boost->second);
                    }
                }
                applySlop(query, slop);
                clauses.add(newLucene<BooleanClause>(query, BooleanClause::SHOULD));
            }
        }
        if (clauses.empty()) { // happens for stopwords
            return QueryPtr();
        }
        return getBooleanQuery(clauses, true);
    }
    QueryPtr query(QueryParser::getFieldQuery(field, queryText));
    applySlop(query, slop);
    return query;
}

QueryPtr MultiFieldQueryParser::getFieldQuery(const String& field, const String& queryText) {
    return getFieldQuery(field, queryText, 0);
}

void MultiFieldQueryParser::applySlop(const QueryPtr& query, int32_t slop) {
    if (MiscUtils::typeOf<PhraseQuery>(query)) {
        boost::dynamic_pointer_cast<PhraseQuery>(query)->setSlop(slop);
    }
    if (MiscUtils::typeOf<MultiPhraseQuery>(query)) {
        boost::dynamic_pointer_cast<MultiPhraseQuery>(query)->setSlop(slop);
    }
}

QueryPtr MultiFieldQueryParser::getFuzzyQuery(const String& field, const String& termStr, double minSimilarity) {
    if (field.empty()) {
        Collection<BooleanClausePtr> clauses(Collection<BooleanClausePtr>::newInstance());
        for (Collection<String>::iterator field = fields.begin(); field != fields.end(); ++field) {
            clauses.add(newLucene<BooleanClause>(getFuzzyQuery(*field, termStr, minSimilarity), BooleanClause::SHOULD));
        }
        return getBooleanQuery(clauses, true);
    }
    return QueryParser::getFuzzyQuery(field, termStr, minSimilarity);
}

QueryPtr MultiFieldQueryParser::getPrefixQuery(const String& field, const String& termStr) {
    if (field.empty()) {
        Collection<BooleanClausePtr> clauses(Collection<BooleanClausePtr>::newInstance());
        for (Collection<String>::iterator field = fields.begin(); field != fields.end(); ++field) {
            clauses.add(newLucene<BooleanClause>(getPrefixQuery(*field, termStr), BooleanClause::SHOULD));
        }
        return getBooleanQuery(clauses, true);
    }
    return QueryParser::getPrefixQuery(field, termStr);
}

QueryPtr MultiFieldQueryParser::getWildcardQuery(const String& field, const String& termStr) {
    if (field.empty()) {
        Collection<BooleanClausePtr> clauses(Collection<BooleanClausePtr>::newInstance());
        for (Collection<String>::iterator field = fields.begin(); field != fields.end(); ++field) {
            clauses.add(newLucene<BooleanClause>(getWildcardQuery(*field, termStr), BooleanClause::SHOULD));
        }
        return getBooleanQuery(clauses, true);
    }
    return QueryParser::getWildcardQuery(field, termStr);
}

QueryPtr MultiFieldQueryParser::getRangeQuery(const String& field, const String& part1, const String& part2, bool inclusive) {
    if (field.empty()) {
        Collection<BooleanClausePtr> clauses(Collection<BooleanClausePtr>::newInstance());
        for (Collection<String>::iterator field = fields.begin(); field != fields.end(); ++field) {
            clauses.add(newLucene<BooleanClause>(getRangeQuery(*field, part1, part2, inclusive), BooleanClause::SHOULD));
        }
        return getBooleanQuery(clauses, true);
    }
    return QueryParser::getRangeQuery(field, part1, part2, inclusive);
}

QueryPtr MultiFieldQueryParser::parse(LuceneVersion::Version matchVersion, Collection<String> queries, Collection<String> fields, const AnalyzerPtr& analyzer) {
    if (queries.size() != fields.size()) {
        boost::throw_exception(IllegalArgumentException(L"queries.size() != fields.size()"));
    }
    BooleanQueryPtr booleanQuery(newLucene<BooleanQuery>());
    for (int32_t i = 0; i < fields.size(); ++i) {
        QueryParserPtr queryParser(newLucene<QueryParser>(matchVersion, fields[i], analyzer));
        QueryPtr query(queryParser->parse(queries[i]));
        if (query && (!MiscUtils::typeOf<BooleanQuery>(query) || !boost::dynamic_pointer_cast<BooleanQuery>(query)->getClauses().empty())) {
            booleanQuery->add(query, BooleanClause::SHOULD);
        }
    }
    return booleanQuery;
}

QueryPtr MultiFieldQueryParser::parse(LuceneVersion::Version matchVersion, const String& query, Collection<String> fields, Collection<BooleanClause::Occur> flags, const AnalyzerPtr& analyzer) {
    if (fields.size() != flags.size()) {
        boost::throw_exception(IllegalArgumentException(L"fields.size() != flags.size()"));
    }
    BooleanQueryPtr booleanQuery(newLucene<BooleanQuery>());
    for (int32_t i = 0; i < fields.size(); ++i) {
        QueryParserPtr queryParser(newLucene<QueryParser>(matchVersion, fields[i], analyzer));
        QueryPtr q(queryParser->parse(query));
        if (q && (!MiscUtils::typeOf<BooleanQuery>(q) || !boost::dynamic_pointer_cast<BooleanQuery>(q)->getClauses().empty())) {
            booleanQuery->add(q, flags[i]);
        }
    }
    return booleanQuery;
}

QueryPtr MultiFieldQueryParser::parse(LuceneVersion::Version matchVersion, Collection<String> queries, Collection<String> fields, Collection<BooleanClause::Occur> flags, const AnalyzerPtr& analyzer) {
    if (queries.size() != fields.size() || fields.size() != flags.size()) {
        boost::throw_exception(IllegalArgumentException(L"queries, fields, and flags array have have different length"));
    }
    BooleanQueryPtr booleanQuery(newLucene<BooleanQuery>());
    for (int32_t i = 0; i < fields.size(); ++i) {
        QueryParserPtr queryParser(newLucene<QueryParser>(matchVersion, fields[i], analyzer));
        QueryPtr query(queryParser->parse(queries[i]));
        if (query && (!MiscUtils::typeOf<BooleanQuery>(query) || !boost::dynamic_pointer_cast<BooleanQuery>(query)->getClauses().empty())) {
            booleanQuery->add(query, flags[i]);
        }
    }
    return booleanQuery;
}

}
