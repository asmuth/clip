/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef INDEXREADER_H
#define INDEXREADER_H

#include "fnord-fts/index/SegmentInfos.h"

namespace fnord {
namespace fts {

/// IndexReader is an abstract class, providing an interface for accessing an index.  Search of an index is done
/// entirely through this abstract interface, so that any subclass which implements it is searchable.
///
/// Concrete subclasses of IndexReader are usually constructed with a call to one of the static open methods,
/// eg. {@link #open(DirectoryPtr, bool)}.
///
/// For efficiency, in this API documents are often referred to via document numbers, non-negative integers which
/// each name a unique document in the index.  These document numbers are ephemeral -they may change as documents
/// are added to and deleted from an index.  Clients should thus not rely on a given document having the same number
/// between sessions.
///
/// An IndexReader can be opened on a directory for which an IndexWriter is opened already, but it cannot be used
/// to delete documents from the index then.
///
/// NOTE: for backwards API compatibility, several methods are not listed as abstract, but have no useful implementations
/// in this base class and instead always throw UnsupportedOperation exception.  Subclasses are strongly encouraged to
/// override these methods, but in many cases may not need to.
///
/// NOTE: as of 2.4, it's possible to open a read-only IndexReader using the static open methods that accept the bool
/// readOnly parameter.  Such a reader has better concurrency as it's not necessary to synchronize on the isDeleted
/// method.  You must specify false if you want to make changes with the resulting IndexReader.
///
/// NOTE: {@link IndexReader} instances are completely thread safe, meaning multiple threads can call any of its methods,
/// concurrently.  If your application requires external synchronization, you should not synchronize on the IndexReader
/// instance; use your own (non-Lucene) objects instead.
class IndexReader : public LuceneObject {
public:
    IndexReader();
    virtual ~IndexReader();

    LUCENE_CLASS(IndexReader);

public:
    /// Constants describing field properties, for example used for {@link IndexReader#getFieldNames(FieldOption)}.
    enum FieldOption {
        /// All fields
        FIELD_OPTION_ALL,
        /// All indexed fields
        FIELD_OPTION_INDEXED,
        /// All fields that store payloads
        FIELD_OPTION_STORES_PAYLOADS,
        /// All fields that omit tf
        FIELD_OPTION_OMIT_TERM_FREQ_AND_POSITIONS,
        /// All fields which are not indexed
        FIELD_OPTION_UNINDEXED,
        /// All fields which are indexed with termvectors enabled
        FIELD_OPTION_INDEXED_WITH_TERMVECTOR,
        /// All fields which are indexed but don't have termvectors enabled
        FIELD_OPTION_INDEXED_NO_TERMVECTOR,
        /// All fields with termvectors enabled. Please note that only standard termvector fields are returned
        FIELD_OPTION_TERMVECTOR,
        /// All fields with termvectors with position values enabled
        FIELD_OPTION_TERMVECTOR_WITH_POSITION,
        /// All fields with termvectors with offset values enabled
        FIELD_OPTION_TERMVECTOR_WITH_OFFSET,
        /// All fields with termvectors with offset values and position values enabled
        FIELD_OPTION_TERMVECTOR_WITH_POSITION_OFFSET
    };

    static const int32_t DEFAULT_TERMS_INDEX_DIVISOR;

protected:
    bool closed;
    bool _hasChanges;
    int32_t refCount;

public:
    /// Returns the current refCount for this reader
    int32_t getRefCount();

    /// Increments the refCount of this IndexReader instance.  RefCounts are used to determine when a reader can be
    /// closed safely, i.e. as soon as there are no more references.  Be sure to always call a corresponding {@link
    /// #decRef}, in a finally clause; otherwise the reader may never be closed.  Note that {@link #close} simply
    /// calls decRef(), which means that the IndexReader will not really be closed until {@link #decRef} has been
    /// called for all outstanding references.
    /// @see #decRef
    void incRef();

    /// Decreases the refCount of this IndexReader instance.  If the refCount drops to 0, then pending changes
    /// (if any) are committed to the index and this reader is closed.
    /// @see #incRef
    void decRef();

