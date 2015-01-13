/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef NEARSPANSORDERED_H
#define NEARSPANSORDERED_H

#include "Spans.h"

namespace Lucene {

/// A Spans that is formed from the ordered subspans of a SpanNearQuery where the subspans do not overlap
/// and have a maximum slop between them.
///
/// The formed spans only contains minimum slop matches.  The matching slop is computed from the distance(s)
/// between the non overlapping matching Spans.
///
/// Successive matches are always formed from the successive Spans of the SpanNearQuery.
///
/// The formed spans may contain overlaps when the slop is at least 1.  For example, when querying using
/// <pre>t1 t2 t3</pre>
/// with slop at least 1, the fragment:
/// <pre>t1 t2 t1 t3 t2 t3</pre>
/// matches twice:
/// <pre>t1 t2 .. t3      </pre>
/// <pre>      t1 .. t2 t3</pre>
///
/// Note: Only public for subclassing.  Most implementations should not need this class
class NearSpansOrdered : public Spans {
public:
    NearSpansOrdered(const SpanNearQueryPtr& spanNearQuery, const IndexReaderPtr& reader, bool collectPayloads = true);
    virtual ~NearSpansOrdered();

    LUCENE_CLASS(NearSpansOrdered);

protected:
    int32_t allowedSlop;
    bool firstTime;
    bool more;

    /// The spans in the same order as the SpanNearQuery
    Collection<SpansPtr> subSpans;

    /// Indicates that all subSpans have same doc()
    bool inSameDoc;

    int32_t matchDoc;
    int32_t matchStart;
    int32_t matchEnd;
    Collection<ByteArray> matchPayload;

    Collection<SpansPtr> subSpansByDoc;
    SpanNearQueryPtr query;
    bool collectPayloads;

public:
    virtual int32_t doc();
    virtual int32_t start();
    virtual int32_t end();

    Collection<SpansPtr> getSubSpans();

    virtual Collection<ByteArray> getPayload();
    virtual bool isPayloadAvailable();
    virtual bool next();
    virtual bool skipTo(int32_t target);

    /// Check whether two Spans in the same document are ordered.
    /// @return true if spans1 starts before spans2 or the spans start at the same position, and
    /// spans1 ends before spans2.
    static bool docSpansOrdered(const SpansPtr& spans1, const SpansPtr& spans2);

    virtual String toString();

protected:
    /// Advances the subSpans to just after an ordered match with a minimum slop that is smaller than the
    /// slop allowed by the SpanNearQuery.
    /// @return true if there is such a match.
    bool advanceAfterOrdered();

    /// Advance the subSpans to the same document.
    bool toSameDoc();

    // Like {@link #docSpansOrdered(SpansPtr, SpansPtr)}, but use the spans starts and ends as parameters.
    static bool docSpansOrdered(int32_t start1, int32_t end1, int32_t start2, int32_t end2);

    /// Order the subSpans within the same document by advancing all later spans after the previous one.
    bool stretchToOrder();

    /// The subSpans are ordered in the same doc, so there is a possible match.  Compute the slop while
    /// making the match as short as possible by advancing all subSpans except the last one in reverse order.
    bool shrinkToAfterShortestMatch();
};

}

#endif
