/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef SEGMENTREADER_H
#define SEGMENTREADER_H

#include "fnord-fts/index/IndexReader.h"
#include "fnord-fts/util/CloseableThreadLocal.h"

namespace fnord {
namespace fts {

class SegmentReader : public IndexReader {
public:
    SegmentReader();
    virtual ~SegmentReader();

    LUCENE_CLASS(SegmentReader);

protected:
    bool readOnly;

public:
    BitVectorPtr deletedDocs;
    SegmentReaderRefPtr deletedDocsRef;
    CoreReadersPtr core;
    FieldsReaderLocalPtr fieldsReaderLocal;
    SegmentInfoPtr rollbackSegmentInfo;
    CloseableThreadLocal<TermVectorsReader> termVectorsLocal;
    FieldInfosPtr fieldInfos();

    /// Create a clone from the initial TermVectorsReader and store it in the ThreadLocal.
    /// @return TermVectorsReader
    TermVectorsReaderPtr getTermVectorsReader();

    TermVectorsReaderPtr getTermVectorsReaderOrig();
    FieldsReaderPtr getFieldsReader();
    MapStringNorm _norms;

private:
    SegmentInfoPtr si;
    int32_t readBufferSize;
    bool deletedDocsDirty;
    bool normsDirty;
    int32_t pendingDeleteCount;

    bool rollbackHasChanges;
    bool rollbackDeletedDocsDirty;
    bool rollbackNormsDirty;
    int32_t rollbackPendingDeleteCount;

    // optionally used for the .nrm file shared by multiple norms
    IndexInputPtr singleNormStream;
    SegmentReaderRefPtr singleNormRef;

public:
    virtual void initialize();

    using IndexReader::document;
    using IndexReader::termPositions;

    static SegmentReaderPtr get(bool readOnly, const SegmentInfoPtr& si, int32_t termInfosIndexDivisor);
    static SegmentReaderPtr get(bool readOnly, const DirectoryPtr& dir, const SegmentInfoPtr& si, int32_t readBufferSize, bool doOpenStores, int32_t termInfosIndexDivisor);

    void openDocStores();

    virtual LuceneObjectPtr clone(const LuceneObjectPtr& other = LuceneObjectPtr());
    virtual LuceneObjectPtr clone(bool openReadOnly, const LuceneObjectPtr& other = LuceneObjectPtr());
    SegmentReaderPtr reopenSegment(const SegmentInfoPtr& si, bool doClone, bool openReadOnly);

    static bool hasDeletions(const SegmentInfoPtr& si);

    /// Returns true if any documents have been deleted
    virtual bool hasDeletions();

    static bool usesCompoundFile(const SegmentInfoPtr& si);
    static bool hasSeparateNorms(const SegmentInfoPtr& si);

    HashSet<String> files();

    /// Returns an enumeration of all the terms in the index.
    virtual TermEnumPtr terms();

    /// Returns an enumeration of all terms starting at a given term.
    virtual TermEnumPtr terms(const TermPtr& t);

    /// Get the {@link Document} at the n'th position.
    virtual DocumentPtr document(int32_t n, const FieldSelectorPtr& fieldSelector);

    /// Returns true if document n has been deleted
    virtual bool isDeleted(int32_t n);

    /// Returns an enumeration of all the documents which contain term.
    virtual TermDocsPtr termDocs(const TermPtr& term);

    /// Returns an unpositioned {@link TermDocs} enumerator.
    virtual TermDocsPtr termDocs();

    /// Returns an unpositioned {@link TermPositions} enumerator.
    virtual TermPositionsPtr termPositions();

    /// Returns the number of documents containing the term t.
    virtual int32_t docFreq(const TermPtr& t);

    /// Returns the number of documents in this index.
    virtual int32_t numDocs();

    /// Returns one greater than the largest possible document number.
    virtual int32_t maxDoc();

    /// Get a list of unique field names that exist in this index and have the specified field option information.
    virtual HashSet<String> getFieldNames(FieldOption fieldOption);

    /// Returns true if there are norms stored for this field.
    virtual bool hasNorms(const String& field);

