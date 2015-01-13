/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef FIELDABLE_H
#define FIELDABLE_H

#include "fnord-fts/util/LuceneObject.h"

namespace Lucene {

/// Synonymous with {@link Field}.
///
/// WARNING: This interface may change within minor versions, despite Lucene's backward compatibility
/// requirements.  This means new methods may be added from version to version.  This change only
/// affects the Fieldable API; other backwards compatibility promises remain intact. For example, Lucene
/// can still read and write indices created within the same major version.
class Fieldable {
public:
    LUCENE_INTERFACE(Fieldable);
    virtual ~Fieldable() {}

public:
    /// Sets the boost factor hits on this field.  This value will be multiplied into the score of all
    /// hits on this this field of this document.
    ///
    /// The boost is multiplied by {@link Document#getBoost()} of the document containing this field.
    /// If a document has multiple fields with the same name, all such values are multiplied together.
    /// This product is then used to compute the norm factor for the field.  By default, in the {@link
    /// Similarity#computeNorm(String, FieldInvertState)} method, the boost value is multiplied by the
    /// {@link Similarity#lengthNorm(String,int)} and then rounded by {@link Similarity#encodeNorm(double)}
    /// before it is stored in the index.  One should attempt to ensure that this product does not overflow
    /// the range of that encoding.
    ///
    /// @see Document#setBoost(double)
    /// @see Similarity#computeNorm(String, FieldInvertState)
    /// @see Similarity#encodeNorm(double)
    virtual void setBoost(double boost) = 0;

    /// Returns the boost factor for hits for this field.
    ///
    /// The default value is 1.0.
    ///
    /// Note: this value is not stored directly with the document in the index. Documents returned from
    /// {@link IndexReader#document(int)} and {@link Searcher#doc(int)} may thus not have the same value
    /// present as when this field was indexed.
    virtual double getBoost() = 0;

    /// Returns the name of the field as an interned string. For example "date", "title", "body", ...
    virtual String name() = 0;

    /// The value of the field as a String, or empty.
    ///
    /// For indexing, if isStored()==true, the stringValue() will be used as the stored field value
    /// unless isBinary()==true, in which case getBinaryValue() will be used.
    ///
    /// If isIndexed()==true and isTokenized()==false, this String value will be indexed as a single token.
    /// If isIndexed()==true and isTokenized()==true, then tokenStreamValue() will be used to generate
    /// indexed tokens if not null, else readerValue() will be used to generate indexed tokens if not null,
    /// else stringValue() will be used to generate tokens.
    virtual String stringValue() = 0;

    /// The value of the field as a Reader, which can be used at index time to generate indexed tokens.
    /// @see #stringValue()
    virtual ReaderPtr readerValue() = 0;

    /// The TokenStream for this field to be used when indexing, or null.
    /// @see #stringValue()
    virtual TokenStreamPtr tokenStreamValue() = 0;

    /// True if the value of the field is to be stored in the index for return with search hits.
    virtual bool isStored() = 0;

    /// True if the value of the field is to be indexed, so that it may be searched on.
    virtual bool isIndexed() = 0;

    /// True if the value of the field should be tokenized as text prior to indexing.  Un-tokenized fields
    /// are indexed as a single word and may not be Reader-valued.
    virtual bool isTokenized() = 0;

    /// True if the term or terms used to index this field are stored as a term vector, available from
    /// {@link IndexReader#getTermFreqVector(int,String)}.  These methods do not provide access to the
    /// original content of the field, only to terms used to index it. If the original content must be
    /// preserved, use the stored attribute instead.
    virtual bool isTermVectorStored() = 0;

    /// True if terms are stored as term vector together with their offsets (start and end position in
    /// source text).
    virtual bool isStoreOffsetWithTermVector() = 0;

    /// True if terms are stored as term vector together with their token positions.
    virtual bool isStorePositionWithTermVector() = 0;

    /// True if the value of the field is stored as binary.
    virtual bool isBinary() = 0;

    /// True if norms are omitted for this indexed field.
    virtual bool getOmitNorms() = 0;

    /// If set, omit normalization factors associated with this indexed field.
    /// This effectively disables indexing boosts and length normalization for this field.
    virtual void setOmitNorms(bool omitNorms) = 0;

    /// Indicates whether a Field is Lazy or not.  The semantics of Lazy loading are such that if a Field
    /// is lazily loaded, retrieving it's values via {@link #stringValue()} or {@link #getBinaryValue()}
    /// is only valid as long as the {@link IndexReader} that retrieved the {@link Document} is still open.
    ///
    /// @return true if this field can be loaded lazily
    virtual bool isLazy() = 0;

    /// Returns offset into byte[] segment that is used as value, if Field is not binary returned value is
    /// undefined.
    /// @return index of the first character in byte[] segment that represents this Field value.
    virtual int32_t getBinaryOffset() = 0;

    /// Returns length of byte[] segment that is used as value, if Field is not binary returned value is
    /// undefined.
    /// @return length of byte[] segment that represents this Field value.
    virtual int32_t getBinaryLength() = 0;

    /// Return the raw byte[] for the binary field.  Note that you must also call {@link #getBinaryLength}
    /// and {@link #getBinaryOffset} to know which range of bytes in this returned array belong to the field.
    /// @return reference to the Field value as byte[].
    virtual ByteArray getBinaryValue() = 0;

    /// Return the raw byte[] for the binary field.  Note that you must also call {@link #getBinaryLength}
    /// and {@link #getBinaryOffset} to know which range of bytes in this returned array belong to the field.
    ///
    /// About reuse: if you pass in the result byte[] and it is used, likely the underlying implementation will
    /// hold onto this byte[] and return it in future calls to {@link #getBinaryValue()}.  So if you subsequently
    /// re-use the same byte[] elsewhere it will alter this Fieldable's value.
    /// @param result  User defined buffer that will be used if possible.  If this is null or not large enough,
    /// a new buffer is allocated
    /// @return reference to the Field value as byte[].
    virtual ByteArray getBinaryValue(ByteArray result) = 0;

    /// @see #setOmitTermFreqAndPositions
    virtual bool getOmitTermFreqAndPositions() = 0;

    /// If set, omit term freq, positions and payloads from postings for this field.
    ///
    /// NOTE: While this option reduces storage space required in the index, it also means any query requiring
    /// positional information, such as {@link PhraseQuery} or {@link SpanQuery} subclasses will silently fail
    /// to find results.
    virtual void setOmitTermFreqAndPositions(bool omitTermFreqAndPositions) = 0;
};

}

#endif