    /// Returns a IndexReader reading the index in the given Directory, with readOnly = true.
    /// @param directory the index directory
    static IndexReaderPtr open(const DirectoryPtr& directory);

    /// Returns an IndexReader reading the index in the given Directory.  You should pass readOnly = true, since it
    /// gives much better concurrent performance, unless you intend to do write operations (delete documents or change
    /// norms) with the reader.
    /// @param directory the index directory
    /// @param readOnly true if no changes (deletions, norms) will be made with this IndexReader
    static IndexReaderPtr open(const DirectoryPtr& directory, bool readOnly);

    /// Returns an IndexReader reading the index in the given {@link IndexCommit}.  You should pass readOnly = true,
    /// since it gives much better concurrent performance, unless you intend to do write operations (delete documents
    /// or change norms) with the reader.
    /// @param commit the commit point to open
    /// @param readOnly true if no changes (deletions, norms) will be made with this IndexReader
    static IndexReaderPtr open(const IndexCommitPtr& commit, bool readOnly);

    /// Returns an IndexReader reading the index in the given Directory, with a custom {@link IndexDeletionPolicy}.
    /// You should pass readOnly=true, since it gives much better concurrent performance, unless you intend to do write
    /// operations (delete documents or change norms) with the reader.
    /// @param directory the index directory
    /// @param deletionPolicy a custom deletion policy (only used if you use this reader to perform
    /// deletes or to set norms); see {@link IndexWriter} for details.
    /// @param readOnly true if no changes (deletions, norms) will be made with this IndexReader
    static IndexReaderPtr open(const DirectoryPtr& directory, const IndexDeletionPolicyPtr& deletionPolicy, bool readOnly);

    /// Returns an IndexReader reading the index in the given Directory, with a custom {@link IndexDeletionPolicy}.
    /// You should pass readOnly=true, since it gives much better concurrent performance, unless you intend to do write
    /// operations (delete documents or change norms) with the reader.
    /// @param directory the index directory
    /// @param deletionPolicy a custom deletion policy (only used if you use this reader to perform
    /// deletes or to set norms); see {@link IndexWriter} for details.
    /// @param readOnly true if no changes (deletions, norms) will be made with this IndexReader
    /// @param termInfosIndexDivisor Subsamples which indexed terms are loaded into RAM. This has the
    /// same effect as {@link IndexWriter#setTermIndexInterval} except that setting must be done at
    /// indexing time while this setting can be set per reader.  When set to N, then one in every
    /// N*termIndexInterval terms in the index is loaded into memory.  By setting this to a value > 1
    /// you can reduce memory usage, at the expense of higher latency when loading a TermInfo.  The
    /// default value is 1.  Set this to -1 to skip loading the terms index entirely.
    static IndexReaderPtr open(const DirectoryPtr& directory, const IndexDeletionPolicyPtr& deletionPolicy, bool readOnly, int32_t termInfosIndexDivisor);

    /// Returns an IndexReader reading the index in the given Directory, using a specific commit and with a custom
    /// {@link IndexDeletionPolicy}.  You should pass readOnly=true, since it gives much better concurrent performance,
    /// unless you intend to do write operations (delete documents or change norms) with the reader.
    /// @param commit the specific {@link IndexCommit} to open; see {@link IndexReader#listCommits} to list all
    /// commits in a directory
    /// @param deletionPolicy a custom deletion policy (only used if you use this reader to perform
    /// deletes or to set norms); see {@link IndexWriter} for details.
    /// @param readOnly true if no changes (deletions, norms) will be made with this IndexReader
    static IndexReaderPtr open(const IndexCommitPtr& commit, const IndexDeletionPolicyPtr& deletionPolicy, bool readOnly);

