/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef FILTERINDEXREADER_H
#define FILTERINDEXREADER_H

#include "fnord-fts/index/IndexReader.h"
#include "fnord-fts/index/TermPositions.h"
#include "fnord-fts/index/TermEnum.h"

namespace Lucene {

/// A FilterIndexReader contains another IndexReader, which it uses as its basic source of data, possibly
/// transforming the data along the way or providing additional functionality. The class FilterIndexReader
/// itself simply implements all abstract methods of IndexReader with versions that pass all requests to
/// the contained index reader. Subclasses of FilterIndexReader may further override some of these methods
/// and may also provide additional methods and fields.
class FilterIndexReader : public IndexReader {
public:
    /// Construct a FilterIndexReader based on the specified base reader. Directory locking for delete,
    /// undeleteAll, and setNorm operations is left to the base reader.
    ///
    /// Note that base reader is closed if this FilterIndexReader is closed.
    /// @param in specified base reader.
    FilterIndexReader(const IndexReaderPtr& in);

    virtual ~FilterIndexReader();

    LUCENE_CLASS(FilterIndexReader);

protected:
    IndexReaderPtr in;

public:
    virtual DirectoryPtr directory();
    virtual Collection<TermFreqVectorPtr> getTermFreqVectors(int32_t docNumber);
    virtual TermFreqVectorPtr getTermFreqVector(int32_t docNumber, const String& field);
    virtual void getTermFreqVector(int32_t docNumber, const String& field, const TermVectorMapperPtr& mapper);
    virtual void getTermFreqVector(int32_t docNumber, const TermVectorMapperPtr& mapper);
    virtual int32_t numDocs();
    virtual int32_t maxDoc();
    virtual DocumentPtr document(int32_t n, const FieldSelectorPtr& fieldSelector);
    virtual bool isDeleted(int32_t n);
    virtual bool hasDeletions();
    virtual bool hasNorms(const String& field);
    virtual ByteArray norms(const String& field);
    virtual void norms(const String& field, ByteArray norms, int32_t offset);
    virtual TermEnumPtr terms();
    virtual TermEnumPtr terms(const TermPtr& t);
    virtual int32_t docFreq(const TermPtr& t);
    virtual TermDocsPtr termDocs();
    virtual TermDocsPtr termDocs(const TermPtr& term);
    virtual TermPositionsPtr termPositions();
    virtual HashSet<String> getFieldNames(FieldOption fieldOption);
    virtual int64_t getVersion();
    virtual bool isCurrent();
    virtual bool isOptimized();
    virtual Collection<IndexReaderPtr> getSequentialSubReaders();

    /// If the subclass of FilteredIndexReader modifies the contents of the FieldCache, you must
    /// override this method to provide a different key
    virtual LuceneObjectPtr getFieldCacheKey();

    /// If the subclass of FilteredIndexReader modifies the deleted docs, you must override this
    /// method to provide a different key
    virtual LuceneObjectPtr getDeletesCacheKey();

protected:
    virtual void doUndeleteAll();
    virtual void doSetNorm(int32_t doc, const String& field, uint8_t value);
    virtual void doDelete(int32_t docNum);
    virtual void doCommit(MapStringString commitUserData);
    virtual void doClose();
};

/// Base class for filtering {@link TermDocs} implementations.
class FilterTermDocs : public TermPositions, public LuceneObject {
public:
    FilterTermDocs(const TermDocsPtr& in);
    virtual ~FilterTermDocs();

    LUCENE_CLASS(FilterTermDocs);

protected:
    TermDocsPtr in;

public:
    virtual void seek(const TermPtr& term);
    virtual void seek(const TermEnumPtr& termEnum);
    virtual int32_t doc();
    virtual int32_t freq();
    virtual bool next();
    virtual int32_t read(Collection<int32_t> docs, Collection<int32_t> freqs);
    virtual bool skipTo(int32_t target);
    virtual void close();
};

/// Base class for filtering {@link TermPositions} implementations.
class FilterTermPositions : public FilterTermDocs {
public:
    FilterTermPositions(const TermPositionsPtr& in);
    virtual ~FilterTermPositions();

    LUCENE_CLASS(FilterTermPositions);

public:
    virtual int32_t nextPosition();
    virtual int32_t getPayloadLength();
    virtual ByteArray getPayload(ByteArray data, int32_t offset);
    virtual bool isPayloadAvailable();
};

/// Base class for filtering {@link TermEnum} implementations.
class FilterTermEnum : public TermEnum {
public:
    FilterTermEnum(const TermEnumPtr& in);
    virtual ~FilterTermEnum();

    LUCENE_CLASS(FilterTermEnum);

protected:
    TermEnumPtr in;

public:
    virtual bool next();
    virtual TermPtr term();
    virtual int32_t docFreq();
    virtual void close();
};

}

#endif
