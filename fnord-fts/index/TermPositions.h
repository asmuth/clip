/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef TERMPOSITIONS_H
#define TERMPOSITIONS_H

#include "fnord-fts/index/TermDocs.h"

namespace Lucene {

/// TermPositions provides an interface for enumerating the <document, frequency, <position>*>
/// tuples for a term.  The document and frequency are the same as for a TermDocs.  The positions portion
/// lists the ordinal positions of each occurrence of a term in a document.
/// @see IndexReader#termPositions()
class TermPositions : public TermDocs {
protected:
    TermPositions();

public:
    virtual ~TermPositions();
    LUCENE_INTERFACE(TermPositions);

public:
    /// Returns next position in the current document.  It is an error to call this more than {@link #freq()}
    /// times without calling {@link #next()}.  This is invalid until {@link #next()} is called for
    // the first time.
    virtual int32_t nextPosition();

    /// Returns the length of the payload at the current term position.  This is invalid until {@link
    /// #nextPosition()} is called for the first time.
    /// @return length of the current payload in number of bytes
    virtual int32_t getPayloadLength();

    /// Returns the payload data at the current term position.  This is invalid until {@link #nextPosition()}
    /// is called for the first time.
    /// This method must not be called more than once after each call of {@link #nextPosition()}. However,
    /// payloads are loaded lazily, so if the payload data for the current position is not needed,
    /// this method may not be called at all for performance reasons.
    /// @param data the array into which the data of this payload is to be stored
    /// @param offset the offset in the array into which the data of this payload is to be stored.
    /// @return a byte array containing the data of this payload
    virtual ByteArray getPayload(ByteArray data, int32_t offset);

    /// Checks if a payload can be loaded at this position.
    /// Payloads can only be loaded once per call to {@link #nextPosition()}.
    /// @return true if there is a payload available at this position that can be loaded
    virtual bool isPayloadAvailable();
};

}

#endif