    /// Returns an IndexReader reading the index in the given Directory, using a specific commit and with a custom {@link
    /// IndexDeletionPolicy}.  You should pass readOnly=true, since it gives much better concurrent performance, unless
    /// you intend to do write operations (delete documents or change norms) with the reader.
    /// @param commit the specific {@link IndexCommit} to open; see {@link IndexReader#listCommits} to
    /// list all commits in a directory
    /// @param deletionPolicy a custom deletion policy (only used if you use this reader to perform deletes
    /// or to set norms); see {@link IndexWriter} for details.
    /// @param readOnly true if no changes (deletions, norms) will be made with this IndexReader
    /// @param termInfosIndexDivisor Subsamples which indexed terms are loaded into RAM. This has the same effect as
    /// {@link IndexWriter#setTermIndexInterval} except that setting must be done at indexing time while this setting can
    /// be set per reader.  When set to N, then one in every N * termIndexInterval terms in the index is loaded into
    /// memory.  By setting this to a value > 1 you can reduce memory usage, at the expense of higher latency when loading
    /// a TermInfo.  The default value is 1.  Set this to -1 to skip loading the terms index entirely.
    static IndexReaderPtr open(const IndexCommitPtr& commit, const IndexDeletionPolicyPtr& deletionPolicy, bool readOnly, int32_t termInfosIndexDivisor);

    /// Refreshes an IndexReader if the index has changed since this instance was (re)opened.
    ///
    /// Opening an IndexReader is an expensive operation. This method can be used to refresh an existing IndexReader to
    /// reduce these costs. This method tries to only load segments that have changed or were created after the
    /// IndexReader was (re)opened.
    ///
    /// If the index has not changed since this instance was (re)opened, then this call is a NOOP and returns this
    /// instance. Otherwise, a new instance is returned. The old instance is not closed and remains usable.
    ///
    /// If the reader is reopened, even though they share resources internally, it's safe to make changes (deletions,
    /// norms) with the new reader.  All shared mutable state obeys "copy on write" semantics to ensure the changes are
    /// not seen by other readers.
    ///
    /// You can determine whether a reader was actually reopened by comparing the old instance with the
    /// instance returned by this method:
    ///
    /// <pre>
    /// IndexReaderPtr reader = ...
    /// ...
    /// IndexReaderPtr newReader = r.reopen();
    /// if (newReader != reader)
    /// {
    ///     ... // reader was reopened
    ///     reader->close();
    /// }
    /// reader = newReader;
    /// ...
    /// </pre>
    ///
    /// Be sure to synchronize that code so that other threads, if present, can never use reader after it has been
    /// closed and before it's switched to newReader.  If this reader is a near real-time reader (obtained from
    /// {@link IndexWriter#getReader()}, reopen() will simply call writer.getReader() again for you, though this
    /// may change in the future.
    virtual IndexReaderPtr reopen();

    /// Just like {@link #reopen()}, except you can change the readOnly of the original reader.  If the index is
    /// unchanged but readOnly is different then a new reader will be returned.
    virtual IndexReaderPtr reopen(bool openReadOnly);

    /// Reopen this reader on a specific commit point.  This always returns a readOnly reader.  If the specified commit
    /// point matches what this reader is already on, and this reader is already readOnly, then this same instance is
    /// returned; if it is not already readOnly, a readOnly clone is returned.
    virtual IndexReaderPtr reopen(const IndexCommitPtr& commit);

    /// Efficiently clones the IndexReader (sharing most internal state).
    ///
    /// On cloning a reader with pending changes (deletions, norms), the original reader transfers its write lock to the
    /// cloned reader.  This means only the cloned reader may make further changes to the index, and commit the changes
    /// to the index on close, but the old reader still reflects all changes made up until it was cloned.
    ///
    /// Like {@link #reopen()}, it's safe to make changes to either the original or the cloned reader: all shared mutable
    /// state obeys "copy on write" semantics to ensure the changes are not seen by other readers.
    virtual LuceneObjectPtr clone(const LuceneObjectPtr& other = LuceneObjectPtr());

    /// Clones the IndexReader and optionally changes readOnly.  A readOnly reader cannot open a writable reader.
    virtual LuceneObjectPtr clone(bool openReadOnly, const LuceneObjectPtr& other = LuceneObjectPtr());

