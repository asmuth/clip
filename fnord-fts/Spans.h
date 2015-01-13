/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef SPANS_H
#define SPANS_H

#include "LuceneObject.h"

namespace Lucene {

/// An enumeration of span matches.  Used to implement span searching.  Each span represents a range of term
/// positions within a document.  Matches are enumerated in order, by increasing document number, within that
/// by increasing start position and finally by increasing end position.
class Spans : public LuceneObject {
public:
    virtual ~Spans();
    LUCENE_CLASS(Spans);

public:
    /// Move to the next match, returning true if any such exists.
    virtual bool next() = 0;

    /// Skips to the first match beyond the current, whose document number is greater than or equal to target.
    ///
    /// Returns true if there is such a match.
    ///
    /// Behaves as if written:
    /// <pre>
    /// bool skipTo(int32_t target)
    /// {
    ///     do
    ///     {
    ///         if (!next())
    ///             return false;
    ///     }
    ///     while (target > doc());
    ///     return true;
    /// }
    /// </pre>
    /// Most implementations are considerably more efficient than that.
    virtual bool skipTo(int32_t target) = 0;

    /// Returns the document number of the current match.  Initially invalid.
    virtual int32_t doc() = 0;

    /// Returns the start position of the current match.  Initially invalid.
    virtual int32_t start() = 0;

    /// Returns the end position of the current match.  Initially invalid.
    virtual int32_t end() = 0;

    /// Returns the payload data for the current span.  This is invalid until {@link #next()} is called for the
    /// first time.  This method must not be called more than once after each call of {@link #next()}. However,
    /// most payloads are loaded lazily, so if the payload data for the current position is not needed, this
    /// method may not be called at all for performance reasons.  An ordered SpanQuery does not lazy load, so
    /// if you have payloads in your index and you do not want ordered SpanNearQuerys to collect payloads, you
    /// can disable collection with a constructor option.
    ///
    /// Note that the return type is a collection, thus the ordering should not be relied upon.
    ///
    /// @return a List of byte arrays containing the data of this payload, otherwise null if isPayloadAvailable
    /// is false
    virtual Collection<ByteArray> getPayload() = 0;

    /// Checks if a payload can be loaded at this position.
    ///
    /// Payloads can only be loaded once per call to {@link #next()}.
    ///
    /// @return true if there is a payload available at this position that can be loaded
    virtual bool isPayloadAvailable() = 0;
};

}

#endif
