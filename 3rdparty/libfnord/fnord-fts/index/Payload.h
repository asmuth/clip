/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef PAYLOAD_H
#define PAYLOAD_H

#include "fnord-fts/util/LuceneObject.h"

namespace fnord {
namespace fts {

/// A Payload is metadata that can be stored together with each occurrence of a term. This metadata is stored
/// inline in the posting list of the specific term.
///
/// To store payloads in the index a {@link TokenStream} has to be used that produces payload data.
///
/// Use {@link TermPositions#getPayloadLength()} and {@link TermPositions#getPayload(byte[], int)} to retrieve
/// the payloads from the index.
class Payload : public LuceneObject {
public:
    /// Creates an empty payload and does not allocate a byte array.
    Payload();

    /// Creates a new payload with the the given array as data.  A reference to the passed-in array is held,
    /// ie. no copy is made.
    /// @param data the data of this payload
    Payload(ByteArray data);

    /// Creates a new payload with the the given array as data.  A reference to the passed-in array is held,
    /// ie. no copy is made.
    /// @param data the data of this payload
    /// @param offset the offset in the data byte array
    /// @param length the length of the data
    Payload(ByteArray data, int32_t offset, int32_t length);

    virtual ~Payload();

    LUCENE_CLASS(Payload);

protected:
    /// the byte array containing the payload data
    ByteArray data;

    /// the offset within the byte array
    int32_t offset;

    /// the length of the payload data
    int32_t _length;

public:
    /// Sets this payloads data.  A reference to the passed-in array is held, ie. no copy is made.
    void setData(ByteArray data);

    /// Sets this payloads data.  A reference to the passed-in array is held, ie. no copy is made.
    void setData(ByteArray data, int32_t offset, int32_t length);

    /// Returns a reference to the underlying byte array that holds this payloads data.
    ByteArray getData();

    /// Returns the offset in the underlying byte array
    int32_t getOffset();

    /// Returns the length of the payload data.
    int32_t length();

    /// Returns the byte at the given index.
    uint8_t byteAt(int32_t index);

    /// Allocates a new byte array, copies the payload data into it and returns it.
    ByteArray toByteArray();

    /// Copies the payload data to a byte array.
    /// @param target the target byte array
    /// @param targetOffset the offset in the target byte array
    void copyTo(ByteArray target, int32_t targetOffset);

    /// Clones this payload by creating a copy of the underlying byte array.
    virtual LuceneObjectPtr clone(const LuceneObjectPtr& other = LuceneObjectPtr());

    virtual bool equals(const LuceneObjectPtr& other);
    virtual int32_t hashCode();
};

}

}
#endif
