/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef PARALLELREADER_H
#define PARALLELREADER_H

#include "fnord-fts/index/IndexReader.h"

namespace Lucene {

/// An IndexReader which reads multiple, parallel indexes.  Each index added must have the same number of
/// documents, but typically each contains different fields.  Each document contains the union of the fields
/// of all documents with the same document number.  When searching, matches for a query term are from the
/// first index added that has the field.
///
/// This is useful, eg., with collections that have large fields which change rarely and small fields that
/// change more frequently.  The smaller fields may be re-indexed in a new index and both indexes may be
/// searched together.
///
/// Warning: It is up to you to make sure all indexes are created and modified the same way.  For example,
/// if you add documents to one index, you need to add the same documents in the same order to the other
/// indexes. Failure to do so will result in undefined behavior
class ParallelReader : public IndexReader {
public:
    /// Construct a ParallelReader.
    /// @param closeSubReaders indicates whether the subreaders should be closed when this ParallelReader
    /// is closed
    ParallelReader(bool closeSubReaders = true);

    virtual ~ParallelReader();

    LUCENE_CLASS(ParallelReader);

protected:
    Collection<IndexReaderPtr> readers;
    Collection<uint8_t> decrefOnClose; // remember which subreaders to decRef on close
    bool incRefReaders;
    MapStringIndexReader fieldToReader;
    MapIndexReaderSetString readerToFields;
    Collection<IndexReaderPtr> storedFieldReaders;

    int32_t _maxDoc;
    int32_t _numDocs;
    bool _hasDeletions;

public:
    /// Add an IndexReader.
    void add(const IndexReaderPtr& reader);

    /// Add an IndexReader whose stored fields will not be returned.  This can accelerate search when stored
    /// fields are only needed from a subset of the IndexReaders.
    void add(const IndexReaderPtr& reader, bool ignoreStoredFields);

    virtual LuceneObjectPtr clone(const LuceneObjectPtr& other = LuceneObjectPtr());

    /// Tries to reopen the subreaders.
    ///
    /// If one or more subreaders could be re-opened (ie. subReader.reopen() returned a new instance != subReader),
    /// then a new ParallelReader instance is returned, otherwise this instance is returned.
    ///
    /// A re-opened instance might share one or more subreaders with the old instance. Index modification
    /// operations result in undefined behavior when performed before the old instance is closed.
    /// (see {@link IndexReader#reopen()}).
    ///
    /// If subreaders are shared, then the reference count of those readers is increased to ensure that the
    /// subreaders remain open until the last referring reader is closed.
    virtual IndexReaderPtr reopen();

    /// Returns the number of documents in this index.
    virtual int32_t numDocs();

    /// Returns one greater than the largest possible document number.  This may be used to, eg., determine
    /// how big to allocate an array which will have an element for every document number in an index.
    virtual int32_t maxDoc();

    /// Returns true if any documents have been deleted
    virtual bool hasDeletions();

    /// Returns true if document n has been deleted
    virtual bool isDeleted(int32_t n);

    /// Get the {@link Document} at the n'th position.
    virtual DocumentPtr document(int32_t n, const FieldSelectorPtr& fieldSelector);

    /// Return an array of term frequency vectors for the specified document.
    virtual Collection<TermFreqVectorPtr> getTermFreqVectors(int32_t docNumber);

    /// Return a term frequency vector for the specified document and field.
    virtual TermFreqVectorPtr getTermFreqVector(int32_t docNumber, const String& field);

    /// Load the Term Vector into a user-defined data structure instead of relying on the parallel arrays
    /// of the {@link TermFreqVector}.
    virtual void getTermFreqVector(int32_t docNumber, const String& field, const TermVectorMapperPtr& mapper);

    /// Map all the term vectors for all fields in a Document
    virtual void getTermFreqVector(int32_t docNumber, const TermVectorMapperPtr& mapper);

    /// Returns true if there are norms stored for this field.
    virtual bool hasNorms(const String& field);

    /// Returns the byte-encoded normalization factor for the named field of every document.
    virtual ByteArray norms(const String& field);

    /// Reads the byte-encoded normalization factor for the named field of every document.
    virtual void norms(const String& field, ByteArray norms, int32_t offset);

    /// Returns an enumeration of all the terms in the index. The enumeration is ordered by
    /// Term::compareTo(). Each term is greater than all that precede it in the enumeration.
    /// Note that after calling terms(), {@link TermEnum#next()} must be called on the resulting
    /// enumeration before calling other methods such as {@link TermEnum#term()}.
    virtual TermEnumPtr terms();

    /// Returns an enumeration of all terms starting at a given term. If the given term does not
    /// exist, the enumeration is positioned at the first term greater than the supplied term.
    /// The enumeration is ordered by Term::compareTo(). Each term is greater than all that precede
    /// it in the enumeration.
    virtual TermEnumPtr terms(const TermPtr& t);

    /// Returns the number of documents containing the term t.
    virtual int32_t docFreq(const TermPtr& t);

    /// Returns an enumeration of all the documents which contain term.  For each document, the
    /// document number, the frequency of the term in that document is also provided, for use in
    /// search scoring.  If term is null, then all non-deleted docs are returned with freq=1.
    /// The enumeration is ordered by document number.  Each document number is greater than all
    /// that precede it in the enumeration.
    virtual TermDocsPtr termDocs(const TermPtr& term);

    /// Returns an unpositioned {@link TermDocs} enumerator.
    virtual TermDocsPtr termDocs();

    /// Returns an enumeration of all the documents which contain term.
    virtual TermPositionsPtr termPositions(const TermPtr& term);

    /// Returns an unpositioned {@link TermPositions} enumerator.
    virtual TermPositionsPtr termPositions();

    /// Checks recursively if all subreaders are up to date.
    virtual bool isCurrent();

    /// Checks recursively if all subindexes are optimized
    virtual bool isOptimized();

    /// Not implemented.
    virtual int64_t getVersion();

    Collection<IndexReaderPtr> getSubReaders();

    /// Get a list of unique field names that exist in this index and have the specified field option
    /// information.
    virtual HashSet<String> getFieldNames(FieldOption fieldOption);

protected:
    IndexReaderPtr doReopen(bool doClone);

    /// Implements deletion of the document numbered docNum.
    virtual void doDelete(int32_t docNum);

    /// Implements actual undeleteAll().
    virtual void doUndeleteAll();

    /// Implements setNorm in subclass.
    virtual void doSetNorm(int32_t doc, const String& field, uint8_t value);

    /// Implements commit.
    virtual void doCommit(MapStringString commitUserData);

    /// Implements close.
    virtual void doClose();

    friend class ParallelTermEnum;
    friend class ParallelTermDocs;
    friend class ParallelTermPositions;
};

}

#endif