    /// Returns the directory associated with this index.  The default implementation returns the directory specified by
    /// subclasses when delegating to the IndexReader(Directory) constructor, or throws an UnsupportedOperation exception
    /// if one was not specified.
    virtual DirectoryPtr directory();

    /// Returns the time the index in the named directory was last modified.  Do not use this to check
    /// whether the reader is still up-to-date, use {@link #isCurrent()} instead.
    static int64_t lastModified(const DirectoryPtr& directory2);

    /// Reads version number from segments files. The version number is initialized with a timestamp
    /// and then increased by one for each change of the index.
    /// @param directory where the index resides.
    /// @return version number.
    static int64_t getCurrentVersion(const DirectoryPtr& directory);

    /// Reads commitUserData, previously passed to {@link IndexWriter#commit(MapStringString)}, from
    /// current index segments file.  This will return null if {@link IndexWriter#commit(MapStringString)}
    /// has never been called for this index.
    static MapStringString getCommitUserData(const DirectoryPtr& directory);

    /// Version number when this IndexReader was opened. Not implemented in the IndexReader base class.
    ///
    /// If this reader is based on a Directory (ie, was created by calling {@link #open}, or {@link
    /// #reopen} on a reader based on a Directory), then this method returns the version recorded in the
    /// commit that the reader opened.  This version is advanced every time {@link IndexWriter#commit}
    /// is called.
    ///
    /// If instead this reader is a near real-time reader (ie, obtained by a call to {@link
    /// IndexWriter#getReader}, or by calling {@link #reopen} on a near real-time reader), then this
    /// method returns the version of the last commit done by the writer.  Note that even as further
    /// changes are made with the writer, the version will not changed until a commit is completed.
    /// Thus, you should not rely on this method to determine when a near real-time reader should be
    /// opened.  Use {@link #isCurrent} instead.
    virtual int64_t getVersion();

    /// Retrieve the String userData optionally passed to IndexWriter#commit.  This will return null if
    /// {@link IndexWriter#commit(MapStringString)} has never been called for this index.
    virtual MapStringString getCommitUserData();

    /// Check whether any new changes have occurred to the index since this reader was opened.
    ///
    /// If this reader is based on a Directory (ie, was created by calling {@link #open}, or {@link
    /// #reopen} on a reader based on a Directory), then this method checks if any further commits (see
    /// {@link IndexWriter#commit} have occurred in that directory).
    ///
    /// If instead this reader is a near real-time reader (ie, obtained by a call to {@link
    /// IndexWriter#getReader}, or by calling {@link #reopen} on a near real-time reader), then this
    /// method checks if either a new commit has occurred, or any new uncommitted changes have taken
    /// place via the writer.  Note that even if the writer has only performed merging, this method
    /// will still return false.
    ///
    /// In any event, if this returns false, you should call {@link #reopen} to get a new reader that
    /// sees the changes.
    virtual bool isCurrent();

    /// Checks is the index is optimized (if it has a single segment and no deletions).  Not implemented
    /// in the IndexReader base class.
    /// @return true if the index is optimized; false otherwise
    virtual bool isOptimized();

    /// Return an array of term frequency vectors for the specified document.  The array contains a
    /// vector for each vectorized field in the document.  Each vector contains terms and frequencies
    /// for all terms in a given vectorized field.  If no such fields existed, the method returns null.
    /// The term vectors that are returned may either be of type {@link TermFreqVector} or of type
    /// {@link TermPositionVector} if positions or offsets have been stored.
    ///
    /// @param docNumber document for which term frequency vectors are returned
    /// @return array of term frequency vectors. May be null if no term vectors have been stored for the
    /// specified document.
    virtual Collection<TermFreqVectorPtr> getTermFreqVectors(int32_t docNumber) = 0;

    /// Return a term frequency vector for the specified document and field. The returned vector contains
    /// terms and frequencies for the terms in the specified field of this document, if the field had the
    /// storeTermVector flag set. If termvectors had been stored with positions or offsets, a
    /// {@link TermPositionVector} is returned.
    ///
    /// @param docNumber document for which the term frequency vector is returned.
    /// @param field field for which the term frequency vector is returned.
    /// @return term frequency vector May be null if field does not exist in the specified document or
    /// term vector was not stored.
    virtual TermFreqVectorPtr getTermFreqVector(int32_t docNumber, const String& field) = 0;

