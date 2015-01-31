/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef FIELDSELECTOR_H
#define FIELDSELECTOR_H

#include "fnord-fts/util/LuceneObject.h"

namespace fnord {
namespace fts {

/// The FieldSelector allows one to make decisions about what Fields get loaded on a {@link Document} by
/// {@link IndexReader#document(int32_t, FieldSelector)}
class FieldSelector : public LuceneObject {
protected:
    FieldSelector();

public:
    virtual ~FieldSelector();

    LUCENE_CLASS(FieldSelector);

public:
    /// Provides information about what should be done with this Field
    enum FieldSelectorResult {
        /// Null value
        SELECTOR_NULL,

        /// Load this {@link Field} every time the {@link Document} is loaded, reading in the data as it is
        /// encountered. {@link Document#getField(String)} and {@link Document#getFieldable(String)} should
        /// not return null.
        /// {@link Document#add(Fieldable)} should be called by the Reader.
        SELECTOR_LOAD,

        /// Lazily load this {@link Field}.  This means the {@link Field} is valid, but it may not actually
        /// contain its data until invoked.  {@link Document#getField(String)} SHOULD NOT BE USED.  {@link
        /// Document#getFieldable(String)} is safe to use and should return a valid instance of a {@link
        /// Fieldable}.
        /// {@link Document#add(Fieldable)} should be called by the Reader.
        SELECTOR_LAZY_LOAD,

        /// Do not load the {@link Field}.  {@link Document#getField(String)} and {@link
        /// Document#getFieldable(String)} should return null.  {@link Document#add(Fieldable)} is not called.
        /// {@link Document#add(Fieldable)} should not be called by the Reader.
        SELECTOR_NO_LOAD,

        /// Load this field as in the {@link #LOAD} case, but immediately return from {@link Field} loading
        /// for the {@link Document}.  Thus, the Document may not have its complete set of Fields.  {@link
        /// Document#getField(String)} and {@link Document#getFieldable(String)} should both be valid for
        /// this {@link Field}
        /// {@link Document#add(Fieldable)} should be called by the Reader.
        SELECTOR_LOAD_AND_BREAK,

        /// Load the size of this {@link Field} rather than its value.  Size is measured as number of bytes
        /// required to store the field == bytes for a binary or any compressed value, and 2*chars for a String
        /// value.  The size is stored as a binary value, represented as an int in a byte[], with the higher
        /// order byte first in [0]
        SELECTOR_SIZE,

        /// Like {@link #SIZE} but immediately break from the field loading loop, i.e., stop loading further
        /// fields, after the size is loaded
        SELECTOR_SIZE_AND_BREAK
    };

public:
    virtual FieldSelectorResult accept(const String& fieldName) = 0;
};

}

}
#endif
