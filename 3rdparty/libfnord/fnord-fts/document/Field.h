/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef FIELD_H
#define FIELD_H

#include "fnord-fts/document/AbstractField.h"

namespace fnord {
namespace fts {

class Field : public AbstractField {
public:
    /// Create a field by specifying its name, value and how it will be saved in the index. Term vectors
    /// will not be stored in the index.
    ///
    /// @param name The name of the field
    /// @param value The string to process
    /// @param store Whether value should be stored in the index
    /// @param index Whether the field should be indexed, and if so, if it should be tokenized before indexing
    Field(const String& name, const String& value, Store store, Index index);

    /// Create a field by specifying its name, value and how it will be saved in the index.
    ///
    /// @param name The name of the field
    /// @param value The string to process
    /// @param store Whether value should be stored in the index
    /// @param index Whether the field should be indexed, and if so, if it should be tokenized before indexing
    /// @param termVector Whether term vector should be stored
    Field(const String& name, const String& value, Store store, Index index, TermVector termVector);

    /// Create a tokenized and indexed field that is not stored. Term vectors will not be stored.  The Reader is
    /// read only when the Document is added to the index, ie. you may not close the Reader until {@link
    /// IndexWriter#addDocument(Document)} has been called.
    ///
    /// @param name The name of the field
    /// @param reader The reader with the content
    Field(const String& name, const ReaderPtr& reader);

    /// Create a tokenized and indexed field that is not stored, optionally with storing term vectors.  The
    /// Reader is read only when the Document is added to the index, ie. you may not close the Reader until
    /// {@link IndexWriter#addDocument(Document)} has been called.
    ///
    /// @param name The name of the field
    /// @param reader The reader with the content
    /// @param termVector Whether term vector should be stored
    Field(const String& name, const ReaderPtr& reader, TermVector termVector);

    /// Create a tokenized and indexed field that is not stored. Term vectors will not be stored. This is useful
    /// for pre-analyzed fields.  The TokenStream is read only when the Document is added to the index, ie. you
    /// may not close the TokenStream until {@link IndexWriter#addDocument(Document)} has been called.
    ///
    /// @param name The name of the field
    /// @param tokenStream The TokenStream with the content
    Field(const String& name, const TokenStreamPtr& tokenStream);

    /// Create a tokenized and indexed field that is not stored, optionally with storing term vectors.  This is
    /// useful for pre-analyzed fields.  The TokenStream is read only when the Document is added to the index,
    /// ie. you may not close the TokenStream until {@link IndexWriter#addDocument(Document)} has been called.
    ///
    /// @param name The name of the field
    /// @param tokenStream The TokenStream with the content
    /// @param termVector Whether term vector should be stored
    Field(const String& name, const TokenStreamPtr& tokenStream, TermVector termVector);

    /// Create a stored field with binary value. Optionally the value may be compressed.
    ///
    /// @param name The name of the field
    /// @param value The binary value
    /// @param store How value should be stored (compressed or not)
    Field(const String& name, ByteArray value, Store store);

    /// Create a stored field with binary value. Optionally the value may be compressed.
    ///
    /// @param name The name of the field
    /// @param value The binary value
    /// @param offset Starting offset in value where this Field's bytes are
    /// @param length Number of bytes to use for this Field, starting at offset
    /// @param store How value should be stored (compressed or not)
    Field(const String& name, ByteArray value, int32_t offset, int32_t length, Store store);

    virtual ~Field();

    LUCENE_CLASS(Field);

public:
    using AbstractField::isStored;
    using AbstractField::isIndexed;

    /// Specifies whether and how a field should be stored.
    static bool isStored(Store store);

    /// Specifies whether and how a field should be indexed.
    static bool isIndexed(Index index);
    static bool isAnalyzed(Index index);
    static bool omitNorms(Index index);

    /// Get the best representation of the index given the flags.
    static Field::Index toIndex(bool indexed, bool analyzed);

    /// Get the best representation of the index given the flags.
    static Field::Index toIndex(bool indexed, bool analyzed, bool omitNorms);

    /// Specifies whether and how a field should have term vectors.
    static bool isStored(TermVector termVector);
    static bool withPositions(TermVector termVector);
    static bool withOffsets(TermVector termVector);

    /// Get the best representation of the index given the flags.
    static Field::TermVector toTermVector(bool stored, bool withOffsets, bool withPositions);

    /// The value of the field as a String, or null.  If null, the Reader value or binary value is used.
    /// Exactly one of stringValue(), readerValue(), and getBinaryValue() must be set.
    virtual String stringValue();

    /// The value of the field as a Reader, or null.  If null, the String value or binary value is used.
    /// Exactly one of stringValue(), readerValue(), and getBinaryValue() must be set.
    virtual ReaderPtr readerValue();

    /// The value of the field as a TokesStream, or null.  If null, the Reader value or String value is
    /// analyzed to produce the indexed tokens.
    virtual TokenStreamPtr tokenStreamValue();

    /// Change the value of this field.  This can be used during indexing to re-use a single Field instance
    /// to improve indexing speed.  Typically a single {@link Document} instance is re-used as well.  This
    /// helps most on small documents.
    ///
    /// Each Field instance should only be used once within a single {@link Document} instance.
    virtual void setValue(const String& value);

    /// Change the value of this field.
    virtual void setValue(const ReaderPtr& value);

    /// Change the value of this field.
    virtual void setValue(ByteArray value);

    /// Change the value of this field.
    virtual void setValue(ByteArray value, int32_t offset, int32_t length);

    /// Sets the token stream to be used for indexing and causes isIndexed() and isTokenized() to return
    /// true.  May be combined with stored values from stringValue() or getBinaryValue()
    virtual void setTokenStream(const TokenStreamPtr& tokenStream);

protected:
    void ConstructField(const String& name, const String& value, Store store, Index index, TermVector termVector);
    void ConstructField(const String& name, const ReaderPtr& reader, TermVector termVector);
    void ConstructField(const String& name, const TokenStreamPtr& tokenStream, TermVector termVector);
    void ConstructField(const String& name, ByteArray value, int32_t offset, int32_t length, Store store);
};

}

}
#endif
