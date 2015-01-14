/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef LUCENEEXCEPTION_H
#define LUCENEEXCEPTION_H

#include "fnord-fts/fts.h"

namespace fnord {
namespace fts {

/// Lucene exception container.
class LuceneException : public std::exception {
public:
    enum ExceptionType {
        Null,
        AlreadyClosed,
        Compression,
        CorruptIndex,
        FieldReader,
        FileNotFound,
        IllegalArgument,
        IllegalState,
        IndexOutOfBounds,
        IO,
        LockObtainFailed,
        LockReleaseFailed,
        Lookahead,
        MergeAborted,
        Merge,
        NoSuchDirectory,
        NullPointer,
        NumberFormat,
        OutOfMemory,
        Parse,
        QueryParser,
        Runtime,
        StaleReader,
        StopFillCache,
        Temporary,
        TimeExceeded,
        TooManyClauses,
        UnsupportedOperation
    };

    LuceneException(const String& error = EmptyString, LuceneException::ExceptionType type = Null) throw();
    ~LuceneException() throw();

protected:
    ExceptionType type;
    String error;

public:
    ExceptionType getType() const;
    String getError() const;
    bool isNull() const;
    void throwException();
};

template <class ParentException, LuceneException::ExceptionType Type>
class ExceptionTemplate : public ParentException {
public:
    ExceptionTemplate(const String& error = EmptyString, LuceneException::ExceptionType type = Type) : ParentException(error, type) {
    }
};

typedef ExceptionTemplate<LuceneException, LuceneException::Runtime> RuntimeException;
typedef ExceptionTemplate<LuceneException, LuceneException::OutOfMemory> OutOfMemoryError;
typedef ExceptionTemplate<LuceneException, LuceneException::Temporary> TemporaryException;
typedef ExceptionTemplate<RuntimeException, LuceneException::IllegalState> IllegalStateException;
typedef ExceptionTemplate<RuntimeException, LuceneException::IllegalArgument> IllegalArgumentException;
typedef ExceptionTemplate<RuntimeException, LuceneException::IndexOutOfBounds> IndexOutOfBoundsException;
typedef ExceptionTemplate<RuntimeException, LuceneException::NullPointer> NullPointerException;
typedef ExceptionTemplate<RuntimeException, LuceneException::FieldReader> FieldReaderException;
typedef ExceptionTemplate<RuntimeException, LuceneException::Merge> MergeException;
typedef ExceptionTemplate<RuntimeException, LuceneException::StopFillCache> StopFillCacheException;
typedef ExceptionTemplate<RuntimeException, LuceneException::TimeExceeded> TimeExceededException;
typedef ExceptionTemplate<RuntimeException, LuceneException::TooManyClauses> TooManyClausesException;
typedef ExceptionTemplate<RuntimeException, LuceneException::UnsupportedOperation> UnsupportedOperationException;
typedef ExceptionTemplate<IllegalArgumentException, LuceneException::NumberFormat> NumberFormatException;
typedef ExceptionTemplate<IllegalStateException, LuceneException::AlreadyClosed> AlreadyClosedException;
typedef ExceptionTemplate<LuceneException, LuceneException::IO> IOException;
typedef ExceptionTemplate<IOException, LuceneException::CorruptIndex> CorruptIndexException;
typedef ExceptionTemplate<IOException, LuceneException::FileNotFound> FileNotFoundException;
typedef ExceptionTemplate<IOException, LuceneException::LockObtainFailed> LockObtainFailedException;
typedef ExceptionTemplate<IOException, LuceneException::LockReleaseFailed> LockReleaseFailedException;
typedef ExceptionTemplate<IOException, LuceneException::MergeAborted> MergeAbortedException;
typedef ExceptionTemplate<IOException, LuceneException::StaleReader> StaleReaderException;
typedef ExceptionTemplate<FileNotFoundException, LuceneException::NoSuchDirectory> NoSuchDirectoryException;
typedef ExceptionTemplate<LuceneException, LuceneException::Lookahead> LookaheadSuccess;
typedef ExceptionTemplate<LuceneException, LuceneException::Parse> ParseException;
typedef ExceptionTemplate<LuceneException, LuceneException::QueryParser> QueryParserError;
typedef ExceptionTemplate<LuceneException, LuceneException::Compression> CompressionException;
}

}
#endif
