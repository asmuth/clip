/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef TERMATTRIBUTE_H
#define TERMATTRIBUTE_H

#include "fnord-fts/util/Attribute.h"

namespace Lucene {

/// The term text of a Token.
class TermAttribute : public Attribute {
public:
    TermAttribute();
    virtual ~TermAttribute();

    LUCENE_CLASS(TermAttribute);

protected:
    static const int32_t MIN_BUFFER_SIZE;

    CharArray _termBuffer;
    int32_t _termLength;

public:
    virtual String toString();

    /// Returns the Token's term text.
    ///
    /// This method has a performance penalty because the text is stored internally in a char[].  If possible,
    /// use {@link #termBuffer()} and {@link #termLength()} directly instead.  If you really need a String, use
    /// this method, which is nothing more than a convenience call to new String(token.termBuffer(), 0,
    /// token.termLength())
    virtual String term();

    /// Copies the contents of buffer, starting at offset for length characters, into the termBuffer array.
    /// @param buffer the buffer to copy
    /// @param offset the index in the buffer of the first character to copy
    /// @param length the number of characters to copy
    virtual void setTermBuffer(const wchar_t* buffer, int32_t offset, int32_t length);

    /// Copies the contents of buffer into the termBuffer array.
    /// @param buffer the buffer to copy
    virtual void setTermBuffer(const String& buffer);

    /// Returns the internal termBuffer character array which you can then directly alter.  If the array is
    /// too small for your token, use {@link #resizeTermBuffer(int)} to increase it.  After altering the buffer
    /// be sure to call {@link #setTermLength} to record the number of valid characters that were placed into
    /// the termBuffer.
    virtual CharArray termBuffer();

    /// Optimized implementation of termBuffer.
    virtual wchar_t* termBufferArray();

    /// Grows the termBuffer to at least size newSize, preserving the existing content. Note: If the next
    /// operation is to change the contents of the term buffer use {@link #setTermBuffer(char[], int, int)},
    /// {@link #setTermBuffer(String)}, or {@link #setTermBuffer(String, int, int)} to optimally combine the
    /// resize with the setting of the termBuffer.
    /// @param newSize minimum size of the new termBuffer
    /// @return newly created termBuffer with length >= newSize
    virtual CharArray resizeTermBuffer(int32_t newSize);

    /// Return number of valid characters (length of the term) in the termBuffer array.
    virtual int32_t termLength();

    /// Set number of valid characters (length of the term) in the termBuffer array. Use this to truncate the
    /// termBuffer or to synchronize with external manipulation of the termBuffer.  Note: to grow the size of
    /// the array, use {@link #resizeTermBuffer(int)} first.
    /// @param length the truncated length
    virtual void setTermLength(int32_t length);

    virtual int32_t hashCode();
    virtual void clear();
    virtual LuceneObjectPtr clone(const LuceneObjectPtr& other = LuceneObjectPtr());
    virtual bool equals(const LuceneObjectPtr& other);
    virtual void copyTo(const AttributePtr& target);

protected:
    /// Allocates a buffer char[] of at least newSize, without preserving the existing content.  Its always
    /// used in places that set the content.
    /// @param newSize minimum size of the buffer
    void growTermBuffer(int32_t newSize);

    void initTermBuffer();
};

}

#endif