    /// Returns the byte-encoded normalization factor for the named field of every document.
    virtual ByteArray norms(const String& field);

    /// Read norms into a pre-allocated array.
    virtual void norms(const String& field, ByteArray norms, int32_t offset);

    bool termsIndexLoaded();

    /// NOTE: only called from IndexWriter when a near real-time reader is opened, or applyDeletes is run, sharing a
    /// segment that's still being merged.  This method is not thread safe, and relies on the synchronization in IndexWriter
    void loadTermsIndex(int32_t termsIndexDivisor);

    bool normsClosed(); // for testing only
    bool normsClosed(const String& field); // for testing only

    /// Return a term frequency vector for the specified document and field. The vector returned contains term
    /// numbers and frequencies for all terms in the specified field of this document, if the field had
    /// storeTermVector flag set.  If the flag was not set, the method returns null.
    virtual TermFreqVectorPtr getTermFreqVector(int32_t docNumber, const String& field);

    /// Load the Term Vector into a user-defined data structure instead of relying on the parallel arrays
    /// of the {@link TermFreqVector}.
    virtual void getTermFreqVector(int32_t docNumber, const String& field, const TermVectorMapperPtr& mapper);

    /// Map all the term vectors for all fields in a Document
    virtual void getTermFreqVector(int32_t docNumber, const TermVectorMapperPtr& mapper);

    /// Return an array of term frequency vectors for the specified document.  The array contains a vector for
    /// each vectorized field in the document.  Each vector vector contains term numbers and frequencies for all
    /// terms in a given vectorized field.  If no such fields existed, the method returns null.
    virtual Collection<TermFreqVectorPtr> getTermFreqVectors(int32_t docNumber);

    /// Return the name of the segment this reader is reading.
    String getSegmentName();

    /// Return the SegmentInfo of the segment this reader is reading.
    SegmentInfoPtr getSegmentInfo();
    void setSegmentInfo(const SegmentInfoPtr& info);

    void startCommit();
    void rollbackCommit();

    /// Returns the directory this index resides in.
    virtual DirectoryPtr directory();

    /// This is necessary so that cloned SegmentReaders (which share the underlying postings data)
    /// will map to the same entry in the FieldCache.
    virtual LuceneObjectPtr getFieldCacheKey();
    virtual LuceneObjectPtr getDeletesCacheKey();

    /// Returns the number of unique terms (across all fields) in this reader.
    virtual int64_t getUniqueTermCount();

    static SegmentReaderPtr getOnlySegmentReader(const DirectoryPtr& dir);
    static SegmentReaderPtr getOnlySegmentReader(const IndexReaderPtr& reader);

    virtual int32_t getTermInfosIndexDivisor();

protected:
    bool checkDeletedCounts();
    void loadDeletedDocs();

    /// Clones the norm bytes.  May be overridden by subclasses.
    /// @param bytes Byte array to clone
    /// @return New BitVector
    virtual ByteArray cloneNormBytes(ByteArray bytes);

    /// Clones the deleteDocs BitVector.  May be overridden by subclasses.
    /// @param bv BitVector to clone
    /// @return New BitVector
    virtual BitVectorPtr cloneDeletedDocs(const BitVectorPtr& bv);

    /// Implements commit.
    virtual void doCommit(MapStringString commitUserData);

    virtual void commitChanges(MapStringString commitUserData);

    /// Implements close.
    virtual void doClose();

    /// Implements deletion of the document numbered docNum.
    /// Applications should call {@link #deleteDocument(int)} or {@link #deleteDocuments(Term)}.
    virtual void doDelete(int32_t docNum);

    /// Implements actual undeleteAll() in subclass.
    virtual void doUndeleteAll();

    /// can return null if norms aren't stored
    ByteArray getNorms(const String& field);

    /// Implements setNorm in subclass.
    virtual void doSetNorm(int32_t doc, const String& field, uint8_t value);

    void openNorms(const DirectoryPtr& cfsDir, int32_t readBufferSize);

    friend class ReaderPool;
    friend class IndexWriter;
    friend class Norm;
};

}

}
#endif
