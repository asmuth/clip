/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef ABSTRACTFIELD_H
#define ABSTRACTFIELD_H

#include "fnord-fts/document/Fieldable.h"

namespace fnord {
namespace fts {

class AbstractField : public Fieldable, public LuceneObject {
public:
    /// Specifies whether and how a field should be stored.
    enum Store {
        /// Store the original field value in the index. This is useful for short texts like a document's title
        /// which should be displayed with the results. The value is stored in its original form, ie. no analyzer
        /// is used before it is stored.
        STORE_YES,

        /// Do not store the field value in the index.
        STORE_NO
    };

    /// Specifies whether and how a field should be indexed.
    enum Index {
        /// Do not index the field value. This field can thus not be searched, but one can still access its
        /// contents provided it is {@link Field.Store stored}.
        INDEX_NO,

        /// Index the tokens produced by running the field's value through an Analyzer.  This is useful for
        /// common text.
        INDEX_ANALYZED,

        /// Index the field's value without using an Analyzer, so it can be searched.  As no analyzer is used
        /// the value will be stored as a single term. This is useful for unique Ids like product numbers.
        INDEX_NOT_ANALYZED,

        /// Index the field's value without an Analyzer, and also disable the storing of norms.  Note that you
        /// can also separately enable/disable norms by calling {@link Field#setOmitNorms}.  No norms means
        /// that index-time field and document boosting and field length normalization are disabled.  The benefit
        /// is less memory usage as norms take up one byte of RAM per indexed field for every document in the
        /// index, during searching.  Note that once you index a given field with norms enabled, disabling norms
        /// will have no effect.  In other words, for this to have the above described effect on a field, all
        /// instances of that field must be indexed with NOT_ANALYZED_NO_NORMS from the beginning.
        INDEX_NOT_ANALYZED_NO_NORMS,

        /// Index the tokens produced by running the field's value through an Analyzer, and also separately
        /// disable the  storing of norms.  See {@link #NOT_ANALYZED_NO_NORMS} for what norms are and why you
        /// may want to disable them.
        INDEX_ANALYZED_NO_NORMS
    };

    /// Specifies whether and how a field should have term vectors.
    enum TermVector {
        /// Do not store term vectors.
        TERM_VECTOR_NO,

        /// Store the term vectors of each document. A term vector is a list of the document's terms and their
        /// number of occurrences in that document.
        TERM_VECTOR_YES,

        /// Store the term vector + token position information
        /// @see #YES
        TERM_VECTOR_WITH_POSITIONS,

        /// Store the term vector + token offset information
        /// @see #YES
        TERM_VECTOR_WITH_OFFSETS,

        /// Store the term vector + token position and offset information
        /// @see #YES
        /// @see #WITH_POSITIONS
        /// @see #WITH_OFFSETS
        TERM_VECTOR_WITH_POSITIONS_OFFSETS
    };

public:
    virtual ~AbstractField();

    LUCENE_CLASS(AbstractField);

protected:
    AbstractField();
    AbstractField(const String& name, Store store, Index index, TermVector termVector);

    String _name;
    bool storeTermVector;
    bool storeOffsetWithTermVector;
    bool storePositionWithTermVector;
    bool _omitNorms;
    bool _isStored;
    bool _isIndexed;
    bool _isTokenized;
    bool _isBinary;
    bool lazy;
    bool omitTermFreqAndPositions;
    double boost;

    // the data object for all different kind of field values
    FieldsData fieldsData;

    // pre-analyzed tokenStream for indexed fields
    TokenStreamPtr tokenStream;

    // length/offset for all primitive types
    int32_t binaryLength;
    int32_t binaryOffset;

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
    virtual void setBoost(double boost);

    /// Returns the boost factor for hits for this field.
    ///
    /// The default value is 1.0.
    ///
    /// Note: this value is not stored directly with the document in the index. Documents returned from
    /// {@link IndexReader#document(int)} and {@link Searcher#doc(int)} may thus not have the same value
    /// present as when this field was indexed.
    virtual double getBoost();

    /// Returns the name of the field as an interned string. For example "date", "title", "body", ...
    virtual String name();

    /// True if the value of the field is to be stored in the index for return with search hits. It is an
    /// error for this to be true if a field is Reader-valued.
    virtual bool isStored();

    /// True if the value of the field is to be indexed, so that it may be searched on.
    virtual bool isIndexed();

    /// True if the value of the field should be tokenized as text prior to indexing.  Un-tokenized fields
    /// are indexed as a single word and may not be Reader-valued.
    virtual bool isTokenized();

    /// True if the term or terms used to index this field are stored as a term vector, available from
    /// {@link IndexReader#getTermFreqVector(int,String)}.  These methods do not provide access to the
    /// original content of the field, only to terms used to index it. If the original content must be
    /// preserved, use the stored attribute instead.
    virtual bool isTermVectorStored();

    /// True if terms are stored as term vector together with their offsets (start and end position in
    /// source text).
    virtual bool isStoreOffsetWithTermVector();

    /// True if terms are stored as term vector together with their token positions.
    virtual bool isStorePositionWithTermVector();

    /// True if the value of the field is stored as binary.
    virtual bool isBinary();

    /// Return the raw byte[] for the binary field.  Note that you must also call {@link #getBinaryLength}
    /// and {@link #getBinaryOffset} to know which range of bytes in this returned array belong to the field.
    /// @return reference to the Field value as byte[].
    virtual ByteArray getBinaryValue();

    /// Return the raw byte[] for the binary field.  Note that you must also call {@link #getBinaryLength}
    /// and {@link #getBinaryOffset} to know which range of bytes in this returned array belong to the field.
    /// @return reference to the Field value as byte[].
    virtual ByteArray getBinaryValue(ByteArray result);

    /// Returns length of byte[] segment that is used as value, if Field is not binary returned value is
    /// undefined.
    /// @return length of byte[] segment that represents this Field value.
    virtual int32_t getBinaryLength();

    /// Returns offset into byte[] segment that is used as value, if Field is not binary returned value is
    /// undefined.
    /// @return index of the first character in byte[] segment that represents this Field value.
    virtual int32_t getBinaryOffset();

    /// True if norms are omitted for this indexed field.
    virtual bool getOmitNorms();

    /// @see #setOmitTermFreqAndPositions
    virtual bool getOmitTermFreqAndPositions();

    /// If set, omit normalization factors associated with this indexed field.
    /// This effectively disables indexing boosts and length normalization for this field.
    virtual void setOmitNorms(bool omitNorms);

    /// If set, omit term freq, positions and payloads from postings for this field.
    ///
    /// NOTE: While this option reduces storage space required in the index, it also means any query requiring
    /// positional information, such as {@link PhraseQuery} or {@link SpanQuery} subclasses will silently fail
    /// to find results.
    virtual void setOmitTermFreqAndPositions(bool omitTermFreqAndPositions);

    /// Indicates whether a Field is Lazy or not.  The semantics of Lazy loading are such that if a Field
    /// is lazily loaded, retrieving it's values via {@link #stringValue()} or {@link #getBinaryValue()}
    /// is only valid as long as the {@link IndexReader} that retrieved the {@link Document} is still open.
    ///
    /// @return true if this field can be loaded lazily
    virtual bool isLazy();

    /// Prints a Field for human consumption.
    virtual String toString();

protected:
    void setStoreTermVector(TermVector termVector);
};

}

}
#endif
