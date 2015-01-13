/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef NUMERICTOKENSTREAM_H
#define NUMERICTOKENSTREAM_H

#include "fnord-fts/analysis/TokenStream.h"

namespace Lucene {

/// This class provides a {@link TokenStream} for indexing numeric values that can be used by {@link NumericRangeQuery}
/// or {@link NumericRangeFilter}.
///
/// Note that for simple usage, {@link NumericField} is recommended.  {@link NumericField} disables norms and term freqs,
/// as they are not usually needed during searching.  If you need to change these settings, you should use this class.
///
/// See {@link NumericField} for capabilities of fields indexed numerically.
///
/// Here's an example usage, for an int field:
///
/// FieldPtr field = newLucene<Field>(name, newLucene<NumericTokenStream>(precisionStep)->setIntValue(value));
/// field->setOmitNorms(true);
/// field->setOmitTermFreqAndPositions(true);
/// document->add(field);
///
/// For optimal performance, re-use the TokenStream and Field instance for more than one document:
///
/// NumericTokenStreamPtr stream = newLucene<NumericTokenStream>(precisionStep);
/// FieldPtr field = newLucene<Field>(name, stream);
/// field->setOmitNorms(true);
/// field->setOmitTermFreqAndPositions(true);
/// DocumentPtr document = newLucene<Document>();
/// document->add(field);
///
/// for (all documents)
/// {
///     stream->setIntValue(value);
///     writer->addDocument(document);
/// }
///
/// This stream is not intended to be used in analyzers; it's more for iterating the different precisions during
/// indexing a specific numeric value.
///
/// NOTE: as token streams are only consumed once the document is added to the index, if you index more than one
/// numeric field, use a separate NumericTokenStream * instance for each.
///
/// See {@link NumericRangeQuery} for more details on the <a href="../search/NumericRangeQuery.html#precisionStepDesc">precisionStep</a>
/// parameter as well as how numeric fields work under the hood.
class NumericTokenStream : public TokenStream {
public:
    /// Creates a token stream for numeric values using the default precisionStep {@link
    /// NumericUtils#PRECISION_STEP_DEFAULT} (4). The stream is not yet initialized, before using set a
    /// value using the various setValue() methods.
    NumericTokenStream();

    /// Creates a token stream for numeric values with the specified precisionStep. The stream is not yet
    /// initialized, before using set a value using the various setValue() methods.
    NumericTokenStream(int32_t precisionStep);

    /// Creates a token stream for numeric values with the specified precisionStep using the given {@link
    /// AttributeSource}.  The stream is not yet initialized, before using set a value using the various
    /// setValue() methods.
    NumericTokenStream(const AttributeSourcePtr& source, int32_t precisionStep);

    /// Creates a token stream for numeric values with the specified precisionStep using the given {@link
    /// AttributeFactory}.  The stream is not yet initialized, before using set a value using the various
    /// setValue() methods.
    NumericTokenStream(const AttributeFactoryPtr& factory, int32_t precisionStep);

    virtual ~NumericTokenStream();

    LUCENE_CLASS(NumericTokenStream);

protected:
    TermAttributePtr termAtt;
    TypeAttributePtr typeAtt;
    PositionIncrementAttributePtr posIncrAtt;

    int32_t shift;
    int32_t valSize; // valSize == 0 means not initialized
    int32_t precisionStep;

    int64_t value;

public:
    /// The full precision token gets this token type assigned.
    static const String& TOKEN_TYPE_FULL_PREC();

    /// The lower precision tokens gets this token type assigned.
    static const String& TOKEN_TYPE_LOWER_PREC();

    /// Initializes the token stream with the supplied long value.
    /// @param value the value, for which this TokenStream should enumerate tokens.
    /// @return this instance, because of this you can use it the following way:
    /// newLucene<Field>(name, newLucene<NumericTokenStream>(precisionStep)->setLongValue(value))
    NumericTokenStreamPtr setLongValue(int64_t value);

    /// Initializes the token stream with the supplied int value.
    /// @param value the value, for which this TokenStream should enumerate tokens.
    /// @return this instance, because of this you can use it the following way:
    /// newLucene<Field>(name, newLucene<NumericTokenStream>(precisionStep)->setIntValue(value))
    NumericTokenStreamPtr setIntValue(int32_t value);

    /// Initializes the token stream with the supplied double value.
    /// @param value the value, for which this TokenStream should enumerate tokens.
    /// @return this instance, because of this you can use it the following way:
    /// newLucene<Field>(name, newLucene<NumericTokenStream>(precisionStep)->setDoubleValue(value))
    NumericTokenStreamPtr setDoubleValue(double value);

    virtual void reset();
    virtual bool incrementToken();
    virtual String toString();
};

}

#endif
