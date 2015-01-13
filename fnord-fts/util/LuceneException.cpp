/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "LuceneException.h"

namespace Lucene {

LuceneException::LuceneException(const String& error, ExceptionType type) throw() {
    this->error = error;
    this->type = type;
}

LuceneException::~LuceneException() throw() {
}

LuceneException::ExceptionType LuceneException::getType() const {
    return type;
}

String LuceneException::getError() const {
    return error;
}

bool LuceneException::isNull() const {
    return (type == Null);
}

void LuceneException::throwException() {
    switch (type) {
    case LuceneException::AlreadyClosed:
        boost::throw_exception(AlreadyClosedException(error, type));
    case LuceneException::Compression:
        boost::throw_exception(CompressionException(error, type));
    case LuceneException::CorruptIndex:
        boost::throw_exception(CorruptIndexException(error, type));
    case LuceneException::FieldReader:
        boost::throw_exception(FieldReaderException(error, type));
    case LuceneException::FileNotFound:
        boost::throw_exception(FileNotFoundException(error, type));
    case LuceneException::IllegalArgument:
        boost::throw_exception(IllegalArgumentException(error, type));
    case LuceneException::IllegalState:
        boost::throw_exception(IllegalStateException(error, type));
    case LuceneException::IndexOutOfBounds:
        boost::throw_exception(IndexOutOfBoundsException(error, type));
    case LuceneException::IO:
        boost::throw_exception(IOException(error, type));
    case LuceneException::LockObtainFailed:
        boost::throw_exception(LockObtainFailedException(error, type));
    case LuceneException::LockReleaseFailed:
        boost::throw_exception(LockReleaseFailedException(error, type));
    case LuceneException::Lookahead:
        boost::throw_exception(LookaheadSuccess(error, type));
    case LuceneException::MergeAborted:
        boost::throw_exception(MergeAbortedException(error, type));
    case LuceneException::Merge:
        boost::throw_exception(MergeException(error, type));
    case LuceneException::NoSuchDirectory:
        boost::throw_exception(NoSuchDirectoryException(error, type));
    case LuceneException::NullPointer:
        boost::throw_exception(NullPointerException(error, type));
    case LuceneException::NumberFormat:
        boost::throw_exception(NumberFormatException(error, type));
    case LuceneException::OutOfMemory:
        boost::throw_exception(OutOfMemoryError(error, type));
    case LuceneException::Parse:
        boost::throw_exception(ParseException(error, type));
    case LuceneException::QueryParser:
        boost::throw_exception(QueryParserError(error, type));
    case LuceneException::Runtime:
        boost::throw_exception(RuntimeException(error, type));
    case LuceneException::StaleReader:
        boost::throw_exception(StaleReaderException(error, type));
    case LuceneException::StopFillCache:
        boost::throw_exception(StopFillCacheException(error, type));
    case LuceneException::Temporary:
        boost::throw_exception(TemporaryException(error, type));
    case LuceneException::TimeExceeded:
        boost::throw_exception(TimeExceededException(error, type));
    case LuceneException::TooManyClauses:
        boost::throw_exception(TooManyClausesException(error, type));
    case LuceneException::UnsupportedOperation:
        boost::throw_exception(UnsupportedOperationException(error, type));
    case LuceneException::Null:
        // silence static analyzer
        break;
    }
}

}