    /// Load the Term Vector into a user-defined data structure instead of relying on the parallel arrays
    /// of the {@link TermFreqVector}.
    /// @param docNumber The number of the document to load the vector for
    /// @param field The name of the field to load
    /// @param mapper The {@link TermVectorMapper} to process the vector.  Must not be null.
    virtual void getTermFreqVector(int32_t docNumber, const String& field, const TermVectorMapperPtr& mapper) = 0;

    /// Map all the term vectors for all fields in a Document
    /// @param docNumber The number of the document to load the vector for
    /// @param mapper The {@link TermVectorMapper} to process the vector.  Must not be null.
    virtual void getTermFreqVector(int32_t docNumber, const TermVectorMapperPtr& mapper) = 0;

    /// Returns true if an index exists at the specified directory.  If the directory does not exist or
    /// if there is no index in it.
    /// @param directory the directory to check for an index
    /// @return true if an index exists; false otherwise
    static bool indexExists(const DirectoryPtr& directory);

    /// Returns the number of documents in this index.
    virtual int32_t numDocs() = 0;

    /// Returns one greater than the largest possible document number.  This may be used to, eg., determine
    /// how big to allocate an array which will have an element for every document number in an index.
    virtual int32_t maxDoc() = 0;

    /// Returns the number of deleted documents.
    int32_t numDeletedDocs();

    /// Returns the stored fields of the n'th Document in this index.
    ///
    /// NOTE: for performance reasons, this method does not check if the requested document is deleted, and
    /// therefore asking for a deleted document may yield unspecified results. Usually this is not required,
    /// however you can call {@link #isDeleted(int)} with the requested document ID to verify the document
    /// is not deleted.
    virtual DocumentPtr document(int32_t n);

    /// Get the {@link Document} at the n'th position. The {@link FieldSelector} may be used to determine
    /// what {@link Field}s to load and how they should be loaded.
    /// NOTE: If this Reader (more specifically, the underlying FieldsReader) is closed before the lazy
    /// {@link Field} is loaded an exception may be thrown. If you want the value of a lazy {@link Field}
    /// to be available after closing you must explicitly load it or fetch the Document again with a new
    /// loader.
    ///
    /// NOTE: for performance reasons, this method does not check if the requested document is deleted,
    /// and therefore asking for a deleted document may yield unspecified results. Usually this is not
    /// required, however you can call {@link #isDeleted(int32_t)} with the requested document ID to verify
    /// the document is not deleted.
    ///
    /// @param n Get the document at the n'th position
    /// @param fieldSelector The {@link FieldSelector} to use to determine what Fields should be loaded on
    /// the Document. May be null, in which case all Fields will be loaded.
    /// @return The stored fields of the {@link Document} at the n'th position
    /// @see Fieldable
    /// @see FieldSelector
    /// @see SetBasedFieldSelector
    /// @see LoadFirstFieldSelector
    virtual DocumentPtr document(int32_t n, const FieldSelectorPtr& fieldSelector) = 0;

    /// Returns true if document n has been deleted
    virtual bool isDeleted(int32_t n) = 0;

    /// Returns true if any documents have been deleted
    virtual bool hasDeletions() = 0;

    /// Used for testing
    virtual bool hasChanges();

    /// Returns true if there are norms stored for this field.
    virtual bool hasNorms(const String& field);

    /// Returns the byte-encoded normalization factor for the named field of every document.  This is used
    /// by the search code to score documents.
    /// @see Field#setBoost(double)
    virtual ByteArray norms(const String& field) = 0;

    /// Reads the byte-encoded normalization factor for the named field of every document.  This is used
    /// by the search code to score documents.
    /// @see Field#setBoost(double)
    virtual void norms(const String& field, ByteArray norms, int32_t offset) = 0;

