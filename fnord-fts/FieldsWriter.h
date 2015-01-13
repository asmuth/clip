/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef FIELDSWRITER_H
#define FIELDSWRITER_H

#include "LuceneObject.h"

namespace Lucene {

class FieldsWriter : public LuceneObject {
public:
    FieldsWriter(const DirectoryPtr& d, const String& segment, const FieldInfosPtr& fn);
    FieldsWriter(const IndexOutputPtr& fdx, const IndexOutputPtr& fdt, const FieldInfosPtr& fn);
    virtual ~FieldsWriter();

    LUCENE_CLASS(FieldsWriter);

protected:
    FieldInfosPtr fieldInfos;
    IndexOutputPtr fieldsStream;
    IndexOutputPtr indexStream;
    bool doClose;

public:
    static const uint8_t FIELD_IS_TOKENIZED;
    static const uint8_t FIELD_IS_BINARY;
    static const uint8_t FIELD_IS_COMPRESSED;

    static const int32_t FORMAT; // Original format
    static const int32_t FORMAT_VERSION_UTF8_LENGTH_IN_BYTES; // Changed strings to UTF8
    static const int32_t FORMAT_LUCENE_3_0_NO_COMPRESSED_FIELDS; // Lucene 3.0: Removal of compressed fields

    // NOTE: if you introduce a new format, make it 1 higher than the current one, and always change this
    // if you switch to a new format!
    static const int32_t FORMAT_CURRENT;

public:
    void setFieldsStream(const IndexOutputPtr& stream);

    /// Writes the contents of buffer into the fields stream and adds a new entry for this document into the index
    /// stream.  This assumes the buffer was already written in the correct fields format.
    void flushDocument(int32_t numStoredFields, const RAMOutputStreamPtr& buffer);

    void skipDocument();
    void flush();
    void close();
    void writeField(const FieldInfoPtr& fi, const FieldablePtr& field);

    /// Bulk write a contiguous series of documents.  The lengths array is the length (in bytes) of each raw document.
    /// The stream IndexInput is the fieldsStream from which we should bulk-copy all bytes.
    void addRawDocuments(const IndexInputPtr& stream, Collection<int32_t> lengths, int32_t numDocs);

    void addDocument(const DocumentPtr& doc);
};

}

#endif
