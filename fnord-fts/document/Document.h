/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef DOCUMENT_H
#define DOCUMENT_H

#include "fnord-fts/util/LuceneObject.h"

namespace Lucene {

/// Documents are the unit of indexing and search.
///
/// A Document is a set of fields.  Each field has a name and a textual value.  A field may be {@link
/// Fieldable#isStored() stored} with the document, in which case it is returned with search hits on the
/// document.  Thus each document should typically contain one or more stored fields which uniquely
/// identify it.
///
/// Note that fields which are not {@link Fieldable#isStored() stored} are not available in documents
/// retrieved from the index, eg. with {@link ScoreDoc#doc}, {@link Searcher#doc(int)} or {@link
/// IndexReader#document(int)}.
class Document : public LuceneObject {
public:
    /// Constructs a new document with no fields.
    Document();

    virtual ~Document();

    LUCENE_CLASS(Document);

protected:
    Collection<FieldablePtr> fields;
    double boost;

public:
    /// Sets a boost factor for hits on any field of this document.  This value will be multiplied into the
    /// score of all hits on this document.
    ///
    /// The default value is 1.0.
    ///
    /// Values are multiplied into the value of {@link Fieldable#getBoost()} of each field in this document.
    /// Thus, this method in effect sets a default boost for the fields of this document.
    ///
    /// @see Fieldable#setBoost(double)
    void setBoost(double boost);

    /// Returns, at indexing time, the boost factor as set by {@link #setBoost(double)}.
    ///
    /// Note that once a document is indexed this value is no longer available from the index.  At search time,
    /// for retrieved documents, this method always returns 1. This however does not mean that the boost value
    /// set at  indexing  time was ignored - it was just combined with other indexing time factors and stored
    /// elsewhere, for better indexing and search performance. (For more information see the "norm(t,d)" part
    /// of the scoring formula in {@link Similarity}.)
    ///
    /// @see #setBoost(double)
    double getBoost();

    /// Adds a field to a document.  Several fields may be added with the same name.  In this case, if the fields
    /// are indexed, their text is treated as though appended for the purposes of search.
    ///
    /// Note that add like the removeField(s) methods only makes sense prior to adding a document to an index.
    /// These methods cannot be used to change the content of an existing index! In order to achieve this, a
    /// document has to be deleted from an index and a new changed version of that document has to be added.
    void add(const FieldablePtr& field);

    /// Removes field with the specified name from the document.  If multiple fields exist with this name, this
    /// method removes the first field that has been added.  If there is no field with the specified name, the
    /// document remains unchanged.
    ///
    /// Note that the removeField(s) methods like the add method only make sense  prior to adding a document to
    /// an index. These methods cannot be used to change the content of an existing index!  In order to achieve
    /// this, a document has to be deleted from an index and a new changed version of that document has to be added.
    void removeField(const String& name);

    /// Removes all fields with the given name from the document.  If there is no field with the specified name,
    /// the document remains unchanged.
    ///
    /// Note that the removeField(s) methods like the add method only make sense prior to adding a document to an
    /// index. These methods cannot be used to change the content of an existing index! In order to achieve this,
    /// a document has to be deleted from an index and a new changed version of that document has to be added.
    void removeFields(const String& name);

    /// Returns a field with the given name if any exist in this document, or null.  If multiple fields exists with
    /// this name, this method returns the first value added.
    /// Do not use this method with lazy loaded fields.
    FieldPtr getField(const String& name);

    /// Returns a field with the given name if any exist in this document, or null.  If multiple fields exists with
    /// this name, this method returns the first value added.
    FieldablePtr getFieldable(const String& name);

    /// Returns the string value of the field with the given name if any exist in this document, or null.  If multiple
    /// fields exist with this name, this method returns the first value added. If only binary fields with this name
    /// exist, returns null.
    String get(const String& name);

    /// Returns a List of all the fields in a document.
    ///
    /// Note that fields which are <i>not</i> {@link Fieldable#isStored() stored} are not available in documents
    /// retrieved from the index, eg. {@link Searcher#doc(int)} or {@link IndexReader#document(int)}.
    Collection<FieldablePtr> getFields();

    /// Returns an array of {@link Field}s with the given name.  Do not use with lazy loaded fields.  This method
    /// returns an empty array when there are no matching fields.  It never returns null.
    /// @param name the name of the field
    /// @return a Field[] array
    Collection<FieldPtr> getFields(const String& name);

    /// Returns an array of {@link Fieldable}s with the given name.
    /// This method returns an empty array when there are no matching fields.  It never returns null.
    /// @param name the name of the field
    /// @return a Fieldable[] array
    Collection<FieldablePtr> getFieldables(const String& name);

    /// Returns an array of values of the field specified as the method parameter.
    /// This method returns an empty array when there are no matching fields.  It never returns null.
    /// @param name the name of the field
    /// @return a String[] of field values
    Collection<String> getValues(const String& name);

    /// Returns an array of byte arrays for of the fields that have the name specified as the method parameter.
    /// This method returns an empty array when there are no matching fields.  It never returns null.
    /// @param name the name of the field
    /// @return a byte[][] of binary field values
    Collection<ByteArray> getBinaryValues(const String& name);

    /// Returns an array of bytes for the first (or only) field that has the name specified as the method parameter.
    /// This method will return null if no binary fields with the specified name are available.  There may be
    /// non-binary fields with the same name.
    /// @param name the name of the field.
    /// @return a byte[] containing the binary field value or null
    ByteArray getBinaryValue(const String& name);

    /// Returns a string representation of the object
    virtual String toString();
};

}

#endif
