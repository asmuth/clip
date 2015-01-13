/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef TERMINFOSWRITER_H
#define TERMINFOSWRITER_H

#include "LuceneObject.h"

namespace Lucene {

/// This stores a monotonically increasing set of <Term, TermInfo> pairs in a Directory.  A TermInfos
/// can be written once, in order.
class TermInfosWriter : public LuceneObject {
public:
    TermInfosWriter(const DirectoryPtr& directory, const String& segment, const FieldInfosPtr& fis, int32_t interval);
    TermInfosWriter(const DirectoryPtr& directory, const String& segment, const FieldInfosPtr& fis, int32_t interval, bool isIndex);
    virtual ~TermInfosWriter();

    LUCENE_CLASS(TermInfosWriter);

public:
    /// The file format version, a negative number.
    static const int32_t FORMAT;

    /// Changed strings to true utf8 with length-in-bytes not length-in-chars.
    static const int32_t FORMAT_VERSION_UTF8_LENGTH_IN_BYTES;

    /// NOTE: always change this if you switch to a new format.
    static const int32_t FORMAT_CURRENT;

    /// The fraction of terms in the "dictionary" which should be stored in RAM.  Smaller values use more memory, but
    /// make searching slightly faster, while larger values use less memory and make searching slightly slower.
    /// Searching is typically not dominated by dictionary lookup, so tweaking this is rarely useful.
    int32_t indexInterval;

    /// The fraction of {@link TermDocs} entries stored in skip tables, used to accelerate {@link TermDocs#skipTo(int)}.
    /// Larger values result in smaller indexes, greater acceleration, but fewer accelerable cases, while smaller values
    /// result in bigger indexes, less acceleration and more accelerable cases. More detailed experiments would be useful
    /// here.
    int32_t skipInterval;

    /// The maximum number of skip levels. Smaller values result in slightly smaller indexes, but slower skipping
    /// in big posting lists.
    int32_t maxSkipLevels;

protected:
    FieldInfosPtr fieldInfos;
    IndexOutputPtr output;
    TermInfoPtr lastTi;
    int64_t size;

    int64_t lastIndexPointer;
    bool isIndex;
    ByteArray lastTermBytes;
    int32_t lastTermBytesLength;
    int32_t lastFieldNumber;

    TermInfosWriterPtr otherWriter;
    TermInfosWriterWeakPtr _other;
    UTF8ResultPtr utf8Result;

    // Currently used only by assert statements
    UnicodeResultPtr unicodeResult1;
    UnicodeResultPtr unicodeResult2;

public:
    virtual void initialize();

    void add(const TermPtr& term, const TermInfoPtr& ti);

    /// Adds a new <<fieldNumber, termBytes>, TermInfo> pair to the set.  Term must be lexicographically
    /// greater than all previous Terms added. TermInfo pointers must be positive and greater than all previous.
    void add(int32_t fieldNumber, ByteArray termBytes, int32_t termBytesLength, const TermInfoPtr& ti);

    /// Called to complete TermInfos creation.
    void close();

protected:
    void initialize(const DirectoryPtr& directory, const String& segment, const FieldInfosPtr& fis, int32_t interval, bool isi);

    /// Currently used only by assert statements
    bool initUnicodeResults();

    /// Currently used only by assert statement
    int32_t compareToLastTerm(int32_t fieldNumber, ByteArray termBytes, int32_t termBytesLength);

    void writeTerm(int32_t fieldNumber, ByteArray termBytes, int32_t termBytesLength);
};

}

#endif
