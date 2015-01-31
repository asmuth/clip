/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef MULTIREADER_H
#define MULTIREADER_H

#include "fnord-fts/index/IndexReader.h"

namespace fnord {
namespace fts {

/// An IndexReader which reads multiple indexes, appending their content.
class MultiReader : public IndexReader {
public:
    /// Construct a MultiReader aggregating the named set of (sub)readers.  Directory locking for delete,
    /// undeleteAll, and setNorm operations is left to the subreaders.
    /// @param closeSubReaders indicates whether the subreaders should be closed when this MultiReader is closed
    /// @param subReaders set of (sub)readers
    MultiReader(Collection<IndexReaderPtr> subReaders, bool closeSubReaders = true);

    virtual ~MultiReader();

    LUCENE_CLASS(MultiReader);

protected:
    Collection<IndexReaderPtr> subReaders;
    Collection<int32_t> starts; // 1st docno for each segment
    Collection<uint8_t> decrefOnClose; // remember which subreaders to decRef on close
    MapStringByteArray normsCache;
    int32_t _maxDoc;
    int32_t _numDocs;
    bool _hasDeletions;

public:
    /// Tries to reopen the subreaders.
    ///
    /// If one or more subreaders could be re-opened (ie. subReader.reopen() returned a new instance != subReader),
    /// then a new MultiReader instance is returned, otherwise this instance is returned.
    ///
    /// A re-opened instance might share one or more subreaders with the old instance. Index modification
    /// operations result in undefined behavior when performed before the old instance is closed. (see {@link
    /// IndexReader#reopen()}).
    ///
    /// If subreaders are shared, then the reference count of those readers is increased to ensure that the
    /// subreaders remain open until the last referring reader is closed.
    virtual IndexReaderPtr reopen();

    /// Clones the subreaders. (see {@link IndexReader#clone()}).
    ///
    /// If subreaders are shared, then the reference count of those readers is increased to ensure that the
    /// subreaders remain open until the last referring reader is closed.
    virtual LuceneObjectPtr clone(const LuceneObjectPtr& other = LuceneObjectPtr());

    virtual Collection<TermFreqVectorPtr> getTermFreqVectors(int32_t docNumber);
    virtual TermFreqVectorPtr getTermFreqVector(int32_t docNumber, const String& field);
    virtual void getTermFreqVector(int32_t docNumber, const String& field, const TermVectorMapperPtr& mapper);
    virtual void getTermFreqVector(int32_t docNumber, const TermVectorMapperPtr& mapper);

    virtual bool isOptimized();

    /// Returns the number of documents in this index.
    virtual int32_t numDocs();

    /// Returns one greater than the largest possible document number.
    virtual int32_t maxDoc();

    /// Get the {@link Document} at the n'th position. The {@link FieldSelector} may be used to determine
    /// what {@link Field}s to load and how they should be loaded.
    virtual DocumentPtr document(int32_t n, const FieldSelectorPtr& fieldSelector);

    /// Returns true if document n has been deleted
    virtual bool isDeleted(int32_t n);

    /// Returns true if any documents have been deleted
    virtual bool hasDeletions();

    /// Returns true if there are norms stored for this field.
    virtual bool hasNorms(const String& field);

    /// Returns the byte-encoded normalization factor for the named field of every document.
    virtual ByteArray norms(const String& field);

    /// Reads the byte-encoded normalization factor for the named field of every document.
    virtual void norms(const String& field, ByteArray norms, int32_t offset);

    /// Returns an enumeration of all the terms in the index.
    virtual TermEnumPtr terms();

    /// Returns an enumeration of all terms starting at a given term.
    virtual TermEnumPtr terms(const TermPtr& t);

    /// Returns the number of documents containing the term t.
    virtual int32_t docFreq(const TermPtr& t);

    /// Returns an unpositioned {@link TermDocs} enumerator.
    virtual TermDocsPtr termDocs();

    /// Returns an unpositioned {@link TermPositions} enumerator.
    virtual TermPositionsPtr termPositions();

    /// Get a list of unique field names that exist in this index and have the specified field option
    /// information.
    virtual HashSet<String> getFieldNames(FieldOption fieldOption);

    /// Checks recursively if all subreaders are up to date.
    virtual bool isCurrent();

    /// Not implemented.
    virtual int64_t getVersion();

    /// Returns the sequential sub readers that this reader is logically composed of.
    virtual Collection<IndexReaderPtr> getSequentialSubReaders();

protected:
    /// If clone is true then we clone each of the subreaders
    /// @param doClone
    /// @return New IndexReader, or same one (this) if reopen/clone is not necessary
    IndexReaderPtr doReopen(bool doClone);

    /// Implements deletion of the document numbered docNum.
    virtual void doDelete(int32_t docNum);

    /// Implements actual undeleteAll() in subclass.
    virtual void doUndeleteAll();

    /// Find reader for doc n
    int32_t readerIndex(int32_t n);

    /// Implements setNorm in subclass.
    virtual void doSetNorm(int32_t doc, const String& field, uint8_t value);

    virtual void doCommit(MapStringString commitUserData);

    /// Implements close.
    virtual void doClose();
};

}

}
#endif
