/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef FIELDMASKINGSPANQUERY_H
#define FIELDMASKINGSPANQUERY_H

#include "SpanQuery.h"

namespace Lucene {

/// Wrapper to allow {@link SpanQuery} objects participate in composite single-field SpanQueries by
/// 'lying' about their search field.  That is, the masked SpanQuery will function as normal, but
/// {@link SpanQuery#getField()} simply hands back the value supplied in this class's constructor.
///
/// This can be used to support Queries like {@link SpanNearQuery} or {@link SpanOrQuery} across
/// different fields, which is not ordinarily permitted.
///
/// This can be useful for denormalized relational data: for example, when indexing a document with
/// conceptually many 'children':
///
/// <pre>
/// teacherid: 1
/// studentfirstname: james
/// studentsurname: jones
///
/// teacherid: 2
/// studenfirstname: james
/// studentsurname: smith
/// studentfirstname: sally
/// studentsurname: jones
/// </pre>
///
/// A SpanNearQuery with a slop of 0 can be applied across two {@link SpanTermQuery} objects as follows:
///
/// <pre>
/// SpanQueryPtr q1 = newLucene<SpanTermQuery>(newLucene<Term>(L"studentfirstname", L"james"));
/// SpanQueryPtr q2 = newLucene<SpanTermQuery>(newLucene<Term>(L"studentsurname", L"jones"));
/// SpanQueryPtr q2m = newLucene<FieldMaskingSpanQuery>(q2, L"studentfirstname");
///
/// Collection<SpanQueryPtr> span = newCollection<SpanQueryPtr>(q1, q1);
///
/// QueryPtr q = newLucene<SpanNearQuery>(span, -1, false);
/// </pre>
/// to search for 'studentfirstname:james studentsurname:jones' and find teacherid 1 without matching
/// teacherid 2 (which has a 'james' in position 0 and 'jones' in position 1).
///
/// Note: as {@link #getField()} returns the masked field, scoring will be done using the norms of the
/// field name supplied. This may lead to unexpected scoring behaviour.
class FieldMaskingSpanQuery : public SpanQuery {
public:
    FieldMaskingSpanQuery(const SpanQueryPtr& maskedQuery, const String& maskedField);
    virtual ~FieldMaskingSpanQuery();

    LUCENE_CLASS(FieldMaskingSpanQuery);

protected:
    SpanQueryPtr maskedQuery;
    String field;

public:
    using SpanQuery::toString;

    virtual String getField();
    SpanQueryPtr getMaskedQuery();
    virtual SpansPtr getSpans(const IndexReaderPtr& reader);
    virtual void extractTerms(SetTerm terms);
    virtual WeightPtr createWeight(const SearcherPtr& searcher);
    virtual SimilarityPtr getSimilarity(const SearcherPtr& searcher);
    virtual QueryPtr rewrite(const IndexReaderPtr& reader);

    virtual String toString(const String& field);
    virtual bool equals(const LuceneObjectPtr& other);
    virtual int32_t hashCode();

    /// Returns a clone of this query.
    virtual LuceneObjectPtr clone(const LuceneObjectPtr& other = LuceneObjectPtr());
};

}

#endif