    /// Resets the normalization factor for the named field of the named  document.  The norm represents
    /// the product of the field's {@link Fieldable#setBoost(double) boost} and its {@link
    /// Similarity#lengthNorm(String, int) length normalization}.  Thus, to preserve the length normalization
    /// values when resetting this, one should base the new value upon the old.
    ///
    /// NOTE: If this field does not store norms, then this method call will silently do nothing.
    ///
    /// @see #norms(String)
    /// @see Similarity#decodeNorm(byte)
    virtual void setNorm(int32_t doc, const String& field, uint8_t value);

    /// Resets the normalization factor for the named field of the named document.
    ///
    /// @see #norms(String)
    /// @see Similarity#decodeNorm(byte)
    virtual void setNorm(int32_t doc, const String& field, double value);

    /// Returns an enumeration of all the terms in the index. The enumeration is ordered by
    /// Term::compareTo(). Each term is greater than all that precede it in the enumeration.
    /// Note that after calling terms(), {@link TermEnum#next()} must be called on the resulting
    /// enumeration before calling other methods such as {@link TermEnum#term()}.
    virtual TermEnumPtr terms() = 0;

    /// Returns an enumeration of all terms starting at a given term. If the given term does not
    /// exist, the enumeration is positioned at the first term greater than the supplied term.
    /// The enumeration is ordered by Term::compareTo(). Each term is greater than all that precede
    /// it in the enumeration.
    virtual TermEnumPtr terms(const TermPtr& t) = 0;

    /// Returns the number of documents containing the term t.
    virtual int32_t docFreq(const TermPtr& t) = 0;

    /// Returns an enumeration of all the documents which contain term.  For each document, the
    /// document number, the frequency of the term in that document is also provided, for use in
    /// search scoring.  If term is null, then all non-deleted docs are returned with freq=1.
    /// The enumeration is ordered by document number.  Each document number is greater than all
    /// that precede it in the enumeration.
    virtual TermDocsPtr termDocs(const TermPtr& term);

    /// Returns an unpositioned {@link TermDocs} enumerator.
    virtual TermDocsPtr termDocs() = 0;

    /// Returns an enumeration of all the documents which contain term.  For each document, in
    /// addition to the document number and frequency of the term in that document, a list of all
    /// of the ordinal positions of the term in the document is available.  Thus, this method
    /// positions of the term in the document is available.
    /// This positional information facilitates phrase and proximity searching.
    /// The enumeration is ordered by document number.  Each document number is greater than all
    /// that precede it in the enumeration.
    virtual TermPositionsPtr termPositions(const TermPtr& term);

    /// Returns an unpositioned {@link TermPositions} enumerator.
    virtual TermPositionsPtr termPositions() = 0;

    /// Deletes the document numbered docNum.  Once a document is deleted it will not appear in
    /// TermDocs or TermPostitions enumerations.  Attempts to read its field with the {@link
    /// #document} method will result in an error.  The presence of this document may still be
    /// reflected in the {@link #docFreq} statistic, though this will be corrected eventually as
    /// the index is further modified.
    virtual void deleteDocument(int32_t docNum);

    /// Deletes all documents that have a given term indexed.  This is useful if one uses a
    /// document field to hold a unique ID string for the document.  Then to delete such a
    /// document, one merely constructs a term with the appropriate field and the unique ID string
    /// as its text and passes it to this method.  See {@link #deleteDocument(int)} for information
    /// about when this deletion will become effective.
    /// @return the number of documents deleted
    virtual int32_t deleteDocuments(const TermPtr& term);

    /// Undeletes all documents currently marked as deleted in this index.
    virtual void undeleteAll();

    void flush();

    /// @param commitUserData Opaque Map (String -> String) that's recorded into the segments file
    /// in the index, and retrievable by {@link IndexReader#getCommitUserData}.
    void flush(MapStringString commitUserData);

    /// Commit changes resulting from delete, undeleteAll, or setNorm operations.
    /// If an exception is hit, then either no changes or all changes will have been committed to
    /// the index (transactional semantics).
    void commit(MapStringString commitUserData);

    /// Closes files associated with this index.  Also saves any new deletions to disk.
    /// No other methods should be called after this has been called.
    void close();

    /// Get a list of unique field names that exist in this index and have the specified field option information.
    /// @param fieldOption specifies which field option should be available for the returned fields
    /// @return Collection of Strings indicating the names of the fields.
    virtual HashSet<String> getFieldNames(FieldOption fieldOption) = 0;

    /// Return the IndexCommit that this reader has opened.  This method is only implemented by those
    /// readers that correspond to a Directory with its own segments_N file.
    virtual IndexCommitPtr getIndexCommit();

    /// Prints the filename and size of each file within a given compound file.  Add the -extract flag
    /// to extract files to the current working directory.  In order to make the extracted version of
    /// the index work, you have to copy the segments file from the compound index into the directory
    /// where the extracted files are stored.
    /// @param args Usage: IndexReader [-extract] <cfsfile>
    static void main(Collection<String> args);

    /// Returns all commit points that exist in the Directory.  Normally, because the default is {@link
    /// KeepOnlyLastCommitDeletionPolicy}, there would be only one commit point.  But if you're using a
    /// custom {@link IndexDeletionPolicy} then there could be many commits.  Once you have a given
    /// commit, you can open a reader on it by calling {@link IndexReader#open(IndexCommit,bool)}.
    /// There must be at least one commit in the Directory, else this method throws an exception.
    /// Note that if a commit is in progress while this method is running, that commit may or may not
    /// be returned array.
    static Collection<IndexCommitPtr> listCommits(const DirectoryPtr& dir);

    /// Returns the sequential sub readers that this reader is logically composed of.  For example,
    /// IndexSearcher uses this API to drive searching by one sub reader at a time.  If this reader is
    /// not composed of sequential child readers, it should return null.  If this method returns an empty
    /// array, that means this reader is a null reader (for example a MultiReader that has no sub readers).
    ///
    /// NOTE: You should not try using sub-readers returned by this method to make any changes (setNorm,
    /// deleteDocument, etc.). While this might succeed for one composite reader (like MultiReader), it
    /// will most likely lead to index corruption for other readers (like DirectoryReader obtained
    /// through {@link #open}. Use the parent reader directly.
    virtual Collection<IndexReaderPtr> getSequentialSubReaders();

    virtual LuceneObjectPtr getFieldCacheKey();

    /// This returns null if the reader has no deletions.
    virtual LuceneObjectPtr getDeletesCacheKey();

    /// Returns the number of unique terms (across all fields) in this reader.
    ///
    /// This method returns int64_t, even though internally Lucene cannot handle more than 2^31 unique
    /// terms, for a possible future when this limitation is removed.
    virtual int64_t getUniqueTermCount();

    /// For IndexReader implementations that use TermInfosReader to read terms, this returns the current
    /// indexDivisor as specified when the reader was opened.
    virtual int32_t getTermInfosIndexDivisor();

protected:
    void ensureOpen();

    static IndexReaderPtr open(const DirectoryPtr& directory, const IndexDeletionPolicyPtr& deletionPolicy, const IndexCommitPtr& commit, bool readOnly, int32_t termInfosIndexDivisor);

    /// Implements setNorm in subclass.
    virtual void doSetNorm(int32_t doc, const String& field, uint8_t value) = 0;

    /// Implements deletion of the document numbered docNum.
    /// Applications should call {@link #deleteDocument(int)} or {@link #deleteDocuments(Term)}.
    virtual void doDelete(int32_t docNum) = 0;

    /// Implements actual undeleteAll() in subclass.
    virtual void doUndeleteAll() = 0;

    /// Does nothing by default. Subclasses that require a write lock for index modifications must
    /// implement this method.
    virtual void acquireWriteLock();

    /// Commit changes resulting from delete, undeleteAll, or setNorm operations.
    /// If an exception is hit, then either no changes or all changes will have been committed to
    /// the index (transactional semantics).
    void commit();

    /// Implements commit.
    virtual void doCommit(MapStringString commitUserData) = 0;

    /// Implements close.
    virtual void doClose() = 0;

    friend class DirectoryReader;
    friend class ParallelReader;
};

}

}
#endif
